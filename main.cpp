
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <filesystem>
#include <thread>
#include <random>
#include <unordered_set>
#include "Game.h"
#include "GameState.h"
#include "Deck.h"
#include "Card.h"
#include "SoundManager.h"

const float CARD_SCALE_FACTOR = 0.18f;
const float CARD_SPACING = 10.5f;
const float CARD_BOTTOM_OFFSET = 70.0f;
const float CARD_LEFT_OFFSET = 240.0f;

std::string getAssetPath(const std::string& relativePath) {
    return std::filesystem::current_path().string() + "/Assets/" + relativePath;
}

// Declare prizeMultipliers
std::map<std::string, int> prizeMultipliers = {
    {"Super Royal", 336}, {"Royal Flush", 198}, {"Straight Flush", 134},
    {"Four of a Kind", 72}, {"Full House", 36}, {"Flush", 19},
    {"Straight", 11}, {"Three of a Kind", 7}, {"Two Pair", 3}, {"Jacks or Better", 1}
};

// Function declarations for missing methods
void initializeUIElements(GameState& state, sf::RenderWindow& window);
void updatePrizeTexts(std::vector<std::unique_ptr<sf::Text>>& prizeTexts, int betAmount, const sf::Font& font, float windowWidth, float windowHeight, int currentPrize, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound);
int evaluateHand(const std::vector<Card>& hand, int betAmount);
void updateCardPositionsAndScales(std::vector<Card>& hand, sf::RenderWindow& window);
void initializeGame(GameState& state, sf::RenderWindow& window, sf::Sprite& backgroundSprite, Deck& deck, sf::Sound& prizeSound);
void handleStartGame(GameState& state, bool& roundInProgress, bool& canCollect, int& playerCredits, int& prize, sf::RenderWindow& window, sf::Sound& prizeSound);
void handleBetIncrease(bool& canBet, int& betAmount, int playerCredits, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::RenderWindow& window, sf::Text& betText, int prize, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound);
void handleDealCards(bool& canBet, bool& roundInProgress, int& playerCredits, int betAmount, std::vector<Card>& hand, std::vector<Card>& deck, sf::RenderWindow& window, sf::Text& creditsText, bool& drawFiveCards, bool& canCollect);
void handleCollectPrize(bool& canCollect, int& prize, int betAmount, std::vector<Card>& hand, int& playerCredits, sf::Text& creditsText, bool& canBet, bool& gameOver, bool& gameStarted, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::RenderWindow& window, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound);
void handleButtonInputs(const sf::Event& event, std::vector<Card>& hand, bool& canBet, int& betAmount, bool& canCollect, int& prize, int& playerCredits, bool& drawFiveCards, bool& roundInProgress, GameState& state, sf::RenderWindow& window, sf::Text& creditsText, sf::Text& betText, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::Sound& prizeSound, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers);
void updateUI(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, std::vector<Card>& hand, bool gameOver, sf::Text& gameOverText, int playerCredits, int betAmount, int prize);
void initializeGamblingGame(std::vector<Card>& deck, Card& currentCard); // Declare initializeGamblingGame
bool loadTexture(Card& card, const std::string& filePath);
bool playGamblingRound(std::vector<Card>& deck, Card& currentCard, bool guessHigher, std::unordered_set<int>& usedCards);











// main.cpp

void initializeUIElements(GameState& state, sf::RenderWindow& window) {
    const int characterSize = 24;
    const sf::Color valueColor = sf::Color::Green;

    auto initializeText = [&](std::unique_ptr<sf::Text>& text, const std::string& str, int size, const sf::Color& color) {
        text = std::make_unique<sf::Text>();
        text->setFont(state.font);
        text->setCharacterSize(size);
        text->setFillColor(color);
        text->setString(str);
    };

    initializeText(state.betText, "Bet: 1", characterSize, valueColor);
    initializeText(state.creditsText, "Credits: 10", characterSize, valueColor);
    initializeText(state.gameOverText, "0-INIT", 60, sf::Color(144, 238, 144));
    state.gameOverText->setPosition(window.getSize().x * 0.70f, window.getSize().y * 0.50f + 40);

    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    state.betText->setPosition(windowWidth * 0.05f, windowHeight * 0.1f);
    state.creditsText->setPosition(windowWidth * 0.2f, windowHeight * 0.1f);
}

