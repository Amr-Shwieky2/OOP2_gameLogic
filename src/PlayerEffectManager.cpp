#include "PlayerEffectManager.h"

void PlayerEffectManager::applyEffect(PlayerEffect effect, float durationSeconds) {
    m_activeEffects[effect].restart();
    m_durations[effect] = durationSeconds;
}

void PlayerEffectManager::update(float /*deltaTime*/) {
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end(); ) {
        PlayerEffect effect = it->first;
        float duration = m_durations[effect];
        if (it->second.getElapsedTime().asSeconds() >= duration) {
            it = m_activeEffects.erase(it);
            m_durations.erase(effect);
        }
        else {
            ++it;
        }
    }
}

bool PlayerEffectManager::hasEffect(PlayerEffect effect) const {
    return m_activeEffects.find(effect) != m_activeEffects.end();
}

float PlayerEffectManager::getRemainingTime(PlayerEffect effect) const {
    auto it = m_activeEffects.find(effect);
    if (it != m_activeEffects.end()) {
        float duration = m_durations.at(effect);
        float elapsed = it->second.getElapsedTime().asSeconds();
        return std::max(0.f, duration - elapsed);
    }
    return 0.f;
}

void PlayerEffectManager::clearEffect(PlayerEffect effect) {
    m_activeEffects.erase(effect);
    m_durations.erase(effect);
}

void PlayerEffectManager::clearAll() {
    m_activeEffects.clear();
    m_durations.clear();
}
