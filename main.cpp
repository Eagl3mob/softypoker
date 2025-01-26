#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <map> // Make sure this line is included
#include "SoundManager.h"
#include "Card.h"
#include "Deck.h"
#include "ButtonInputContext.h"

namespace fs = std::filesystem;

const float CARD_SCALE_FACTOR = 0.18f;
const float CARD_SPACING = 10.5f;
const float CARD_BOTTOM_OFFSET = 70.0f;
const float CARD_LEFT_OFFSET = 240.0f;

bool gameOver = false;
bool roundInProgress = false;
int betAmount = 1, playerCredits = 0, prize = 0;
bool canBet = true, gameStarted = false, drawFiveCards = true, gamblingPhase = false, canCollect = false, canHoldCards = true;
std::vector<Card> hand, deck;
sf::Texture backgroundTexture;
SoundManager soundManager;
bool animateLogoFlag = true;

std::map<std::string, int> prizeMultipliers = {
    {"Super Royal", 336}, {"Royal Flush", 198}, {"Straight Flush", 134},
    {"Four of a Kind", 72}, {"Full House", 36}, {"Flush", 19},
    {"Straight", 11}, {"Three of a Kind", 7}, {"Two Pair", 3}, {"Jacks or Better", 1}
};












std::vector<Card> createDeck();
bool loadTexture(Card& card, const std::string& filePath);
void shuffleDeck(std::vector<Card>& deck);
void dealInitialHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts);
void redrawHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts);
void initializeGamblingGame(std::vector<Card>& deck, Card& currentCard);
std::vector<Card> createGamblingDeck();
void updateCardPositionsAndScales(std::vector<Card>& cards, sf::RenderWindow& window);

void initializeAllSounds(sf::Sound& cardDealSound, sf::Sound& heldSound, sf::Sound& unheldSound, sf::Sound& countSound, sf::Sound& loseSound, sf::Sound& winSound);
void drawHeldCardHighlight(sf::RenderWindow& window, const Card& card);
int evaluateHand(const std::vector<Card>& hand, int betAmount);
bool playGamblingRound(std::vector<Card>& deck, Card& currentCard, bool guessHigher, std::unordered_set<int>& usedCards);
void resetGameState(int& playerCredits, int& betAmount, bool& canCollect, bool& drawFiveCards, std::vector<Card>& hand, std::vector<Card>& deck, std::vector<sf::Text>& prizeTexts, const sf::Font& font, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& prizeValueOnlyText, SoundManager& soundManager);
void startGamblingPhase(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsText, sf::Text& betText, std::vector<sf::Text>& prizeTexts, const sf::Font& font, int& prize, int& playerCredits, sf::Sound& countSound, sf::Sound& loseSound, sf::Sound& winSound, std::vector<Card>& hand, bool& canBet);
void initializePrizeSound(sf::Sound& prizeSound);
float calculateScaleFactor(const sf::RenderWindow& window);
void handleBetPlacement(const sf::Event& event, bool& canBet, int& betAmount, int& playerCredits, std::vector<sf::Text>& prizeTexts, const sf::Font& font, sf::Text& betText, sf::RenderWindow& window, SoundManager& soundManager);

void updateUI(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, std::vector<sf::Text>& prizeTexts, std::vector<Card>& hand, sf::Text& gameOverText, bool gameOver, int playerCredits, int betAmount);