void updatePrizeTexts(std::vector<std::unique_ptr<sf::Text>>& prizeTexts, int betAmount, const sf::Font& font, float windowWidth, float windowHeight, int currentPrize, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound) {
    prizeTexts.clear();
    std::vector<std::string> prizeNames = {
        "Super Royal", "Royal Flush", "Straight Flush",
        "Four of a Kind", "Full House", "Flush",
        "Straight", "Three of a Kind", "Two Pair", "Jacks or Better"
    };

    float prizeTableStartX = windowWidth * 0.70f;
    float prizeTableStartY = windowHeight * 0.09f;
    float characterSize = 22 * (windowHeight / 600.0f);

    for (size_t i = 0; i < prizeNames.size(); ++i) {
        auto prizeText = std::make_unique<sf::Text>();
        prizeText->setFont(font);
        prizeText->setCharacterSize(characterSize);
        prizeText->setString(prizeNames[i] + " : " + std::to_string(prizeMultipliers.at(prizeNames[i]) * betAmount));
        prizeText->setPosition(prizeTableStartX, prizeTableStartY + i * (24 * (windowHeight / 600.0f)));

        if (currentPrize == prizeMultipliers.at(prizeNames[i]) * betAmount) {
            prizeText->setFillColor(sf::Color::Green);
            prizeSound.play();
        } else {
            prizeText->setFillColor(sf::Color(255, 102, 0));
        }

        prizeTexts.push_back(std::move(prizeText));
    }
}

int evaluateHand(const std::vector<Card>& hand, int betAmount) {
    if (hand.size() != 5) return 0;

    std::map<char, int> rankCount;
    std::map<char, int> suitCount;
    std::vector<int> rankValues;
    std::set<char> uniqueRanks;
    std::map<char, int> rankToValue = {
        {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
        {'T', 10}, {'J', 11}, {'Q', 12}, {'K', 13}, {'A', 14}
    };

    for (const auto& card : hand) {
        rankCount[card.rank]++;
        suitCount[card.suit]++;
        rankValues.push_back(rankToValue[card.rank]);
        uniqueRanks.insert(card.rank);
    }

    std::sort(rankValues.begin(), rankValues.end());

    bool isFlush = (suitCount.size() == 1);
    bool isStraight = (uniqueRanks.size() == 5) &&
                      (((int)rankValues[4] - (int)rankValues[0] == 4) ||
                       (rankValues == std::vector<int>{2, 3, 4, 5, 14}));

    if (isFlush && uniqueRanks == std::set<char>{'T', 'J', 'Q', 'K', 'A'}) {
        return (hand[0].suit == 'H' ? 336 : 198) * betAmount;
    }

    if (isFlush && isStraight) {
        return 134 * betAmount;
    }

    for (const auto& pair : rankCount) {
        if (pair.second == 4) return 72 * betAmount;
    }

    bool threeOfAKind = false, pairFound = false;
    for (const auto& pair : rankCount) {
        if (pair.second == 3) threeOfAKind = true;
        if (pair.second == 2) pairFound = true;
    }
    if (threeOfAKind && pairFound) return 36 * betAmount;
    if (isFlush) return 19 * betAmount;
    if (isStraight) return 11 * betAmount;
    for (const auto& pair : rankCount) {
        if (pair.second == 3) return 7 * betAmount;
    }
    int pairCount = 0;
    for (const auto& pair : rankCount) {
        if (pair.second == 2) pairCount++;
    }
    if (pairCount == 2) return 3 * betAmount;
    if (rankCount['J'] == 2 || rankCount['Q'] == 2 || rankCount['K'] == 2 || rankCount['A'] == 2) return 1 * betAmount;

    return 0;
}

void updateCardPositionsAndScales(std::vector<Card>& hand, sf::RenderWindow& window) {
    for (auto& card : hand) {
        float scaleFactor = (window.getSize().y / 600.0f) * CARD_SCALE_FACTOR;
        card.sprite->setScale(scaleFactor, scaleFactor); // Dereference the unique_ptr

        // Ensure the card is positioned within the window's bounds
        float xPos = std::min(CARD_LEFT_OFFSET + (&card - &hand[0]) * (card.sprite->getGlobalBounds().width + CARD_SPACING), window.getSize().x - card.sprite->getGlobalBounds().width);
        float yPos = std::min(window.getSize().y - CARD_BOTTOM_OFFSET - card.sprite->getGlobalBounds().height, window.getSize().y - card.sprite->getGlobalBounds().height);

        card.sprite->setPosition(xPos, yPos); // Dereference the unique_ptr
    }
}

void initializeGame(GameState& state, sf::RenderWindow& window, sf::Sprite& backgroundSprite, Deck& deck, sf::Sound& prizeSound) {
    static sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile(getAssetPath("images/fire_girl.png"))) {
        std::cerr << "Failed to load background texture" << std::endl;
        throw std::runtime_error("Failed to load background texture");
    }
    backgroundSprite.setTexture(backgroundTexture);
    deck.createDeck(); // Ensure createDeck method exists in Deck class
    deck.shuffle();    // Ensure shuffle method exists in Deck class
    state.canBet = true;
    state.gameStarted = false;
    state.mainGameHand.clear();
    state.drawFiveCards = false;
}

