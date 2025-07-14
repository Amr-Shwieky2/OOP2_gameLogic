#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "ObservableButton.h"
#include "MenuButtonObserver.h"
#include "ButtonFactory.h"

/**
 * @brief Responsible for button creation, layout, and management only
 * Single Responsibility: Button lifecycle management
 */
class MenuButtonManager {
public:
    MenuButtonManager();
    ~MenuButtonManager() = default;

    // Button setup and configuration
    void initializeButtons(const sf::Font& font);
    void updateButtons(float deltaTime);
    void renderButtons(sf::RenderWindow& window);

    // Button interaction (delegates to buttons)
    bool handleButtonClick(const sf::Vector2f& mousePos);
    void handleButtonHover(const sf::Vector2f& mousePos);

    // Configuration
    void setButtonLayout(float centerX, float startY, float spacing);

private:
    // Button collection
    std::vector<std::unique_ptr<ObservableButton>> m_buttons;
    std::shared_ptr<MenuButtonObserver> m_buttonObserver;

    // Layout configuration
    struct ButtonLayout {
        float centerX = 980.0f;
        float startY = 200.0f;
        float spacing = 150.0f;
        float buttonWidth = 300.0f;
        float buttonHeight = 100.0f;
        float aboutButtonWidth = 130.0f;
        float aboutButtonHeight = 100.0f;
        float aboutButtonX = 30.0f;
        float aboutButtonY = 650.0f;
    } m_layout;

    // Button creation helpers
    void createMenuButtons(const sf::Font& font);
    void createAboutButton(const sf::Font& font);
    void clearButtons();
};