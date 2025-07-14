#include "ObservableButton.h"
#include "MenuButtonObserver.h"
#include <memory>
#include <SFML/Graphics.hpp>

class MenuButtonObserver;

/**
 * @brief Factory Pattern for creating standardized buttons
 *
 * Provides static methods to create different types of buttons with
 * consistent styling, behavior, and configuration. Eliminates code
 * duplication and ensures all buttons follow the same setup pattern.
 */
class ButtonFactory {
public:
    // Factory methods for specific button types
    static std::unique_ptr<ObservableButton> createAboutButton(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

    static std::unique_ptr<ObservableButton> createStartButton(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

    static std::unique_ptr<ObservableButton> createSettingsButton(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

    static std::unique_ptr<ObservableButton> createHelpButton(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

    static std::unique_ptr<ObservableButton> createExitButton(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

    // Generic factory method with enum
    enum class ButtonType {
        ABOUT,
        START,
        SETTINGS,
        HELP,
        EXIT
    };

    static std::unique_ptr<ObservableButton> createButton(
        ButtonType type,
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font);

private:
    // Helper method to avoid code duplication
    static void setupButton(
        ObservableButton& button,
        std::shared_ptr<MenuButtonObserver> observer,
        const sf::Font& font,
        const std::string& texturePath,
        const sf::Color& fallbackColor);

    // Prevent instantiation (static-only class)
    ButtonFactory() = delete;
    ~ButtonFactory() = delete;
    ButtonFactory(const ButtonFactory&) = delete;
    ButtonFactory& operator=(const ButtonFactory&) = delete;
};