void handleStartGame(GameState& state, bool& roundInProgress, bool& canCollect, int& playerCredits, int& prize, sf::RenderWindow& window, sf::Sound& prizeSound) {
    state.gameStarted = true;
    roundInProgress = true;
    canCollect = false;
    state.gameOver = false;
    playerCredits = 10;
    prize = 0;
    updatePrizeTexts(state.prizeTexts, state.betAmount, state.font, window.getSize().x, window.getSize().y, 0, prizeMultipliers, prizeSound);
}

void handleBetIncrease(bool& canBet, int& betAmount, int playerCredits, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::RenderWindow& window, sf::Text& betText, int prize, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound) {
    if (canBet) {
        if (betAmount < std::min(playerCredits, 5)) {
            betAmount++;
        } else {
            betAmount = 1;
        }
        updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers, prizeSound);
        betText.setString("Bet: " + std::to_string(betAmount));
    }
}

void handleDealCards(bool& canBet, bool& roundInProgress, int& playerCredits, int betAmount, std::vector<Card>& hand, std::vector<Card>& deck, sf::RenderWindow& window, sf::Text& creditsText, bool& drawFiveCards, bool& canCollect) {
    if (canBet && playerCredits >= betAmount) {
        playerCredits -= betAmount;
        creditsText.setString("Credits: " + std::to_string(playerCredits)); // Use dot operator instead of arrow operator
        hand.clear();
        for (int i = 0; i < 5; ++i) {
            hand.push_back(std::move(deck.back()));
            deck.pop_back();
        }
        updateCardPositionsAndScales(hand, window);
        drawFiveCards = false;
        canCollect = true;
        canBet = false;
        roundInProgress = false;
    }
}

void handleCollectPrize(bool& canCollect, int& prize, int betAmount, std::vector<Card>& hand, int& playerCredits, sf::Text& creditsText, bool& canBet, bool& gameOver, bool& gameStarted, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::RenderWindow& window, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound) {
    if (canCollect) {
        prize = evaluateHand(hand, betAmount);
        playerCredits += prize;
        creditsText.setString("Credits: " + std::to_string(playerCredits)); // Use dot operator instead of arrow operator
        canCollect = false;
        canBet = true;
        gameOver = (playerCredits <= 0);
        if (gameOver) {
            gameStarted = false; // Reset gameStarted here
        } else {
            updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers, prizeSound); // Pass necessary parameters
        }
    }
}




void handleButtonInputs(const sf::Event& event, std::vector<Card>& hand, bool& canBet, int& betAmount, bool& canCollect, int& prize, int& playerCredits, bool& drawFiveCards, bool& roundInProgress, bool& gameOver, GameState& state, sf::RenderWindow& window, sf::Sound& prizeSound, sf::Text& betText, const sf::Font& font, const std::map<std::string, int>& prizeMultipliers, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, sf::Text& creditsText, std::vector<Card>& deck) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::S:
                if (!state.gameStarted) {
                    handleStartGame(state, roundInProgress, canCollect, playerCredits, prize, window, prizeSound);
                }
                break;
            case sf::Keyboard::B:
                handleBetIncrease(canBet, betAmount, playerCredits, prizeTexts, window, betText, prize, font, prizeMultipliers, prizeSound);
                break;
            case sf::Keyboard::D:
                if (roundInProgress) {
                    handleDealCards(canBet, roundInProgress, playerCredits, betAmount, hand, deck, window, creditsText, drawFiveCards, canCollect);
                }
                break;
            case sf::Keyboard::C:
                handleCollectPrize(canCollect, prize, betAmount, hand, playerCredits, creditsText, canBet, gameOver, state.gameStarted, prizeTexts, window, font, prizeMultipliers, prizeSound);
                break;
            default:
                break;
        }
    }
}




