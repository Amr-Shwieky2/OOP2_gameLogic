#pragma once

#include <SFML/Graphics.hpp>

class GameState;

class UIOverlay {
private:
    sf::Font m_font;
    sf::Text m_scoreText;
    sf::Text m_livesText;
    sf::Text m_timerText;
    sf::Text m_levelText;
    sf::Text m_pauseLabel;
    sf::Text m_gameOverText;
    sf::Text m_levelCompleteText;

    sf::RectangleShape m_pauseButton;
    sf::RectangleShape m_overlay; // For game over/pause overlay

    bool m_paused = false;

public:
    UIOverlay(float windowWidth);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(const GameState& gameState);
    void update(int score, int lives);
    void draw(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window, const GameState& gameState); // Enhanced version

    bool isPaused() const;

private:
    void updateTexts(const GameState& gameState);
    void updateTexts(int score, int lives, float time = 0.0f, int level = 1); // Fallback

    void drawGameplayUI(sf::RenderWindow& window);
    void drawGameOverUI(sf::RenderWindow& window);
    void drawLevelCompleteUI(sf::RenderWindow& window);
    void drawPauseOverlay(sf::RenderWindow& window);

    // Helper for centering text
    void centerText(sf::Text& text, float x, float y);
};