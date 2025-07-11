#include "Exceptions/UserFriendlyErrors.h"
#include "Exceptions/GameExceptions.h"
#include "Exceptions/Logger.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <format>

// External function declarations - these would be implemented elsewhere in your game
extern GameSession* g_currentSession;
extern bool g_gameRunning;
extern void reloadCurrentLevel();
extern void goToMainMenu();

namespace GameExceptions {

//-------------------------------------------------------------------------
// UserFriendlyErrorManager Implementation
//-------------------------------------------------------------------------

UserFriendlyErrorManager::UserFriendlyErrorManager() {
    // Register common error messages
    registerErrorMessage<ResourceNotFoundException>(
        "Resource Not Found",
        "The game couldn't find a required file. This might be due to incomplete installation or missing game files."
    );
    
    registerErrorMessage<ResourceLoadException>(
        "Resource Load Error",
        "The game couldn't load a required resource. The file might be corrupted or in an unsupported format."
    );
    
    registerErrorCodeMessage(101, "Resource Not Found",
        "A required game file couldn't be found. Please check if the game is properly installed.");
    
    registerErrorCodeMessage(102, "Resource Load Error",
        "A game resource couldn't be loaded. The file might be corrupted or in an unsupported format.");
    
    registerErrorCodeMessage(103, "Invalid Resource Format",
        "A game resource has an invalid format. Please verify the game files or reinstall the game.");
    
    // Add common recovery suggestions
    RecoverySuggestion retrySuggestion(
        "Try again. This might help if the issue was temporary.",
        "Retry", []() { return true; }
    );
    
    RecoverySuggestion reloadSuggestion(
        "Reload the current level. This might resolve gameplay issues.",
        "Reload Level", DefaultRecoveryActions::reloadCurrentLevel
    );
    
    RecoverySuggestion menuSuggestion(
        "Return to the main menu.",
        "Main Menu", DefaultRecoveryActions::goToMainMenu
    );
    
    RecoverySuggestion exitSuggestion(
        "Exit the game. You may want to restart it afterward.",
        "Exit Game", DefaultRecoveryActions::exitGame
    );
    
    // Add suggestions to error types
    addRecoverySuggestion<ResourceNotFoundException>(retrySuggestion);
    addRecoverySuggestion<ResourceLoadException>(retrySuggestion);
    addRecoverySuggestion<ResourceNotFoundException>(exitSuggestion);
    addRecoverySuggestion<ResourceLoadException>(exitSuggestion);
    
    // Add suggestions to error codes
    addErrorCodeSuggestion(101, retrySuggestion);
    addErrorCodeSuggestion(102, retrySuggestion);
    addErrorCodeSuggestion(101, exitSuggestion);
    addErrorCodeSuggestion(102, exitSuggestion);
    
    // Add reload suggestion for gameplay errors
    addErrorCodeSuggestion(201, reloadSuggestion);
    addErrorCodeSuggestion(202, reloadSuggestion);
}

UserFriendlyErrorManager& UserFriendlyErrorManager::getInstance() {
    static UserFriendlyErrorManager instance;
    return instance;
}

void UserFriendlyErrorManager::registerErrorCodeMessage(int errorCode, const std::string& title, const std::string& message) {
    m_errorCodeMessages[errorCode] = UserFriendlyError(title, message);
}

void UserFriendlyErrorManager::addErrorCodeSuggestion(int errorCode, const RecoverySuggestion& suggestion) {
    if (m_errorCodeMessages.find(errorCode) == m_errorCodeMessages.end()) {
        m_errorCodeMessages[errorCode] = UserFriendlyError();
    }
    m_errorCodeMessages[errorCode].addSuggestion(suggestion);
}

UserFriendlyError UserFriendlyErrorManager::getUserFriendlyError(const std::exception& ex) {
    // Try to get the exception hierarchy
    auto exceptionTypes = getExceptionHierarchy(ex);
    
    // Check each type in the hierarchy from most specific to least specific
    for (const auto& typeName : exceptionTypes) {
        auto it = m_errorMessages.find(typeName);
        if (it != m_errorMessages.end()) {
            UserFriendlyError error = it->second;
            
            // Set error code for GameExceptions
            try {
                const Exception& gameEx = dynamic_cast<const Exception&>(ex);
                error.errorCode = std::to_string(gameEx.getErrorCode());
            } catch (const std::bad_cast&) {
                error.errorCode = "Unknown";
            }
            
            return error;
        }
    }
    
    // Default error message if no specific one is found
    UserFriendlyError defaultError("An Error Occurred", ex.what());
    defaultError.errorCode = "Unknown";
    
    // Add default suggestions
    defaultError.addSuggestion(RecoverySuggestion(
        "Try again. This might help if the issue was temporary.",
        "Retry", []() { return true; }
    ));
    
    defaultError.addSuggestion(RecoverySuggestion(
        "Exit the game. You may want to restart it afterward.",
        "Exit Game", DefaultRecoveryActions::exitGame
    ));
    
    return defaultError;
}

UserFriendlyError UserFriendlyErrorManager::getUserFriendlyError(int errorCode) {
    auto it = m_errorCodeMessages.find(errorCode);
    if (it != m_errorCodeMessages.end()) {
        UserFriendlyError error = it->second;
        error.errorCode = std::to_string(errorCode);
        return error;
    }
    
    // Default error message if error code not found
    UserFriendlyError defaultError("An Error Occurred", "An unexpected error occurred.");
    defaultError.errorCode = std::to_string(errorCode);
    
    // Add default suggestions
    defaultError.addSuggestion(RecoverySuggestion(
        "Try again. This might help if the issue was temporary.",
        "Retry", []() { return true; }
    ));
    
    defaultError.addSuggestion(RecoverySuggestion(
        "Exit the game. You may want to restart it afterward.",
        "Exit Game", DefaultRecoveryActions::exitGame
    ));
    
    return defaultError;
}

std::string UserFriendlyErrorManager::formatUserFriendlyMessage(const std::exception& ex) {
    UserFriendlyError error = getUserFriendlyError(ex);
    
    std::stringstream ss;
    ss << error.title << "\n\n";
    ss << error.message << "\n\n";
    
    if (!error.errorCode.empty()) {
        ss << "Error Code: " << error.errorCode << "\n\n";
    }
    
    ss << "Suggestions:\n";
    for (const auto& suggestion : error.suggestions) {
        ss << "- " << suggestion.message << "\n";
    }
    
    return ss.str();
}

std::vector<std::string> UserFriendlyErrorManager::getExceptionHierarchy(const std::exception& ex) {
    std::vector<std::string> hierarchy;
    
    // Add the concrete type
    hierarchy.push_back(typeid(ex).name());
    
    // Try to dynamic_cast to our exception types to determine inheritance
    try {
        const Exception& gameEx = dynamic_cast<const Exception&>(ex);
        hierarchy.push_back(typeid(Exception).name());
        
        try {
            const ResourceException& resEx = dynamic_cast<const ResourceException&>(ex);
            hierarchy.push_back(typeid(ResourceException).name());
            
            try {
                const ResourceNotFoundException& notFoundEx = dynamic_cast<const ResourceNotFoundException&>(ex);
                hierarchy.push_back(typeid(ResourceNotFoundException).name());
            } catch (const std::bad_cast&) {}
            
            try {
                const ResourceLoadException& loadEx = dynamic_cast<const ResourceLoadException&>(ex);
                hierarchy.push_back(typeid(ResourceLoadException).name());
            } catch (const std::bad_cast&) {}
        } catch (const std::bad_cast&) {}
    } catch (const std::bad_cast&) {
        // Not a GameExceptions::Exception, just add std::exception
        hierarchy.push_back(typeid(std::exception).name());
    }
    
    return hierarchy;
}

//-------------------------------------------------------------------------
// ErrorDialog Implementation
//-------------------------------------------------------------------------

ErrorDialog::ErrorDialog() : m_visible(false) {
    // Set default position and size
    m_position = sf::Vector2f(100, 100);
    m_size = sf::Vector2f(600, 400);
    
    // Initialize UI elements
    initializeUI();
}

ErrorDialog& ErrorDialog::getInstance() {
    static ErrorDialog instance;
    return instance;
}

void ErrorDialog::showError(const std::exception& ex) {
    m_currentError = UserFriendlyErrorManager::getInstance().getUserFriendlyError(ex);
    m_visible = true;
    m_selectedSuggestion = -1;
    
    getLogger().info(std::format("Showing error dialog: {}", m_currentError.title));
}

void ErrorDialog::showError(int errorCode) {
    m_currentError = UserFriendlyErrorManager::getInstance().getUserFriendlyError(errorCode);
    m_visible = true;
    m_selectedSuggestion = -1;
    
    getLogger().info(std::format("Showing error dialog for code {}: {}", errorCode, m_currentError.title));
}

void ErrorDialog::showError(const UserFriendlyError& error) {
    m_currentError = error;
    m_visible = true;
    m_selectedSuggestion = -1;
    
    getLogger().info(std::format("Showing error dialog: {}", m_currentError.title));
}

void ErrorDialog::close() {
    m_visible = false;
    getLogger().info("Closed error dialog");
}

void ErrorDialog::update() {
    // Any dialog animations or state updates would go here
    if (!m_visible) {
        return;
    }
}

void ErrorDialog::render(sf::RenderWindow& window) {
    if (!m_visible) {
        return;
    }
    
    // Create a semi-transparent background overlay
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 192));
    window.draw(overlay);
    
    // Draw dialog background
    sf::RectangleShape dialogBg(m_size);
    dialogBg.setPosition(m_position);
    dialogBg.setFillColor(sf::Color(50, 50, 50, 230));
    dialogBg.setOutlineColor(sf::Color(200, 200, 200));
    dialogBg.setOutlineThickness(2.0f);
    window.draw(dialogBg);
    
    // In a real implementation, we would draw all the text and UI elements here
    // For now we'll just draw directly to the console for demonstration purposes
    std::cout << "\n===== ERROR DIALOG =====\n";
    std::cout << "Title: " << m_currentError.title << "\n";
    std::cout << "Message: " << m_currentError.message << "\n";
    std::cout << "Error Code: " << m_currentError.errorCode << "\n\n";
    std::cout << "Suggestions:\n";
    for (size_t i = 0; i < m_currentError.suggestions.size(); ++i) {
        std::cout << i + 1 << ". " << m_currentError.suggestions[i].message << "\n";
    }
    std::cout << "======================\n\n";
}

