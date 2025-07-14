#include "UIOverlay.h"
#include <sstream>

UIOverlay::UIOverlay(float windowWidth) {
    if (!m_font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    m_scoreText.setFont(m_font);
    m_livesText.setFont(m_font);
    m_timerText.setFont(m_font);
    m_pauseLabel.setFont(m_font);

    m_scoreText.setCharacterSize(40);
    m_livesText.setCharacterSize(40);
    m_timerText.setCharacterSize(40);
    m_pauseLabel.setCharacterSize(40);

    m_scoreText.setFillColor(sf::Color(150, 75, 20));
    m_livesText.setFillColor(sf::Color(150, 75, 20));
    m_timerText.setFillColor(sf::Color(150, 75, 20));
    m_pauseLabel.setFillColor(sf::Color(150, 75, 20));

    m_scoreText.setPosition(20.f, 10.f);
    m_livesText.setPosition(240.f, 10.f);
    m_timerText.setPosition(440.f, 10.f);

    m_pauseButton.setSize(sf::Vector2f(140.f, 50.f));
    m_pauseButton.setFillColor(sf::Color(220, 200, 170));
    m_pauseButton.setPosition(windowWidth - 185.f, 10.f);

    m_pauseLabel.setString("Pause");
    m_pauseLabel.setPosition(windowWidth - 160.f, 12.f);
}

void UIOverlay::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
        if (m_pauseButton.getGlobalBounds().contains(mousePos)) {
            m_paused = !m_paused;
        }
    }
}

void UIOverlay::update(int score, int lives) {
    m_scoreText.setString("Score: " + std::to_string(score));
    m_livesText.setString("Lives: " + std::to_string(lives));

    std::ostringstream oss;
    oss << "Time: " << static_cast<int>(m_timer.getElapsedTime().asSeconds());
    m_timerText.setString(oss.str());
}

void UIOverlay::draw(sf::RenderWindow& window) {
    sf::View uiView = window.getDefaultView();
    window.setView(uiView);

    window.draw(m_scoreText);
    window.draw(m_livesText);
    window.draw(m_timerText);
    window.draw(m_pauseButton);
    window.draw(m_pauseLabel);
}

bool UIOverlay::isPaused() const {
    return m_paused;
}

void UIOverlay::reset() {
    m_timer.restart();
}
