#pragma once

#include <unordered_map>
#include <SFML/System/Clock.hpp>

enum class PlayerEffect {
    None,
    SpeedBoost,
    Shield,
    ReverseControl,
    Headwind,
    Magnetic,
    Transparent
};

class PlayerEffectManager {
public:
    void applyEffect(PlayerEffect effect, float durationSeconds);
    void update(float deltaTime);

    bool hasEffect(PlayerEffect effect) const;
    float getRemainingTime(PlayerEffect effect) const;

    void clearEffect(PlayerEffect effect);
    void clearAll();

private:
    std::unordered_map<PlayerEffect, sf::Clock> m_activeEffects;
    std::unordered_map<PlayerEffect, float> m_durations;
};