void updateUI(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, std::vector<Card>& hand, bool gameOver, sf::Text& gameOverText, int playerCredits, int betAmount, int prize) {
    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    float characterSize = 24 * (height / 600.0f); // Adjust character size dynamically

    creditsLabelText.setCharacterSize(characterSize);
    creditsLabelText.setPosition(10.f, height - 140.f);

    creditsValueText.setCharacterSize(characterSize);
    creditsValueText.setString(std::to_string(playerCredits));
    creditsValueText.setPosition(10.f + creditsLabelText.getLocalBounds().width + 5.f, height - 140.f);

    betLabelText.setCharacterSize(characterSize);
    betLabelText.setPosition(10.f, height - 90.f);

    betValueText.setCharacterSize(characterSize);
    betValueText.setString(std::to_string(betAmount));
    betValueText.setPosition(10.f + betLabelText.getLocalBounds().width + 5.f, height - 90.f);

    instructions.setPosition((width - instructions.getLocalBounds().width) / 2.f, 10.f);
    gameOverText.setPosition((width - gameOverText.getLocalBounds().width) / 2.f, height / 2.f);

    float prizeTableHeight = 60.f;
    for (size_t i = 0; i < prizeTexts.size(); ++i) {
        prizeTexts[i]->setCharacterSize(22 * (height / 600.0f));
        prizeTexts[i]->setPosition(width * 0.70f, prizeTableHeight + i * (24 * (height / 600.0f))); // Reduced gap further
    }

    float prizeValueX = width * 0.70f; // Adjusted to match prize table
    float prizeValueY = height * 0.50f; // Adjusted to move further downwards

    for (auto& prizeText : prizeTexts) {
        if (prizeText->getString().toAnsiString().find("Prize:") != std::string::npos) {
            prizeText->setPosition(prizeValueX, prizeValueY);
        } else if (prizeText->getString().toAnsiString() == std::to_string(prize)) {
            prizeText->setFillColor(sf::Color(144, 238, 144)); // Light green color for prize value
            prizeText->setPosition(prizeValueX + prizeText->getLocalBounds().width + 20, prizeValueY + 40); // Move further to the right and downwards
        }
    }

    window.clear();
    window.draw(backgroundSprite);
    window.draw(instructions);
    window.draw(creditsLabelText);
    window.draw(creditsValueText);
    window.draw(betLabelText);
    window.draw(betValueText);
    for (const auto& prizeText : prizeTexts) {
        window.draw(*prizeText); // Dereference unique_ptr
    }
    for (const auto& card : hand) {
        window.draw(*card.sprite); // Dereference unique_ptr
    }
    if (gameOver) {
        window.draw(gameOverText);
    }
    window.display();
}



