void initializeUIElements(const sf::Font& font, sf::RenderWindow& window, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& prizeValueOnlyText);
void updatePrizeTexts(std::vector<sf::Text>& prizeTexts, int betAmount, const sf::Font& font, float windowWidth, float windowHeight, int currentPrize, const std::map<std::string, int>& prizeMultipliers);
void handleResize(sf::RenderWindow& window, sf::View& view, sf::Sprite& backgroundSprite, sf::Sprite& logoSprite, int newWidth, int newHeight, SoundManager& soundManager, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& creditsLabelText, sf::Text& creditsValueText, std::vector<sf::Text>& prizeTexts, sf::Text& prizeValueOnlyText, sf::Text& instructions);
void animateLogo(sf::Sprite& logoSprite, float speed, float deltaTime, bool& animateLogoFlag, const sf::Vector2u& windowSize);
void startGame(sf::RenderWindow& window, const sf::Font& font, std::vector<sf::Text>& prizeTexts, int betAmount, int prize, const std::map<std::string, int>& prizeMultipliers, bool& canBet);
void handleButtonInputs(const ButtonInputContext& context, sf::RenderWindow& window, bool& animateLogoFlag);









  void startGame(sf::RenderWindow& window, const sf::Font& font, std::vector<sf::Text>& prizeTexts, int betAmount, int prize, const std::map<std::string, int>& prizeMultipliers, bool& canBet) {

    canBet = false; // Disable betting during the game

    // Initialize the prize texts
    updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers);

    // Game logic here
}







int main() {
    fs::current_path("D:/Projects/Softypoker");

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "SoftyPoker");

    sf::Font font;
    if (!font.openFromFile("assets/fonts/arialnbi.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    // Correct initialization and property setting for sf::Text
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setString("Press 'S' to start, 'B' to bet. In Building Phase ©2025 By T.E. & E.M.");
    instructions.setCharacterSize(32);
    instructions.setFillColor(sf::Color::Red);

    sf::Text creditsText;
    creditsText.setFont(font);
    creditsText.setString("Credits: ");
    creditsText.setCharacterSize(30);

    sf::Text betText;
    betText.setFont(font);
    betText.setString("Bet: ");
    betText.setCharacterSize(30);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over");
    gameOverText.setCharacterSize(50);

    sf::Text prizeValueOnlyText;
    prizeValueOnlyText.setFont(font);
    prizeValueOnlyText.setString("Prize: 0");
    prizeValueOnlyText.setCharacterSize(30);

    std::vector<sf::Text> prizeTexts;

    SoundManager soundManager;
    try {
        soundManager.initializeAllSounds();
        soundManager.playRandomBackgroundMusic();
    } catch (const std::exception& e) {
        std::cerr << "Sound initialization error: " << e.what() << std::endl;
        return -1;
    }

    std::vector<sf::Texture> backgroundTextures;
    std::vector<std::string> backgroundPaths = {
        "assets/images/fantasy_girl.png",
        "assets/images/fire_girl.png",
        "assets/images/sofa_girl.png",
        "assets/images/claws_girl.png",
        "assets/images/blond_girl.png",
        "assets/images/skul_girl.png"
    };

    for (const auto& path : backgroundPaths) {
        sf::Texture texture;
        if (!texture.openFromFile(path)) {
            std::cerr << "Failed to load texture from " << path << std::endl;
            return -1;
        }
        backgroundTextures.push_back(texture);
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int randomIndex = std::rand() % backgroundTextures.size();

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTextures[randomIndex]);
    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(window.getSize().y) / backgroundSprite.getGlobalBounds().height
    );
    backgroundSprite.setPosition(0, 0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            handleButtonInputs({event, hand, canBet, betAmount, canCollect, prize, playerCredits, drawFiveCards, roundInProgress, gameOver, deck, window, backgroundSprite, creditsText, betText, gameStarted, prizeTexts, font, soundManager.getCardDealSound(), soundManager.getHeldSound(), soundManager.getUnheldSound(), soundManager.getCountSound(), instructions, gameOverText, gamblingPhase, soundManager, creditsLabelText, creditsValueText, betLabelText, betValueText, prizeValueOnlyText, prizeMultipliers}, window, animateLogoFlag);
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(creditsText);
        window.draw(betText);
        window.draw(instructions);
        window.draw(prizeValueOnlyText);
        window.draw(gameOverText);
        window.display();
    }

    return 0;
}



















