#pragma once

#include "Exceptions/Exception.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <optional>

namespace GameExceptions {

/**
 * @brief Recovery suggestion for a specific error
 */
struct RecoverySuggestion {
    std::string message;                   // User-friendly suggestion
    std::string buttonText;                // Text for the recovery action button
    std::function<bool()> recoveryAction;  // Action to take if the user selects this suggestion
    
    RecoverySuggestion(const std::string& msg, const std::string& btn, std::function<bool()> action)
        : message(msg), buttonText(btn), recoveryAction(action) {}
};

/**
 * @brief User-friendly error information
 */
struct UserFriendlyError {
    std::string title;                        // Error title
    std::string message;                      // Detailed message
    std::vector<RecoverySuggestion> suggestions; // Recovery suggestions
    std::string errorCode;                    // Error code for support reference
    
    UserFriendlyError() = default;
    
    UserFriendlyError(const std::string& errorTitle, const std::string& errorMessage)
        : title(errorTitle), message(errorMessage) {}
        
    void addSuggestion(const RecoverySuggestion& suggestion) {
        suggestions.push_back(suggestion);
    }
};

/**
 * @brief Manager for user-friendly error messages
 */
class UserFriendlyErrorManager {
public:
    static UserFriendlyErrorManager& getInstance();
    
    // Register an error message for a specific exception type
    template<typename ExceptionType>
    void registerErrorMessage(const std::string& title, const std::string& message) {
        m_errorMessages[typeid(ExceptionType).name()] = UserFriendlyError(title, message);
    }
    
    // Register an error message for a specific error code
    void registerErrorCodeMessage(int errorCode, const std::string& title, const std::string& message);
    
    // Add a recovery suggestion for an exception type
    template<typename ExceptionType>
    void addRecoverySuggestion(const RecoverySuggestion& suggestion) {
        auto typeName = typeid(ExceptionType).name();
        if (m_errorMessages.find(typeName) == m_errorMessages.end()) {
            m_errorMessages[typeName] = UserFriendlyError();
        }
        m_errorMessages[typeName].addSuggestion(suggestion);
    }
    
    // Add a recovery suggestion for an error code
    void addErrorCodeSuggestion(int errorCode, const RecoverySuggestion& suggestion);
    
    // Get a user-friendly error for an exception
    UserFriendlyError getUserFriendlyError(const std::exception& ex);
    
    // Get a user-friendly error for an error code
    UserFriendlyError getUserFriendlyError(int errorCode);
    
    // Format an exception as a user-friendly message
    std::string formatUserFriendlyMessage(const std::exception& ex);
    
private:
    UserFriendlyErrorManager();
    
    // Map of exception type names to user-friendly errors
    std::unordered_map<std::string, UserFriendlyError> m_errorMessages;
    
    // Map of error codes to user-friendly errors
    std::unordered_map<int, UserFriendlyError> m_errorCodeMessages;
    
    // Get the exception type hierarchy (most specific to least specific)
    std::vector<std::string> getExceptionHierarchy(const std::exception& ex);
};

/**
 * @brief User-friendly error dialog
 * 
 * Shows a dialog with error details and recovery options
 */
class ErrorDialog {
public:
    static ErrorDialog& getInstance();
    
    // Show an error dialog for an exception
    void showError(const std::exception& ex);
    
    // Show an error dialog for an error code
    void showError(int errorCode);
    
    // Show an error dialog with custom error info
    void showError(const UserFriendlyError& error);
    
    // Check if the dialog is currently visible
    bool isVisible() const { return m_visible; }
    
    // Close the dialog
    void close();
    
    // Update and render the dialog
    void update();
    void render(sf::RenderWindow& window);
    
    // Handle events (returns true if the event was handled)
    bool handleEvent(const sf::Event& event);
    
    // Set dialog position and size
    void setPosition(float x, float y) { m_position = sf::Vector2f(x, y); }
    void setSize(float width, float height) { m_size = sf::Vector2f(width, height); }
    
private:
    ErrorDialog();
    
    bool m_visible = false;
    UserFriendlyError m_currentError;
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    
    // Selected suggestion index
    int m_selectedSuggestion = -1;
    
    // Initialize dialog UI elements
    void initializeUI();
};

/**
 * @brief Default recovery actions for common errors
 */
namespace DefaultRecoveryActions {
    // Retry loading a resource
    bool retryResourceLoad(const std::string& resourcePath);
    
    // Reload the current level
    bool reloadCurrentLevel();
    
    // Go to the main menu
    bool goToMainMenu();
    
    // Exit the game
    bool exitGame();
    
    // Enable fallback rendering
    bool enableFallbackRendering();
    
    // Disable advanced graphics
    bool disableAdvancedGraphics();
    
    // Disable audio
    bool disableAudio();
}

} // namespace GameExceptions