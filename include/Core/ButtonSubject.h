#pragma once
#include "IButtonObserver.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

// Observer Pattern: Subject manages observers and notifies them of events
class ButtonSubject {
public:
    // Add observer to notification list
    void addObserver(std::shared_ptr<IButtonObserver> observer) {
        m_observers.push_back(observer);
        std::cout << "Observer added. Total observers: " << m_observers.size() << std::endl;
    }

    // Remove observer from list
    void removeObserver(std::shared_ptr<IButtonObserver> observer) {
        m_observers.erase(
            std::remove_if(m_observers.begin(), m_observers.end(),
                [&observer](const std::weak_ptr<IButtonObserver>& weak) {
                    return weak.expired() || weak.lock() == observer;
                }),
            m_observers.end()
        );
    }

protected:
    // Notify all observers when button clicked
    void notifyClicked(const std::string& buttonId) {
        std::cout << "Notifying observers: Button '" << buttonId << "' clicked" << std::endl;
        cleanupObservers();

        for (auto& weakObs : m_observers) {
            if (auto obs = weakObs.lock()) {
                obs->onButtonClicked(buttonId);
            }
        }
    }

    // Notify all observers when button hovered
    void notifyHovered(const std::string& buttonId) {
        std::cout << "Notifying observers: Button '" << buttonId << "' hovered" << std::endl;
        cleanupObservers();

        for (auto& weakObs : m_observers) {
            if (auto obs = weakObs.lock()) {
                obs->onButtonHovered(buttonId);
            }
        }
    }

private:
    // Observer list - weak_ptr prevents circular references
    std::vector<std::weak_ptr<IButtonObserver>> m_observers;

    // Clean up expired observers
    void cleanupObservers() {
        m_observers.erase(
            std::remove_if(m_observers.begin(), m_observers.end(),
                [](const std::weak_ptr<IButtonObserver>& obs) {
                    return obs.expired();
                }),
            m_observers.end()
        );
    }
};