void handleButtonInputs(const ButtonInputContext& context, sf::RenderWindow& window, bool& animateLogoFlag) {
    const sf::Event& event = context.event;

    if (event.type == sf::Event::KeyPressed) { // Correct event handling for SFML 3
        if (event.key.code == sf::Keyboard::S) {
            if (!context.gameStarted) {
                context.soundManager.stopBackgroundMusic();
                startGame(context.window, context.font, context.prizeTexts, context.betAmount, context.prize, context.prizeMultipliers, context.canBet);
                context.gameStarted = true;
            } else {
                resetGameState(context.playerCredits, context.betAmount, context.canCollect, context.drawFiveCards, context.hand, context.deck, context.prizeTexts, context.font, context.window, context.backgroundSprite, context.instructions, context.creditsLabelText, context.creditsValueText, context.betLabelText, context.betValueText, context.prizeValueOnlyText, context.soundManager);
                updateUI(window, context.backgroundSprite, context.instructions, context.creditsLabelText, context.creditsValueText, context.betLabelText, context.betValueText, context.prizeTexts, context.hand, context.gameOverText, context.gameOver, context.playerCredits, context.betAmount);
            }
            animateLogoFlag = false;
        }

        if (context.gameStarted && context.canBet && event.key.code == sf::Keyboard::B && context.playerCredits > 0) {
            handleBetPlacement(event, context.canBet, context.betAmount, context.playerCredits, context.prizeTexts, context.font, context.betText, window, context.soundManager);
            context.soundManager.getCardDealSound().play();
            context.betValueText.setString(std::to_string(context.betAmount));
        }
    }
}










void resetGameState(int& playerCredits, int& betAmount, bool& canCollect, bool& drawFiveCards, std::vector<Card>& hand, std::vector<Card>& deck, std::vector<sf::Text>& prizeTexts, const sf::Font& font, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& prizeValueOnlyText, SoundManager& soundManager) {
    playerCredits = 10;
    betAmount = 1;
    canCollect = false;
    drawFiveCards = true;
    hand.clear();
    deck = createDeck();
    shuffleDeck(deck);
    prize = 0;
    gameStarted = true;
    roundInProgress = false;
    gameOver = false;
    gamblingPhase = false;
    canBet = true;

    updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers);

    creditsLabelText.setString("Credits: " + std::to_string(playerCredits));
    betLabelText.setString("Bet: " + std::to_string(betAmount));

    // Update UI elements
    updateUI(window, backgroundSprite, instructions, creditsLabelText, creditsValueText, betLabelText, betValueText, prizeTexts, hand, prizeValueOnlyText, gameOver, playerCredits, betAmount);
}







void initializeUIElements(const sf::Font& font, sf::RenderWindow& window, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& prizeValueOnlyText) {
    betLabelText.setFont(font);
    betLabelText.setString("Bet:");
    betLabelText.setCharacterSize(24);
    betLabelText.setFillColor(sf::Color::White);
    betLabelText.setPosition(10, window.getSize().y - 50);

    betValueText.setFont(font);
    betValueText.setString(std::to_string(1));
    betValueText.setCharacterSize(24);
    betValueText.setFillColor(sf::Color::White);
    betValueText.setPosition(70, window.getSize().y - 50);

    creditsLabelText.setFont(font);
    creditsLabelText.setString("Credits:");
    creditsLabelText.setCharacterSize(24);
    creditsLabelText.setFillColor(sf::Color::White);
    creditsLabelText.setPosition(10, window.getSize().y - 80);

    creditsValueText.setFont(font);
    creditsValueText.setString(std::to_string(10));
    creditsValueText.setCharacterSize(24);
    creditsValueText.setFillColor(sf::Color::White);
    creditsValueText.setPosition(100, window.getSize().y - 80);

    prizeValueOnlyText.setFont(font);
    prizeValueOnlyText.setCharacterSize(24);
    prizeValueOnlyText.setFillColor(sf::Color::Green);
    prizeValueOnlyText.setPosition(window.getSize().x * 0.7f, window.getSize().y * 0.09f);
}