int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "SoftyPoker");
    GameState state;

    if (!state.font.loadFromFile(getAssetPath("fonts/arialnbi.ttf"))) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    try {
        SoundManager soundManager;
        soundManager.initializeAllSounds();
        soundManager.playRandomBackgroundMusic();

        // Ensure prizeSound is loaded
        sf::Sound& prizeSound = soundManager.prizeSound;

        Game game;
        Deck deck;  // Create a Deck object
        initializeGame(state, window, game.getBackgroundSprite(), deck, prizeSound);  // Pass the Deck object and prizeSound
        game.run(state);

    } catch (const std::exception& e) {
        std::cerr << "Sound initialization error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}




void initializeGamblingGame(std::vector<Card>& deck, Card& currentCard) {
    Game game;
    deck = game.createDeck(); // Use the createDeck method from the Game class
    game.shuffleDeck(deck); // Use the shuffleDeck method from the Game class
    currentCard = deck.back();
    deck.pop_back();

    // Set the initial position and scale for the card
    currentCard.sprite->setPosition(200, 100); // Example position, adjust as needed
    currentCard.sprite->setScale(0.13f, 0.13f); // Example scale, adjust as needed

    // Debug statement to confirm texture loading
    if (!currentCard.texture) {
        std::cerr << "Failed to load current card texture" << std::endl;
    }
}





bool playGamblingRound(std::vector<Card>& deck, Card& currentCard, bool guessHigher, std::unordered_set<int>& usedCards) {
    if (deck.empty()) {
        initializeGamblingGame(deck, currentCard); // Reshuffle the deck if empty
        usedCards.clear(); // Clear used cards when deck is reshuffled
    }

    Card nextCard;
    int attempts = 0;
    const int maxAttempts = deck.size(); // Maximum attempts based on deck

    do {
        if (deck.empty() || attempts >= maxAttempts) {
            initializeGamblingGame(deck, currentCard); // Reshuffle the deck
            usedCards.clear(); // Clear used cards for reuse
            attempts = 0;
        }
        nextCard = deck.back();
        deck.pop_back();
        attempts++;
    } while (usedCards.find(nextCard.getRankAsInt()) != usedCards.end());

    int currentRank = currentCard.getRankAsInt();
    int nextRank = nextCard.getRankAsInt();

    bool correctGuess = guessHigher ? (nextRank > currentRank) : (nextRank < currentRank);

    usedCards.insert(currentCard.getRankAsInt());
    currentCard = nextCard; // Update current card to next card

    return correctGuess;
}






















void initializeSounds(GameState& state, sf::Sound& cardDealSound, sf::Sound& heldSound, sf::Sound& unheldSound) {
    if (!state.cardDealBuffer.loadFromFile(getAssetPath("sounds/deal.wav"))) {
        std::cerr << "Failed to open sound file '" + getAssetPath("sounds/deal.wav") + "'\n";
        throw std::runtime_error("Failed to load deal.wav");
    }
    cardDealSound.setBuffer(state.cardDealBuffer);
    cardDealSound.setVolume(100);

    if (!state.heldBuffer.loadFromFile(getAssetPath("sounds/hold.wav"))) {
        std::cerr << "Failed to open sound file '" + getAssetPath("sounds/hold.wav") + "'\n";
        throw std::runtime_error("Failed to load hold.wav");
    }
    heldSound.setBuffer(state.heldBuffer);
    heldSound.setVolume(100);

    if (!state.unheldBuffer.loadFromFile(getAssetPath("sounds/unheld.wav"))) {
        std::cerr << "Failed to open sound file '" + getAssetPath("sounds/unheld.wav") + "'\n";
        throw std::runtime_error("Failed to load unheld.wav");
    }
    unheldSound.setBuffer(state.unheldBuffer);
    unheldSound.setVolume(100);
}






































void SoundManager::initializeAllSounds() {
    std::vector<std::pair<std::string, sf::Sound&>> soundFiles = {
        {getAssetPath("sounds/cardDeal.wav"), cardDealSound},
        {getAssetPath("sounds/held.wav"), heldSound},
        {getAssetPath("sounds/unheld.wav"), unheldSound},
        {getAssetPath("sounds/prize.wav"), prizeSound},
        {getAssetPath("sounds/count.wav"), countSound},
        {getAssetPath("sounds/lose.wav"), loseSound},
        {getAssetPath("sounds/win.wav"), winSound}
    };

    std::vector<sf::SoundBuffer> buffers(soundFiles.size());
    for (size_t i = 0; i < soundFiles.size(); i++) {
        if (!buffers[i].loadFromFile(soundFiles[i].first)) {
            std::cerr << "Failed to load sound: " << soundFiles[i].first << std::endl;
            throw std::runtime_error("Failed to initialize sound.");
        }
        soundFiles[i].second.setBuffer(buffers[i]);
        soundFiles[i].second.setVolume(100.0f); // Set volume for each sound
        std::cout << "[Debug] Sound initialized: " << soundFiles[i].first << std::endl;
    }

    // Load background music tracks
    auto music1 = std::make_unique<sf::Music>();
    auto music2 = std::make_unique<sf::Music>();
    auto music3 = std::make_unique<sf::Music>();
    auto music4 = std::make_unique<sf::Music>();
    auto music5 = std::make_unique<sf::Music>();

    if (!music1->openFromFile(getAssetPath("music/music1.ogg"))) {
        throw std::runtime_error("Failed to load music 1");
    }
    if (!music2->openFromFile(getAssetPath("music/music2.ogg"))) {
        throw std::runtime_error("Failed to load music 2");
    }
    if (!music3->openFromFile(getAssetPath("music/music3.ogg"))) {
        throw std::runtime_error("Failed to load music 3");
    }
    if (!music4->openFromFile(getAssetPath("music/music4.ogg"))) {
        throw std::runtime_error("Failed to load music 4");
    }
    if (!music5->openFromFile(getAssetPath("music/music5.ogg"))) {
        throw std::runtime_error("Failed to load music 5");
    }

    musicTracks.push_back(std::move(music1));
    musicTracks.push_back(std::move(music2));
    musicTracks.push_back(std::move(music3));
    musicTracks.push_back(std::move(music4));
    musicTracks.push_back(std::move(music5));

    for (const auto& music : musicTracks) {
        music->setVolume(100.0f); // Set volume for each music track
    }
}





void SoundManager::playRandomBackgroundMusic() {
    if (!musicTracks.empty()) {
        // Select a random music track
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, musicTracks.size() - 1);

        currentMusic = musicTracks[distr(gen)].get();
        currentMusic->setLoop(true);
        currentMusic->play();
        std::cout << "[Debug] Playing random background music" << std::endl;
    }
}










