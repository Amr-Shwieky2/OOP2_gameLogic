#include "ScreenManager.h"

void ScreenManager::registerScreen(ScreenType type, std::function<std::unique_ptr<IScreen>()> creator) {
    m_creators[type] = std::move(creator);
}

void ScreenManager::changeScreen(ScreenType type) {
    auto it = m_creators.find(type);
    if (it != m_creators.end()) {
        m_currentScreen = it->second();
    }
}

void ScreenManager::handleEvents(sf::RenderWindow& window) {
    if (m_currentScreen) {
        m_currentScreen->handleEvents(window);
    }
}

void ScreenManager::update(float deltaTime) {
    if (m_currentScreen) {
        m_currentScreen->update(deltaTime);
    }
}

void ScreenManager::render(sf::RenderWindow& window) {
    if (m_currentScreen) {
        m_currentScreen->render(window);
    }
}