void updatePrizeTexts(std::vector<sf::Text>& prizeTexts, int betAmount, const sf::Font& font, float windowWidth, float windowHeight, int currentPrize, const std::map<std::string, int>& prizeMultipliers) {
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
        sf::Text prizeText;
        prizeText.setFont(font);
        prizeText.setCharacterSize(characterSize);
        prizeText.setString(prizeNames[i] + " : " + std::to_string(prizeMultipliers.at(prizeNames[i]) * betAmount));
        prizeText.setPosition(prizeTableStartX, prizeTableStartY + i * (24 * (windowHeight / 600.0f)));

        if (currentPrize == prizeMultipliers.at(prizeNames[i]) * betAmount) {
            prizeText.setFillColor(sf::Color::Green);
        } else {
            prizeText.setFillColor(sf::Color(255, 102, 0));
        }

        prizeTexts.push_back(prizeText);
    }
}






void updateUI(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& instructions, sf::Text& creditsLabelText, sf::Text& creditsValueText, sf::Text& betLabelText, sf::Text& betValueText, std::vector<sf::Text>& prizeTexts, std::vector<Card>& hand, sf::Text& gameOverText, bool gameOver, int playerCredits, int betAmount) {
    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    float characterSize = 24 * (height / 600.0f); // Adjust character size dynamically

    creditsLabelText.setCharacterSize(characterSize);
    creditsLabelText.setPosition(sf::Vector2f(10.f, height - 140.f)); // Position creditsLabelText

    creditsValueText.setCharacterSize(characterSize);
    creditsValueText.setString(std::to_string(playerCredits));
    creditsValueText.setPosition(sf::Vector2f(10.f + creditsLabelText.getLocalBounds().width + 5.f, height - 140.f)); // Position creditsValueText

    betLabelText.setCharacterSize(characterSize);
    betLabelText.setPosition(sf::Vector2f(10.f, height - 90.f)); // Position betLabelText

    betValueText.setCharacterSize(characterSize);
    betValueText.setString(std::to_string(betAmount));
    betValueText.setPosition(sf::Vector2f(10.f + betLabelText.getLocalBounds().width + 5.f, height - 90.f)); // Position betValueText

    instructions.setPosition(sf::Vector2f((width - instructions.getLocalBounds().width) / 2.f, 10.f)); // Position instructions
    gameOverText.setPosition(sf::Vector2f((width - gameOverText.getLocalBounds().width) / 2.f, height / 2.f)); // Position gameOverText

    float prizeTableHeight = 60.f;
    for (size_t i = 0; i < prizeTexts.size(); ++i) {
        prizeTexts[i].setCharacterSize(22 * (height / 600.0f));
        prizeTexts[i].setPosition(sf::Vector2f(width * 0.70f, prizeTableHeight + i * (24 * (height / 600.0f)))); // Reduced gap further
    }

    float prizeValueX = width * 0.70f;
    float prizeValueY = height * 0.50f;

    for (auto& prizeText : prizeTexts) {
        if (prizeText.getString().toAnsiString().find("Prize:") != std::string::npos) {
            prizeText.setPosition(sf::Vector2f(prizeValueX, prizeValueY));
        } else if (prizeText.getString().toAnsiString() == std::to_string(prize)) {
            prizeText.setFillColor(sf::Color(144, 238, 144)); // Light green color for prize value
            prizeText.setPosition(sf::Vector2f(prizeValueX + prizeText.getLocalBounds().width + 20, prizeValueY + 40)); // Move further to the right and downwards
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
        window.draw(prizeText);
    }
    for (const auto& card : hand) {
        window.draw(card.sprite);
    }
    if (gameOver) {
        window.draw(gameOverText);
    }
    window.display();
}
















void animateLogo(sf::Sprite& logoSprite, float speed, float deltaTime, bool& animateLogoFlag, const sf::Vector2u& windowSize) {
    static sf::Vector2f direction(1.f, 1.f);

    if (!animateLogoFlag) {
        return;
    }

    sf::Vector2f position = logoSprite.getPosition();
    sf::Vector2f movement = direction * speed * deltaTime;

    sf::FloatRect bounds = logoSprite.getGlobalBounds();

    if (position.x + movement.x < 0 || position.x + bounds.width + movement.x > windowSize.x) {
        direction.x = -direction.x;
    }
    if (position.y + movement.y < 0 || position.y + bounds.height + movement.y > windowSize.y) {
        direction.y = -direction.y;
    }

    logoSprite.move(direction * speed * deltaTime);
}














void dealInitialHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsLabelText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts, SoundManager& soundManager) {
    hand.clear();
    float windowHeight = window.getSize().y;

    // Deactivate the B key
    canBet = false;

    for (int i = 0; i < 5; ++i) {
        hand.push_back(deck.back());
        deck.pop_back();
        hand.back().isHeld = false;  // Reset the isHeld state

        float scaleFactor = (windowHeight / 600.0f) * CARD_SCALE_FACTOR;
        hand.back().sprite.setScale(scaleFactor, scaleFactor);

        float xPos = CARD_LEFT_OFFSET + i * (hand.back().sprite.getGlobalBounds().width + CARD_SPACING);
        float yPos = windowHeight - CARD_BOTTOM_OFFSET - hand.back().sprite.getGlobalBounds().height;
        hand.back().sprite.setPosition(xPos, yPos);

        soundManager.getCardDealSound().play();

        window.clear();
        window.draw(backgroundSprite);
        window.draw(creditsLabelText);
        window.draw(betText);
        for (const auto& prizeText : prizeTexts) {
            window.draw(prizeText);
        }
        for (int j = 0; j <= i; ++j) {
            window.draw(hand[j].sprite);
        }
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}




void handleResize(sf::RenderWindow& window, sf::View& view, sf::Sprite& backgroundSprite, sf::Sprite& logoSprite, int newWidth, int newHeight, SoundManager& soundManager, sf::Text& betLabelText, sf::Text& betValueText, sf::Text& creditsLabelText, sf::Text& creditsValueText, std::vector<sf::Text>& prizeTexts, sf::Text& prizeValueOnlyText, sf::Text& instructions) {

    view.setSize(static_cast<float>(newWidth), static_cast<float>(newHeight));
    window.setView(view);

    backgroundSprite.setScale(
        static_cast<float>(newWidth) / backgroundSprite.getGlobalBounds().width,
        static_cast<float>(newHeight) / backgroundSprite.getGlobalBounds().height
    );

    logoSprite.setPosition((newWidth - logoSprite.getGlobalBounds().width) / 2.f, 50.f);

    // Update positions of UI elements as needed
    betLabelText.setPosition(10.f, newHeight - 90.f);
    betValueText.setPosition(10.f + betLabelText.getLocalBounds().width + 5.f, newHeight - 90.f);
    creditsLabelText.setPosition(10.f, newHeight - 140.f);
    creditsValueText.setPosition(10.f + creditsLabelText.getLocalBounds().width + 5.f, newHeight - 140.f);
    prizeValueOnlyText.setPosition(newWidth * 0.70f, 10.f);
    instructions.setPosition((newWidth - instructions.getLocalBounds().width) / 2.f, 10.f);

    // Update prize text positions
    float prizeTableHeight = 60.f;
    for (size_t i = 0; i < prizeTexts.size(); ++i) {
        prizeTexts[i].setPosition(newWidth * 0.70f, prizeTableHeight + i * 30.f); // Adjust spacing as needed
    }
}











void handleBetPlacement(const sf::Event& event, bool& canBet, int& betAmount, int& playerCredits, std::vector<sf::Text>& prizeTexts, const sf::Font& font, sf::Text& betValueText, sf::RenderWindow& window, SoundManager& soundManager) {


    if (event.key.code == sf::Keyboard::B && canBet && playerCredits > 0) {
        betAmount = (betAmount % 5) + 1;
        updatePrizeTexts(prizeTexts, betAmount, font, window.getSize().x, window.getSize().y, prize, prizeMultipliers);
        betText.setString("Bet: " + std::to_string(betAmount));
    }
}









void updatePrizeValue(sf::Text& prizeValueOnlyText, int currentPrize) {
    prizeValueOnlyText.setString(std::to_string(currentPrize) + "-INIT"); // Update string while retaining identifier
}





float calculateScaleFactor(const sf::RenderWindow& window) {
    return static_cast<float>(window.getSize().y) / 1080.0f; // Base on window height
}











std::vector<Card> createDeck() {
    std::vector<Card> deck;
    const char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
    const char suits[] = {'H', 'D', 'C', 'S'};

    for (char suit : suits) {
        for (char rank : ranks) {
            Card card;
            card.suit = suit;
            card.rank = rank;
            std::string filePath = "D:/Programming/Projects/Assets/cards/" + std::string(1, rank) + std::string(1, suit) + ".png";
            if (loadTexture(card, filePath)) {
                deck.push_back(card);
            }
        }
    }
    return deck;
}



bool loadTexture(Card& card, const std::string& filePath) {
    card.texture = new sf::Texture();
    if (!card.texture->loadFromFile(filePath)) {
        delete card.texture;
        card.texture = nullptr;
        return false;
    }
    card.sprite.setTexture(*card.texture);
    return true;
}



void shuffleDeck(std::vector<Card>& deck) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}















