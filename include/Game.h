#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "GameState.h"
#include "SoundManager.h"
#include "Card.h"
#include <vector>

class Game {
public:
    Game();
    void initialize(sf::RenderWindow& window, GameState& state);
    void run(GameState& state);
    sf::Sprite& getBackgroundSprite(); // Corrected to return a reference

    // Make these public
    std::vector<Card> createDeck();
    void shuffleDeck(std::vector<Card>& deck);

private:
    sf::RenderWindow* window;
    sf::View view;
    sf::Sprite backgroundSprite;
    SoundManager soundManager;

    void initializeUIElements(GameState& state);
    void handleResize(int newWidth, int newHeight, GameState& state);
    void handleButtonInputs(const sf::Event& event, GameState& state);
    void updatePrizeTexts(GameState& state, int betAmount, int windowWidth, int windowHeight, int prize);
    void updateCardPositionsAndScales(std::vector<Card>& hand, sf::RenderWindow& window);
};