void SoundManager::playSound(const std::string& soundName) {
    std::cout << "[Debug] Playing sound: " << soundName << std::endl;
    if (soundName == "cardDeal") cardDealSound.play();
    else if (soundName == "held") heldSound.play();
    else if (soundName == "unheld") unheldSound.play();
    else if (soundName == "prize") prizeSound.play();
    else if (soundName == "count") countSound.play();
    else if (soundName == "lose") loseSound.play();
    else if (soundName == "win") winSound.play();
}















void updateUI(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, std::vector<Card>& hand, sf::Text& gameOverText, bool gameOver, int playerCredits, int betAmount, int prize) {
    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    float characterSize = 24 * (height / 600.0f); // Adjust character size dynamically

    creditsLabelText.setCharacterSize(characterSize);
    creditsLabelText.setPosition(10.f, height - 140.f);

    creditsValueText.setCharacterSize(characterSize);
    creditsValueText.setString(std::to_string(playerCredits));
    creditsValueText.setPosition(10.f + creditsLabelText.getLocalBounds().width + 5.f, height - 140.f);

    betLabelText.setCharacterSize(characterSize);
    betLabelText.setPosition(10.f, height - 90.f);

    betValueText.setCharacterSize(characterSize);
    betValueText.setString(std::to_string(betAmount));
    betValueText.setPosition(10.f + betLabelText.getLocalBounds().width + 5.f, height - 90.f);

    instructions.setPosition((width - instructions.getLocalBounds().width) / 2.f, 10.f);
    gameOverText.setPosition((width - gameOverText.getLocalBounds().width) / 2.f, height / 2.f);

    float prizeTableHeight = 60.f;
    for (size_t i = 0; i < prizeTexts.size(); ++i) {
        prizeTexts[i]->setCharacterSize(22 * (height / 600.0f));
        prizeTexts[i]->setPosition(width * 0.70f, prizeTableHeight + i * (24 * (height / 600.0f))); // Reduced gap further
    }

    float prizeValueX = width * 0.70f; // Adjusted to match prize table
    float prizeValueY = height * 0.50f; // Adjusted to move further downwards

    for (auto& prizeText : prizeTexts) {
        if (prizeText->getString().toAnsiString().find("Prize:") != std::string::npos) {
            prizeText->setPosition(prizeValueX, prizeValueY);
        } else if (prizeText->getString().toAnsiString() == std::to_string(prize)) {
            prizeText->setFillColor(sf::Color(144, 238, 144)); // Light green color for prize value
            prizeText->setPosition(prizeValueX + prizeText->getLocalBounds().width + 20, prizeValueY + 40); // Move further to the right and downwards
        }
    }

    window.clear();
    window.draw(backgroundSprite);
    window.draw(instructions);
    window.draw(creditsLabelText);
    window.draw(creditsValueText);
    window.draw(betLabelText);
    window.draw(betValueText);
    for (const auto& prizeText : prizeTexts) {
        window.draw(*prizeText); // Dereference unique_ptr
    }
    for (const auto& card : hand) {
        window.draw(*card.sprite); // Dereference unique_ptr
    }
    if (gameOver) {
        window.draw(gameOverText);
    }
    window.display();
}











void updatePrizeValue(sf::Text& prizeValueOnlyText, int currentPrize) {
    prizeValueOnlyText.setString(std::to_string(currentPrize) + "-INIT"); // Update string while retaining identifier
}







void resetGameState(GameState& state, int& playerCredits, int& betAmount, bool& canCollect, bool& drawFiveCards, std::vector<Card>& hand, std::vector<Card>& deck, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, const sf::Font& font, sf::RenderWindow& window, int& prize, sf::Text& betText, sf::Text& creditsText, bool& gameStarted, bool& roundInProgress, bool& gameOver, bool& gamblingPhase, SoundManager& soundManager) {
    std::cout << "[Debug] resetGameState called" << std::endl;

    Game game; // Declare the Game object

    playerCredits = 10;
    betAmount = 1;
    canCollect = false;
    drawFiveCards = true;
    hand.clear();
    deck = game.createDeck();  // Use Game instance to create the deck
    game.shuffleDeck(deck);    // Use Game instance to shuffle the deck
    prize = 0;
    gameStarted = true;
    roundInProgress = false;
    gameOver = false;
    gamblingPhase = false;
    state.canBet = true;  // Use state.canBet instead of canBet

    updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, state.prizeMultipliers, soundManager.prizeSound);

    creditsText.setString("Credits: " + std::to_string(playerCredits));
    std::cout << "[Debug] creditsText updated to " << creditsText.getString().toAnsiString() << std::endl;
    betText.setString("Bet: " + std::to_string(betAmount));

    std::cout << "[Debug] resetGameState completed" << std::endl;
}







