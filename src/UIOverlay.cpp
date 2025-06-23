// UIOverlay.cpp (محدث)
#include "UIOverlay.h"
#include "GameState.h"
#include <sstream>

UIOverlay::UIOverlay(float windowWidth) {
    if (!m_font.loadFromFile("arial.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    // Setup all text objects
    m_scoreText.setFont(m_font);
    m_livesText.setFont(m_font);
    m_timerText.setFont(m_font);
    m_levelText.setFont(m_font);
    m_pauseLabel.setFont(m_font);
    m_gameOverText.setFont(m_font);
    m_levelCompleteText.setFont(m_font);

    // Set character sizes
    m_scoreText.setCharacterSize(40);
    m_livesText.setCharacterSize(40);
    m_timerText.setCharacterSize(40);
    m_levelText.setCharacterSize(40);
    m_pauseLabel.setCharacterSize(40);
    m_gameOverText.setCharacterSize(60);
    m_levelCompleteText.setCharacterSize(50);

    // Set colors
    sf::Color gameColor(150, 75, 20);
    m_scoreText.setFillColor(gameColor);
    m_livesText.setFillColor(gameColor);
    m_timerText.setFillColor(gameColor);
    m_levelText.setFillColor(gameColor);
    m_pauseLabel.setFillColor(gameColor);
    m_gameOverText.setFillColor(sf::Color::Red);
    m_levelCompleteText.setFillColor(sf::Color::Green);

    // Set positions
    m_scoreText.setPosition(20.f, 10.f);
    m_livesText.setPosition(240.f, 10.f);
    m_timerText.setPosition(440.f, 10.f);
    m_levelText.setPosition(640.f, 10.f);

    // Setup pause button
    m_pauseButton.setSize(sf::Vector2f(140.f, 50.f));
    m_pauseButton.setFillColor(sf::Color(220, 200, 170));
    m_pauseButton.setPosition(windowWidth - 185.f, 10.f);

    m_pauseLabel.setString("Pause");
    m_pauseLabel.setPosition(windowWidth - 160.f, 12.f);

    // Setup overlay for game over/pause
    m_overlay.setSize(sf::Vector2f(windowWidth, 600.f)); // Adjust height as needed
    m_overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent black
    m_overlay.setPosition(0.f, 0.f);
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

void UIOverlay::update(const GameState& gameState) {
    updateTexts(gameState);
}

void UIOverlay::update(int score, int lives) {
    updateTexts(score, lives);
}

void UIOverlay::updateTexts(const GameState& gameState) {
    // Update score
    m_scoreText.setString("Score: " + std::to_string(gameState.getScore()));
    // Update lives
    m_livesText.setString("Lives: " + std::to_string(gameState.getLives()));
    // Update timer
    std::ostringstream oss;
    oss << "Time: " << static_cast<int>(gameState.getTime());
    m_timerText.setString(oss.str());
    // Update level
    m_levelText.setString("Level: " + std::to_string(gameState.getCurrentLevel() + 1));
    // Update game over text
    if (gameState.isGameOver()) {
        m_gameOverText.setString("GAME OVER\nFinal Score: " + std::to_string(gameState.getScore()));
        centerText(m_gameOverText, 700.f, 300.f);
    }
    // Update level complete text
    if (gameState.isLevelComplete()) {
        m_levelCompleteText.setString("LEVEL COMPLETE!\nScore: " + std::to_string(gameState.getScore()));
        centerText(m_levelCompleteText, 700.f, 300.f);
    }
}

void UIOverlay::updateTexts(int score, int lives, float time, int level) {
    m_scoreText.setString("Score: " + std::to_string(score));
    m_livesText.setString("Lives: " + std::to_string(lives));

    std::ostringstream oss;
    oss << "Time: " << static_cast<int>(time);
    m_timerText.setString(oss.str());

    m_levelText.setString("Level: " + std::to_string(level));
}

void UIOverlay::draw(sf::RenderWindow& window) {
    sf::View uiView = window.getDefaultView();
    window.setView(uiView);

    drawGameplayUI(window);

    if (m_paused) {
        drawPauseOverlay(window);
    }
}

void UIOverlay::draw(sf::RenderWindow& window, const GameState& gameState) {
    sf::View uiView = window.getDefaultView();
    window.setView(uiView);
    drawGameplayUI(window);
    if (gameState.isGameOver()) {
        drawGameOverUI(window);
    }
    else if (gameState.isLevelComplete()) {
        drawLevelCompleteUI(window);
    }
    else if (m_paused) {
        drawPauseOverlay(window);
    }
}

void UIOverlay::drawGameplayUI(sf::RenderWindow& window) {
    window.draw(m_scoreText);
    window.draw(m_livesText);
    window.draw(m_timerText);
    window.draw(m_levelText);
    window.draw(m_pauseButton);
    window.draw(m_pauseLabel);
}

void UIOverlay::drawPauseOverlay(sf::RenderWindow& window) {
    window.draw(m_overlay);
    // Draw pause text
    sf::Text pauseText;
    pauseText.setFont(m_font);
    pauseText.setString("PAUSED\nPress button to continue");
    pauseText.setCharacterSize(48);
    pauseText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = pauseText.getLocalBounds();
    pauseText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    pauseText.setPosition(700.f, 300.f);

    window.draw(pauseText);
}

void UIOverlay::drawGameOverUI(sf::RenderWindow& window) {
    window.draw(m_overlay);
    window.draw(m_gameOverText);

    // Add restart instruction
    sf::Text restartText;
    restartText.setFont(m_font);
    restartText.setString("Press R to restart");
    restartText.setCharacterSize(24);
    restartText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = restartText.getLocalBounds();
    restartText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    restartText.setPosition(700.f, 400.f);

    window.draw(restartText);
}

void UIOverlay::drawLevelCompleteUI(sf::RenderWindow& window) {
    window.draw(m_overlay);
    window.draw(m_levelCompleteText);

    // Add continue instruction
    sf::Text continueText;
    continueText.setFont(m_font);
    continueText.setString("Press SPACE to continue");
    continueText.setCharacterSize(24);
    continueText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = continueText.getLocalBounds();
    continueText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    continueText.setPosition(700.f, 400.f);

    window.draw(continueText);
}

void UIOverlay::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    text.setPosition(x, y);
}

bool UIOverlay::isPaused() const {
    return m_paused;
}