void redrawHand(std::vector<Card>& deck, std::vector<Card>& hand, sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsLabelText, sf::Text& betText, const std::vector<sf::Text>& prizeTexts, sf::Sound& cardDealSound) {
    float windowHeight = window.getSize().y;

    for (int i = 0; i < 5; ++i) {
        if (!hand[i].isHeld) {
            hand[i] = deck.back();
            deck.pop_back();

            float scaleFactor = (windowHeight / 600.0f) * CARD_SCALE_FACTOR;
            hand[i].sprite.setScale(scaleFactor, scaleFactor);

            float xPos = CARD_LEFT_OFFSET + i * (hand[i].sprite.getGlobalBounds().width + CARD_SPACING);
            float yPos = windowHeight - CARD_BOTTOM_OFFSET - hand[i].sprite.getGlobalBounds().height;
            hand[i].sprite.setPosition(xPos, yPos);

            cardDealSound.play();

            window.clear();
            window.draw(backgroundSprite);
            window.draw(creditsLabelText);
            window.draw(betText);
            for (const auto& prizeText : prizeTexts) {
                window.draw(prizeText);
            }
            for (int j = 0; j < 5; ++j) {
                window.draw(hand[j].sprite);
            }
            window.display();

            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Smooth transition handling
        }
    }

    // Activate the B key after the redraw function
    canBet = true;
}






