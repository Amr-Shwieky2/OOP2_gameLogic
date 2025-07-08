#pragma once
#include <SFML/Graphics.hpp>

class PlayerEntity;

/**
 * PlayerVisualEffects - Single Responsibility: Handle player visual effects
 */
class PlayerVisualEffects {
public:
    PlayerVisualEffects(PlayerEntity& player);

    void update(float dt);
    void applyRollRotation(float dt);
    void startDamageEffect();
    void setStateColor(const sf::Color& color);
    void resetVisuals();

    bool canTakeDamage() const { return m_damageTimer <= 0.0f; }
    void startDamageCooldown();

private:
    PlayerEntity& m_player;
    float m_damageTimer = 0.0f;
    float m_damageCooldown = 1.0f;

    void updateDamageEffect(float dt);
    void updateVisualSync();
};
