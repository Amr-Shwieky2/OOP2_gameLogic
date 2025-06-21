#pragma once

#include <SFML/Graphics.hpp>

class UIOverlay {
public:
    UIOverlay(float windowWidth);

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(int score, int lives);
    void draw(sf::RenderWindow& window);

    bool isPaused() const;

private:
    sf::Font m_font;
    sf::Text m_scoreText;
    sf::Text m_livesText;
    sf::Text m_timerText;
    sf::Text m_pauseLabel;

    sf::RectangleShape m_pauseButton;
    sf::Clock m_timer;
    bool m_paused = false;
};