void drawHeldCardHighlight(sf::RenderWindow& window, const Card& card) {
    sf::RectangleShape highlight(sf::Vector2f(card.sprite.getGlobalBounds().width, card.sprite.getGlobalBounds().height));
    highlight.setPosition(card.sprite.getPosition());
    highlight.setFillColor(sf::Color::Transparent);
    highlight.setOutlineColor(sf::Color::Green);
    highlight.setOutlineThickness(5);
    window.draw(highlight);
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
        return 134 * betAmount; // Updated value
    }

    for (const auto& pair : rankCount) {
        if (pair.second == 4) return 72 * betAmount; // Updated value
    }

    bool threeOfAKind = false, pairFound = false;
    for (const auto& pair : rankCount) {
        if (pair.second == 3) threeOfAKind = true;
        if (pair.second == 2) pairFound = true;
    }
    if (threeOfAKind && pairFound) return 36 * betAmount;
    if (isFlush) return 19 * betAmount;
    if (isStraight) return 11 * betAmount; // Updated value
    for (const auto& pair : rankCount) {
        if (pair.second == 3) return 7 * betAmount; // Updated value
    }
    int pairCount = 0;
    for (const auto& pair : rankCount) {
        if (pair.second == 2) pairCount++;
    }
    if (pairCount == 2) return 3 * betAmount; // Updated value
    if (rankCount['J'] == 2 || rankCount['Q'] == 2 || rankCount['K'] == 2 || rankCount['A'] == 2) return 1 * betAmount;

    // Re-enable the B key
    canBet = true;

    return 0;
}









