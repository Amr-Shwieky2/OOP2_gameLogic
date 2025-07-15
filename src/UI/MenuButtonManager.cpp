#include "../../include/UI/MenuButtonManager.h"
#include "../../include/Application/AppContext.h"
#include <iostream>

//-------------------------------------------------------------------------------------
MenuButtonManager::MenuButtonManager() {
    m_buttonObserver = std::make_shared<MenuButtonObserver>();
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::initializeButtons(const sf::Font& font) {
    clearButtons();
    createAboutButton(font);
    createMenuButtons(font);
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::createAboutButton(const sf::Font& font) {
    auto aboutBtn = ButtonFactory::createAboutButton(
        sf::Vector2f(m_layout.aboutButtonX, m_layout.aboutButtonY),
        sf::Vector2f(m_layout.aboutButtonWidth, m_layout.aboutButtonHeight),
        m_buttonObserver,
        font
    );
    m_buttons.push_back(std::move(aboutBtn));
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::createMenuButtons(const sf::Font& font) {
    const std::vector<ButtonFactory::ButtonType> buttonTypes = {
        ButtonFactory::ButtonType::START,
        ButtonFactory::ButtonType::SETTINGS,
        ButtonFactory::ButtonType::HELP,
        ButtonFactory::ButtonType::EXIT
    };

    for (size_t i = 0; i < buttonTypes.size(); ++i) {
        sf::Vector2f position(
            m_layout.centerX,
            m_layout.startY + (i * m_layout.spacing)
        );

        auto button = ButtonFactory::createButton(
            buttonTypes[i],
            position,
            sf::Vector2f(m_layout.buttonWidth, m_layout.buttonHeight),
            m_buttonObserver,
            font
        );

        if (button) {
            m_buttons.push_back(std::move(button));
        }
    }
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::updateButtons(float deltaTime) {
    for (auto& button : m_buttons) {
        if (button) {
            button->update(deltaTime);
        }
    }
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::renderButtons(sf::RenderWindow& window) {
    for (auto& button : m_buttons) {
        if (button) {
            button->render(window);
        }
    }
}
//-------------------------------------------------------------------------------------
bool MenuButtonManager::handleButtonClick(const sf::Vector2f& mousePos) {
    for (auto& button : m_buttons) {
        if (button && button->handleClick(mousePos)) {
            return true; // Button was clicked
        }
    }
    return false;
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::handleButtonHover(const sf::Vector2f& mousePos) {
    for (auto& button : m_buttons) {
        if (button) {
            button->handleMouseMove(mousePos);
        }
    }
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::setButtonLayout(float centerX, float startY, float spacing) {
    m_layout.centerX = centerX;
    m_layout.startY = startY;
    m_layout.spacing = spacing;
}
//-------------------------------------------------------------------------------------
void MenuButtonManager::clearButtons() {
    m_buttons.clear();
}
//-------------------------------------------------------------------------------------