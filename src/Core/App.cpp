#include "App.h"
#include "Exceptions/Logger.h"
#include "Exceptions/GameExceptions.h"
#include <iostream>
#include <format>

App::App() {
    // Initialize logger with both console and file output
    auto& logger = GameExceptions::getLogger();
    logger.addTarget(std::make_unique<GameExceptions::FileLogTarget>("auto"));
    logger.info("Game starting");
    
    // Create window
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(1400, 800),
        "Desert Ball",
        sf::Style::Default  // Enables minimize, maximize, and close buttons
    );

    m_window->setFramerateLimit(60);
    
    try {
        // Initialize game screen
        m_screen = std::make_unique<GameplayScreen>();
        logger.info("Game screen initialized successfully");
    }
    catch (const std::exception& ex) {
        logger.logException(ex, GameExceptions::LogLevel::Critical);
        throw GameExceptions::Exception(
            "Failed to initialize game screen",
            ex,
            GameExceptions::Exception::Severity::Fatal
        );
    }
}

void App::run() {
    try {
        GameExceptions::getLogger().info("Starting main game loop");
        mainLoop();
    }
    catch (const GameExceptions::Exception& ex) {
        GameExceptions::getLogger().logException(ex);
        
        // Display error message to user if possible
        if (m_window && m_window->isOpen()) {
            displayErrorMessage(ex.what());
        }
    }
    catch (const std::exception& ex) {
        GameExceptions::getLogger().logException(ex);
        
        // Display error message for standard exceptions
        if (m_window && m_window->isOpen()) {
            displayErrorMessage(std::format("Error: {}", ex.what()));
        }
    }
    catch (...) {
        GameExceptions::getLogger().critical("Unknown exception in main loop");
        
        // Display generic error for unknown exceptions
        if (m_window && m_window->isOpen()) {
            displayErrorMessage("An unknown error occurred. Please restart the game.");
        }
    }
    
    GameExceptions::getLogger().info("Game exiting");
}

void App::mainLoop() {
    sf::Clock clock;

    while (m_window->isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (m_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_window->close();
            }
            else if (event.type == sf::Event::Resized) {
                // Automatically adjust the view to match new window size
                sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height));
                m_window->setView(sf::View(visibleArea));
            }
        }

        try {
            m_screen->handleEvents(*m_window);
            m_screen->update(dt);

            m_window->clear();
            m_screen->render(*m_window);
            m_window->display();
        }
        catch (const std::exception& ex) {
            GameExceptions::getLogger().logException(ex);
            
            // For non-fatal exceptions, log and continue
            if (auto gameEx = dynamic_cast<const GameExceptions::Exception*>(&ex)) {
                if (gameEx->getSeverity() >= GameExceptions::Exception::Severity::Critical) {
                    throw; // Re-throw critical errors
                }
            }
        }
    }
}

void App::displayErrorMessage(const std::string& message) {
    if (!m_window || !m_window->isOpen()) return;

    // Clear window with dark background
    m_window->clear(sf::Color(40, 40, 40));
    
    // Create error text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // If we can't load the font, just give up on visual error messages
        return;
    }
    
    sf::Text errorTitle;
    errorTitle.setFont(font);
    errorTitle.setString("Error");
    errorTitle.setCharacterSize(32);
    errorTitle.setFillColor(sf::Color::Red);
    errorTitle.setPosition(10, 10);
    
    sf::Text errorText;
    errorText.setFont(font);
    errorText.setString(message);
    errorText.setCharacterSize(18);
    errorText.setFillColor(sf::Color::White);
    errorText.setPosition(10, 50);
    
    sf::Text continueText;
    continueText.setFont(font);
    continueText.setString("Press any key to exit...");
    continueText.setCharacterSize(18);
    continueText.setFillColor(sf::Color(200, 200, 200));
    continueText.setPosition(10, m_window->getSize().y - 30);
    
    // Display error and wait for a key press
    m_window->draw(errorTitle);
    m_window->draw(errorText);
    m_window->draw(continueText);
    m_window->display();
    
    // Wait for any key or close event
    sf::Event event;
    while (m_window->isOpen()) {
        while (m_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                event.type == sf::Event::KeyPressed) {
                m_window->close();
                break;
            }
        }
    }
}