void startGame(sf::RenderWindow& window, const sf::Font& font, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, int betAmount, const std::map<std::string, int>& prizeMultipliers, sf::Sound& prizeSound, bool& canBet) {
    float windowWidth = static_cast<float>(window.getSize().x);
    float windowHeight = static_cast<float>(window.getSize().y);

    int prize = 0; // Declare and initialize the prize variable

    updatePrizeTexts(prizeTexts, betAmount, font, windowWidth, windowHeight, prize, prizeMultipliers, prizeSound);
    canBet = false;
}




float calculateScaleFactor(const sf::RenderWindow& window) {
    return static_cast<float>(window.getSize().y) / 1080.0f; // Base on window height
}









void handleBetPlacement(const sf::Event& event, bool& canBet, int& betAmount, int& playerCredits, std::vector<std::unique_ptr<sf::Text>>& prizeTexts, const sf::Font& font, sf::Text& betText, sf::RenderWindow& window, SoundManager& soundManager, const std::map<std::string, int>& prizeMultipliers) {
    if (event.key.code == sf::Keyboard::B && canBet && playerCredits > 0) {
        betAmount = (betAmount % 5) + 1;  // Handle B key cycling
        int prize = 0; // Declare and initialize the prize variable
        updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers, soundManager.prizeSound); // Ensure prizeMultipliers are used
        betText.setString("Bet: " + std::to_string(betAmount));
        std::cout << "[Debug] New Bet: " << betAmount << std::endl;
    }
}
























void dealInitialHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsLabelText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts, sf::Sound& cardDealSound, bool& canBet) {
    hand.clear();
    float windowHeight = window.getSize().y;

    // Deactivate the B key
    canBet = false;

    for (int i = 0; i < 5; ++i) {
        hand.push_back(deck.back());
        deck.pop_back();
        hand.back().isHeld = false;  // Reset the isHeld state

        float scaleFactor = (windowHeight / 600.0f) * CARD_SCALE_FACTOR;
        hand.back().sprite->setScale(scaleFactor, scaleFactor);

        float xPos = CARD_LEFT_OFFSET + i * (hand.back().sprite->getGlobalBounds().width + CARD_SPACING);
        float yPos = windowHeight - CARD_BOTTOM_OFFSET - hand.back().sprite->getGlobalBounds().height;
        hand.back().sprite->setPosition(xPos, yPos);

        cardDealSound.play();

        window.clear();
        window.draw(backgroundSprite);
        window.draw(creditsLabelText);
        window.draw(betText);
        for (const auto& prizeText : prizeTexts) {
            window.draw(prizeText);
        }
        for (int j = 0; j <= i; ++j) {
            window.draw(*hand[j].sprite);
        }
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}




void redrawHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsLabelText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts, sf::Sound& cardDealSound, bool& canBet) {
    float windowHeight = window.getSize().y;

    for (int i = 0; i < 5; ++i) {
        if (!hand[i].isHeld) {
            hand[i] = std::move(deck.back()); // Use std::move to move the card
            deck.pop_back();

            float scaleFactor = (windowHeight / 600.0f) * CARD_SCALE_FACTOR;
            hand[i].sprite->setScale(scaleFactor, scaleFactor);

            float xPos = CARD_LEFT_OFFSET + i * (hand[i].sprite->getGlobalBounds().width + CARD_SPACING);
            float yPos = windowHeight - CARD_BOTTOM_OFFSET - hand[i].sprite->getGlobalBounds().height;
            hand[i].sprite->setPosition(xPos, yPos);

            cardDealSound.play();

            window.clear();
            window.draw(backgroundSprite);
            window.draw(creditsLabelText);
            window.draw(betText);
            for (const auto& prizeText : prizeTexts) {
                window.draw(prizeText);
            }
            for (int j = 0; j < 5; ++j) {
                window.draw(*hand[j].sprite);
            }
            window.display();

            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Smooth transition handling
        }
    }

    // Activate the B key after the redraw function
    canBet = true;
}






