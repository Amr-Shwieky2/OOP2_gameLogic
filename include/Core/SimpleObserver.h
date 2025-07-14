#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

/**
 * @brief Simple event structure for button interactions
 */
struct ButtonEvent {
    enum Type { CLICKED, HOVERED } type;
    std::string buttonId;
};

/**
 * @brief Observer interface for button events
 * Simple version with single event handler
 */
class ButtonObserver {
public:
    virtual ~ButtonObserver() = default;

    // Handle any button event (click, hover, etc.)
    virtual void onButtonEvent(const ButtonEvent& event) = 0;
};

/**
 * @brief Observable base class for buttons
 * Manages observers and sends notifications
 */
class ButtonObservable {
public:
    // Add observer to notification list
    void addObserver(std::shared_ptr<ButtonObserver> observer) {
        m_observers.push_back(observer);
    }

protected:
    // Notify all observers of button event
    void notifyObservers(const ButtonEvent& event) {
        // Clean up expired observers first
        m_observers.erase(
            std::remove_if(m_observers.begin(), m_observers.end(),
                [](const std::weak_ptr<ButtonObserver>& obs) {
                    return obs.expired();
                }),
            m_observers.end()
        );

        // Send notification to all active observers
        for (auto& weakObs : m_observers) {
            if (auto obs = weakObs.lock()) {
                obs->onButtonEvent(event);
            }
        }
    }

private:
    // Observer list using weak_ptr to prevent circular references
    std::vector<std::weak_ptr<ButtonObserver>> m_observers;
};