void initializeGamblingGame(std::vector<Card>& deck, Card& currentCard) {
    deck = createDeck();
    shuffleDeck(deck);
    currentCard = deck.back();
    deck.pop_back();

    // Set the initial position and scale for the card
    currentCard.sprite.setPosition(200, 100); // Example position, adjust as needed
    currentCard.sprite.setScale(0.13f, 0.13f); // Example scale, adjust as needed

    // Debug statement to confirm texture loading
    if (!currentCard.texture) {
        std::cerr << "Failed to load current card texture" << std::endl;
    }
    // Debug statement to check card position and scale
    // std::cout << "Card Position: " << currentCard.sprite.getPosition().x << ", " << currentCard.sprite.getPosition().y << std::endl;
    // std::cout << "Card Scale: " << currentCard.sprite.getScale().x << ", " << currentCard.sprite.getScale().y << std::endl;
}




void updateCardPositionsAndScales(std::vector<Card>& cards, sf::RenderWindow& window) {
    for (auto& card : cards) {
        float scaleFactor = (window.getSize().y / 600.0f) * CARD_SCALE_FACTOR;
        card.sprite.setScale(scaleFactor, scaleFactor);

        // Ensure the card is positioned within the window's bounds
        float xPos = std::min(CARD_LEFT_OFFSET + (&card - &cards[0]) * (card.sprite.getGlobalBounds().width + CARD_SPACING), window.getSize().x - card.sprite.getGlobalBounds().width);
        float yPos = std::min(window.getSize().y - CARD_BOTTOM_OFFSET - card.sprite.getGlobalBounds().height, window.getSize().y - card.sprite.getGlobalBounds().height);

        card.sprite.setPosition(xPos, yPos);

            // Debug statements to check card position and scale
            // std::cout << "Card Position: " << card.sprite.getPosition().x << ", " << card.sprite.getPosition().y << std::endl;
            // std::cout << "Card Scale: " << card.sprite.getScale().x << ", " << card.sprite.getScale().y << std::endl;
    }
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
            std::string filePath = "D:/Programming/Projects/Assets/cards/" + std::string(1, rank) + std::string(1, suit) + ".png";
            if (loadTexture(card, filePath)) {
                deck.push_back(card);
            }
        }
        for (char rank : lowRanks) {
            Card card;
            card.suit = suit;
            card.rank = rank;
            std::string filePath = "D:/Programming/Projects/Assets/cards/" + std::string(1, rank) + std::string(1, suit) + ".png";
            if (loadTexture(card, filePath)) {
                deck.push_back(card);
            }
        }
    }
    return deck;
}




void startGamblingPhase(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Text& creditsText, sf::Text& betText, std::vector<sf::Text>& prizeTexts, const sf::Font& font, int& prize, int& playerCredits, std::vector<Card>& deck, std::vector<Card>& hand, sf::Sound& loseSound, sf::Sound& winSound, sf::Sound& countSound) {
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
    currentGamblingCard.sprite.setScale(scaleFactor, scaleFactor);

    if (hand.size() >= 3) {
        currentGamblingCard.sprite.setPosition(hand[2].sprite.getPosition());
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
                            window.draw(currentGamblingCard.sprite);
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
            window.draw(currentGamblingCard.sprite);
        }
        window.display();
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





