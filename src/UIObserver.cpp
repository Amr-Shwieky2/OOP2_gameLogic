// UIObserver.cpp
#include "UIObserver.h"
#include <sstream>

UIObserver::UIObserver(sf::Font& font)
    : m_font(font) {
    m_notificationText.setFont(m_font);
    m_notificationText.setCharacterSize(30);
    m_notificationText.setOutlineThickness(2.0f);
    m_notificationText.setOutlineColor(sf::Color::Black);
}

UIObserver::~UIObserver() {
    // Unsubscribe from all events
    // (EventSystem will handle cleanup)
}

void UIObserver::initialize() {
    auto& eventSystem = EventSystem::getInstance();

    // Subscribe to events
    eventSystem.subscribe<ScoreChangedEvent>(
        [this](const ScoreChangedEvent& event) { onScoreChanged(event); }
    );

    eventSystem.subscribe<ItemCollectedEvent>(
        [this](const ItemCollectedEvent& event) { onItemCollected(event); }
    );

    eventSystem.subscribe<PlayerStateChangedEvent>(
        [this](const PlayerStateChangedEvent& event) { onPlayerStateChanged(event); }
    );

    eventSystem.subscribe<EnemyKilledEvent>(
        [this](const EnemyKilledEvent& event) { onEnemyKilled(event); }
    );

    eventSystem.subscribe<PlayerDiedEvent>(
        [this](const PlayerDiedEvent& event) {
        }
    );
}

void UIObserver::update(float dt) {
    m_animationTimer += dt;

    // Update notification lifetimes
    if (!m_notifications.empty()) {
        auto& front = m_notifications.front();
        front.lifetime -= dt;

        if (front.lifetime <= 0) {
            m_notifications.pop();
        }
    }
}

void UIObserver::render(sf::RenderWindow& window) {
    if (!m_notifications.empty()) {
        const auto& notification = m_notifications.front();

        // Position at top center of screen
        m_notificationText.setString(notification.text);
        m_notificationText.setFillColor(notification.color);

        // Center the text
        sf::FloatRect bounds = m_notificationText.getLocalBounds();
        m_notificationText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        m_notificationText.setPosition(window.getSize().x / 2.f, 100.f);

        // Fade out effect
        sf::Color color = notification.color;
        if (notification.lifetime < 0.5f) {
            color.a = static_cast<sf::Uint8>(255 * (notification.lifetime / 0.5f));
            m_notificationText.setFillColor(color);
        }

        // Bounce effect
        float scale = 1.0f + 0.1f * std::sin(m_animationTimer * 10.0f);
        m_notificationText.setScale(scale, scale);

        window.draw(m_notificationText);
    }
}

void UIObserver::onScoreChanged(const ScoreChangedEvent& event) {
    std::stringstream ss;
    ss << "+" << event.delta << " points!";
    addNotification(ss.str(), sf::Color::Yellow);
}

void UIObserver::onItemCollected(const ItemCollectedEvent& event) {
    switch (event.type) {
    case ItemCollectedEvent::ItemType::Coin:
        addNotification("Coin collected!", sf::Color::Yellow);
        break;
    case ItemCollectedEvent::ItemType::Gift:
        addNotification("Gift collected!", sf::Color::Magenta);
        break;
    case ItemCollectedEvent::ItemType::PowerUp:
        addNotification("Power-up!", sf::Color::Cyan);
        break;
    }
}

void UIObserver::onPlayerStateChanged(const PlayerStateChangedEvent& event) {
    if (event.newStateName == "Shielded") {
        addNotification("Shield activated!", sf::Color::Cyan);
    }
    else if (event.newStateName == "Boosted") {
        addNotification("Speed boost!", sf::Color(255, 165, 0)); // Orange
    }
}

void UIObserver::onEnemyKilled(const EnemyKilledEvent& event) {
    addNotification("Enemy defeated!", sf::Color::Red);
}

void UIObserver::addNotification(const std::string& text, sf::Color color) {
    // Only keep one notification at a time for simplicity
    while (!m_notifications.empty()) {
        m_notifications.pop();
    }

    Notification notif;
    notif.text = text;
    notif.lifetime = 2.0f; // Show for 2 seconds
    notif.color = color;

    m_notifications.push(notif);
    m_animationTimer = 0.0f; // Reset animation
}