#include "../../include/Screens/SettingsScreen.h"
#include "../../include/Application/AppContext.h"
#include "../../include/Settings/SettingsInitializer.h"  

#include <iostream>
#include "GameExceptions.h"
#include <Services/GameExceptions.h>

SettingsScreen::SettingsScreen() {
    try {
        SettingsInitializer::InitResult init = SettingsInitializer::initialize();

        if (!init.success) {
            THROW_INITIALIZATION_EXCEPTION(init.errorMessage);
        }

        m_resourceManager = std::move(init.resourceManager);
        m_uiRenderer = std::move(init.uiRenderer);
        m_commandHandler = std::move(init.commandHandler);
        m_volumePanel = init.volumePanel;

        m_isInitialized = true;

    }
    catch (const std::exception& e) {
        std::cout << "Initialization error: " << e.what() << std::endl;
        m_isInitialized = false;
    }
}

void SettingsScreen::handleEvents(sf::RenderWindow& window) {
    if (!m_isInitialized) return;

    sf::Event event;
    while (window.pollEvent(event)) {

        if (delegateMouseEvents(event)) continue;

        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }

        if (event.type == sf::Event::KeyPressed && m_commandHandler) {
            bool shouldExit = m_commandHandler->handleKeyboardInput(event);
            if (shouldExit) {
                AppContext::instance().screenManager().requestScreenChange(ScreenType::MENU);
                return;
            }
        }
    }
}


void SettingsScreen::update(float deltaTime) {
    if (!m_isInitialized) return;

    try {
        if (m_uiRenderer) m_uiRenderer->updateAnimation(deltaTime);
        if (m_volumePanel) m_volumePanel->update(deltaTime);
    }
    catch (const std::exception& e) {
        std::cout << "Runtime error in Update: " << e.what() << std::endl;
    }
}

void SettingsScreen::render(sf::RenderWindow& window) {
    if (!m_isInitialized) return;

    try {
        if (m_uiRenderer && m_resourceManager) {
            m_uiRenderer->renderBackground(window, m_resourceManager->getBackgroundSprite());
            m_uiRenderer->renderTexts(window);
            m_uiRenderer->renderAnimationEffects(window);
        }

        if (m_volumePanel) m_volumePanel->render(window);

    }
    catch (const std::exception& e) {
        std::cout << "Runtime error in Render: " << e.what() << std::endl;
    }
}

bool SettingsScreen::delegateMouseEvents(const sf::Event& event) {
    if (m_volumePanel) {
        try {
            return m_volumePanel->handleMouseEvent(event);  
        }
        catch (const std::exception& e) {
            std::cout << "Error in VolumeControlPanel: " << e.what() << std::endl;
        }
    }
    return false;
}

void SettingsScreen::onEnter() {
    if (m_uiRenderer) {
        m_uiRenderer->setAnimationSpeed(1.0f);
    }
}

void SettingsScreen::onExit() {
    // Nothing here – autosave handled elsewhere
}

bool SettingsScreen::hasUnsavedChanges() const {
    return m_volumePanel && m_volumePanel->hasChanged();
}

void SettingsScreen::enableAutoSave(bool enable) {
    if (m_commandHandler) {
        m_commandHandler->enableAutoSave(enable);
    }
}

void SettingsScreen::setAnimationSpeed(float speed) {
    if (m_uiRenderer) {
        m_uiRenderer->setAnimationSpeed(speed);
    }
}
