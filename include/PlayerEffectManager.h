#pragma once

#include <unordered_map>
#include <chrono>
#include "PlayerEffect.h"

class PlayerEffectManager {
public:
    void applyEffect(PlayerEffect effect, float durationSeconds);
    void update(float deltaTime);

    bool hasEffect(PlayerEffect effect) const;
    float getRemainingTime(PlayerEffect effect) const;

    void clearEffect(PlayerEffect effect);
    void clearAll();

private:
    struct EffectTimer {
        float elapsed = 0.f;
        float duration = 0.f;
    };

    std::unordered_map<PlayerEffect, EffectTimer> m_effects;
};
