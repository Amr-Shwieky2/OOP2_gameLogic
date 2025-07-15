#include "../../include/Screens/MenuScreen.h"
#include <iostream>
#include <Application/AppContext.h>
#include "GameExceptions.h"
#include <Services/GameExceptions.h>
#include "../Core/AudioManager.h"

MenuScreen::MenuScreen() {
    std::cout << "MenuScreen: Creating with SRP-compliant architecture..." << std::endl;

    if (!initializeComponents()) {
        m_isInitialized = false;
        std::cout << "MenuScreen: Failed to initialize - some features may not work" << std::endl;
        return;
    }

    m_isInitialized = true;
    std::cout << "MenuScreen: Initialized successfully with specialized components" << std::endl;
    AudioManager::instance().pauseMusic();
    AudioManager::instance().resumeMusic();
}

bool MenuScreen::initializeComponents() {
    try {
        // Create specialized components
        m_buttonManager = std::make_unique<MenuButtonManager>();
        m_animator = std::make_unique<MenuAnimationController>();
        m_eventHandler = std::make_unique<MenuEventHandler>();
        m_renderer = std::make_unique<MenuRenderer>();

        // Initialize components
        if (!m_renderer->initializeResources()) {
            THROW_INITIALIZATION_EXCEPTION("Failed to initialize renderer resources");
        }

        m_buttonManager->initializeButtons(m_renderer->hasValidResources() ?
            AppContext::instance().getFont("arial.ttf") : sf::Font());

        return true;

    }
    catch (const std::exception& e) {
        handleInitializationError("Component initialization", e.what());
        return false;
    }
}

void MenuScreen::handleEvents(sf::RenderWindow& window) {
    if (!areComponentsValid()) return;

    try {
        // Delegate event handling to specialized component
        m_eventHandler->handleEvents(window, *m_buttonManager);
    }
    catch (const std::exception& e) {
        std::cout << "MenuScreen: Error in event handling: " << e.what() << std::endl;
    }
}

void MenuScreen::update(float deltaTime) {
    if (!areComponentsValid()) return;

    try {
        // Update specialized components
        m_animator->update(deltaTime);
        m_buttonManager->updateButtons(deltaTime);
    }
    catch (const std::exception& e) {
        std::cout << "MenuScreen: Error in update: " << e.what() << std::endl;
    }
}

void MenuScreen::render(sf::RenderWindow& window) {
    if (!areComponentsValid()) return;

    try {
        // Delegate rendering to specialized components
        m_renderer->renderBackground(window);
        m_renderer->renderButtons(window, *m_buttonManager);
    }
    catch (const std::exception& e) {
        std::cout << "MenuScreen: Error in rendering: " << e.what() << std::endl;
    }
}

void MenuScreen::onEnter() {
    if (m_animator) {
        m_animator->resetAnimations();
    }

    if (m_eventHandler) {
        m_eventHandler->setEnabled(true);
    }
}

void MenuScreen::onExit() {
    if (m_eventHandler) {
        m_eventHandler->setEnabled(false);
    }
}

void MenuScreen::setAnimationSpeed(float speed) {
    if (m_animator) {
        m_animator->setAnimationSpeed(speed);
    }
}

void MenuScreen::enableAnimations(bool enable) {
    if (m_animator) {
        m_animator->enableGlowEffect(enable);
    }
}

bool MenuScreen::areComponentsValid() const {
    return m_isInitialized &&
        m_buttonManager &&
        m_animator &&
        m_eventHandler &&
        m_renderer;
}

void MenuScreen::handleInitializationError(const std::string& component, const std::string& error) {
    std::cout << "MenuScreen: Error initializing " << component << ": " << error << std::endl;
}