void drawHeldCardHighlight(sf::RenderWindow& window, const Card& card) {
    sf::RectangleShape highlight(sf::Vector2f(card.sprite->getGlobalBounds().width, card.sprite->getGlobalBounds().height));
    highlight.setPosition(card.sprite->getPosition());
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(sf::Color::Green);
    highlight.setOutlineThickness(5);
    window.draw(highlight);
}










std::vector<Card> createGamblingDeck() {
    std::vector<Card> deck;
    const char highRanks[] = {'8', '9', 'T', 'J', 'Q', 'K', 'A'};
    const char lowRanks[] = {'2', '3', '4', '5', '6', '7'};
    const char suits[] = {'H', 'D', 'C', 'S'};

    for (char suit : suits) {
        for (char rank : highRanks) {
            Card card;
            card.suit = suit;
            card.rank = rank;
            std::string filePath = getAssetPath("cards/" + std::string(1, rank) + std::string(1, suit) + ".png");
            if (loadTexture(card, filePath)) {
                deck.push_back(card);
            }
        }
        for (char rank : lowRanks) {
            Card card;
            card.suit = suit;
            card.rank = rank;
            std::string filePath = getAssetPath("cards/" + std::string(1, rank) + std::string(1, suit) + ".png");
            if (loadTexture(card, filePath)) {
                deck.push_back(card);
            }
        }
    }
    return deck;
}



void startGamblingPhase(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsText, sf::Text& betText, std::vector<sf::Text>& prizeTexts, const sf::Font& font, int& prize, int& playerCredits, std::vector<Card>& deck, std::vector<Card>& hand, sf::Sound& loseSound, sf::Sound& winSound, sf::Sound& countSound, bool& canBet) {

    static std::vector<Card> gamblingDeck;
    static Card currentGamblingCard;
    static bool isInitialized = false;
    static std::unordered_set<int> usedCards;

    if (!isInitialized) {
        initializeGamblingGame(gamblingDeck, currentGamblingCard);
        isInitialized = true;
    }

    bool gamblingActive = true;
    bool showCard = false;

    float windowHeight = window.getSize().y;
    float scaleFactor = (windowHeight / 600.0f) * CARD_SCALE_FACTOR;
    updateCardPositionsAndScales(gamblingDeck, window);
    currentGamblingCard.sprite->setScale(scaleFactor, scaleFactor); // Dereference the unique_ptr

    if (hand.size() >= 3) {
        currentGamblingCard.sprite->setPosition(hand[2].sprite->getPosition()); // Dereference the unique_ptr
    }

    while (gamblingActive && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::KeyPressed) {
                bool correctGuess = false;
                if (event.key.code == sf::Keyboard::Left) {
                    correctGuess = playGamblingRound(gamblingDeck, currentGamblingCard, false, usedCards);
                } else if (event.key.code == sf::Keyboard::Right) {
                    correctGuess = playGamblingRound(gamblingDeck, currentGamblingCard, true, usedCards);
                } else if (event.key.code == sf::Keyboard::C) {
                    int transferStep = 1;
                    sf::Clock clock;

                    while (prize > 0) {
                        prize -= transferStep;
                        playerCredits += transferStep;
                        countSound.play();

                        creditsText.setString("Credits: " + std::to_string(playerCredits));

                        window.clear();
                        window.draw(backgroundSprite);
                        window.draw(creditsText);
                        window.draw(betText);

                        if (showCard) {
                            window.draw(*currentGamblingCard.sprite); // Dereference the unique_ptr
                        }
                        window.display();

                        while (clock.getElapsedTime().asMilliseconds() < 200) {
                        }
                        clock.restart();
                    }
                    gamblingActive = false;
                    canBet = true;
                    return;
                }

                showCard = true;
                if (!correctGuess) {
                    loseSound.play();
                    prize = 0;
                    gamblingActive = false;  // Ends the gambling on incorrect guess
                } else {
                    winSound.play();
                    prize *= 2;  // Continues gambling with doubled prize
                }
            }
        }

        // Update UI for gambling phase, excluding prize table
        window.clear();
        window.draw(backgroundSprite);
        window.draw(creditsText);
        window.draw(betText);
        if (showCard) {
            window.draw(*currentGamblingCard.sprite); // Dereference the unique_ptr
        }
        window.display();
    }
}