bool ErrorDialog::handleEvent(const sf::Event& event) {
    if (!m_visible) {
        return false;
    }
    
    // Handle keyboard/mouse events
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
            close();
            return true;
        }
        else if (event.key.code == sf::Keyboard::Return) {
            // Execute the selected suggestion's recovery action
            if (m_selectedSuggestion >= 0 && 
                m_selectedSuggestion < static_cast<int>(m_currentError.suggestions.size())) {
                
                const auto& suggestion = m_currentError.suggestions[m_selectedSuggestion];
                if (suggestion.recoveryAction) {
                    bool result = suggestion.recoveryAction();
                    getLogger().info(std::format("Executed recovery action '{}': {}", 
                                               suggestion.buttonText,
                                               result ? "Success" : "Failed"));
                    
                    if (result) {
                        close();
                    }
                }
                return true;
            }
        }
    }
    
    return false; // Event not handled
}

void ErrorDialog::initializeUI() {
    // In a real implementation, we would create text, buttons, etc.
    // For this example, we'll just log that UI was initialized
    getLogger().debug("Initialized error dialog UI");
}

//-------------------------------------------------------------------------
// DefaultRecoveryActions Implementation
//-------------------------------------------------------------------------

namespace DefaultRecoveryActions {

bool retryResourceLoad(const std::string& resourcePath) {
    getLogger().info(std::format("Retrying resource load: {}", resourcePath));
    
    // This is just a placeholder. In a real implementation, you would
    // have code to retry loading the specific resource.
    return false;
}

bool reloadCurrentLevel() {
    getLogger().info("Reloading current level");
    
    if (g_currentSession) {
        g_currentSession->reloadCurrentLevel();
        return true;
    }
    
    return false;
}

bool goToMainMenu() {
    getLogger().info("Going to main menu");
    
    // Call the external function
    ::goToMainMenu();
    return true;
}

bool exitGame() {
    getLogger().info("Exiting game");
    
    // Set global flag to exit
    g_gameRunning = false;
    return true;
}

bool enableFallbackRendering() {
    getLogger().info("Enabling fallback rendering mode");
    
    // This is a placeholder. In a real implementation, you would
    // have code to switch to a simpler rendering mode.
    return true;
}

bool disableAdvancedGraphics() {
    getLogger().info("Disabling advanced graphics");
    
    // This is a placeholder. In a real implementation, you would
    // have code to disable advanced graphics features.
    return true;
}

bool disableAudio() {
    getLogger().info("Disabling audio");
    
    // This is a placeholder. In a real implementation, you would
    // have code to disable audio.
    return true;
}

} // namespace DefaultRecoveryActions

} // namespace GameExceptions