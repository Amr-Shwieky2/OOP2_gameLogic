#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declarations
class VolumeControlPanel;


class SettingsEventHandler {
public:
    SettingsEventHandler();
    ~SettingsEventHandler();  // explicit destructor

    // Component registration
    void setVolumePanel(std::shared_ptr<VolumeControlPanel> panel);

    // Event handling 
    void handleEvents(sf::RenderWindow& window);

    // Safe cleanup 
    void cleanup();

private:
    // Event handling methods 
    void handleWindowEvents(sf::RenderWindow& window, const sf::Event& event);
    void handleKeyboardEvents(const sf::Event& event);
    void handleMouseEvents(const sf::Event& event);

    // Specific event handlers 
    void handleKeyPressEvents(const sf::Event& event);
    void handleMouseMoveEvents(const sf::Event& event);
    void handleMousePressEvents(const sf::Event& event);
    void handleMouseReleaseEvents(const sf::Event& event);

    // Key processing 
    void processEscapeKey();

    // Mouse event delegation
    bool delegateMouseEventToComponents(const sf::Event& event);

    // Event validation 
    bool isValidMouseEvent(const sf::Event& event) const;
    bool isValidKeyboardEvent(const sf::Event& event) const;

private:
    // Components 
    std::shared_ptr<VolumeControlPanel> m_volumePanel;

    // State 
    bool m_shouldExitToMenu = false;
};