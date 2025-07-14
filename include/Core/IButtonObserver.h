#pragma once
#include <string>

/**
 * @brief Observer interface for button events
 *
 * Pure virtual interface that defines the contract for objects
 * that want to receive notifications about button interactions.
 *
 * Classes implementing this interface can register with ButtonSubject
 * to receive callbacks when button events occur.
 */
class IButtonObserver {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     * Ensures derived classes are destroyed correctly when accessed through base pointer
     */
    virtual ~IButtonObserver() = default;

    /**
     * @brief Called when a button is clicked
     * @param buttonId Unique identifier of the clicked button
     *
     * Implement this method to define what happens when user clicks a button.
     * Common actions: change screen, play sound, update game state
     */
    virtual void onButtonClicked(const std::string& buttonId) = 0;

    /**
     * @brief Called when mouse hovers over a button
     * @param buttonId Unique identifier of the hovered button
     *
     * Implement this method to provide visual/audio feedback on hover.
     * Common actions: highlight button, play hover sound, show tooltip
     */
    virtual void onButtonHovered(const std::string& buttonId) = 0;
};