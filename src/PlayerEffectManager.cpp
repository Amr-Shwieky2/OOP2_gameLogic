#include "PlayerEffectManager.h"
#include <algorithm>

void PlayerEffectManager::applyEffect(PlayerEffect effect, float durationSeconds) {
    m_effects[effect] = EffectTimer{ 0.f, durationSeconds };
}

void PlayerEffectManager::update(float deltaTime) {
    for (auto it = m_effects.begin(); it != m_effects.end(); ) {
        it->second.elapsed += deltaTime;
        if (it->second.elapsed >= it->second.duration) {
            it = m_effects.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool PlayerEffectManager::hasEffect(PlayerEffect effect) const {
    return m_effects.find(effect) != m_effects.end();
}

float PlayerEffectManager::getRemainingTime(PlayerEffect effect) const {
    auto it = m_effects.find(effect);
    if (it != m_effects.end()) {
        return std::max(0.f, it->second.duration - it->second.elapsed);
    }
    return 0.f;
}

void PlayerEffectManager::clearEffect(PlayerEffect effect) {
    m_effects.erase(effect);
}

void PlayerEffectManager::clearAll() {
    m_effects.clear();
}
