#pragma once
#include "EventSystem.h"
#include "GameEvents.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <queue>

/**
 * UIObserver - Listens to game events and displays notifications
 */
class UIObserver {
public:
    UIObserver(sf::Font& font);
    ~UIObserver();

    void initialize();
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    struct Notification {
        std::string text;
        float lifetime;
        sf::Color color;
    };

    void onScoreChanged(const ScoreChangedEvent& event);
    void onItemCollected(const ItemCollectedEvent& event);
    void onPlayerStateChanged(const PlayerStateChangedEvent& event);
    void onEnemyKilled(const EnemyKilledEvent& event);

    void addNotification(const std::string& text, sf::Color color = sf::Color::White);

    sf::Font& m_font;
    std::queue<Notification> m_notifications;
    sf::Text m_notificationText;

    // Animation
    float m_animationTimer = 0.0f;
};