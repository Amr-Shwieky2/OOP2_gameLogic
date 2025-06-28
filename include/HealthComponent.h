#pragma once
#include "Component.h"

/**
 * HealthComponent - Manages entity health and damage
 * Replaces PlayerStats and enemy health management
 */
class HealthComponent : public Component {
public:
    HealthComponent(int maxHealth);

    // Health management
    void takeDamage(int amount);
    void heal(int amount);
    void setHealth(int health);

    int getHealth() const { return m_currentHealth; }
    int getMaxHealth() const { return m_maxHealth; }
    bool isAlive() const { return m_currentHealth > 0; }

    // Invulnerability (for shields, etc.)
    void setInvulnerable(bool invulnerable) { m_invulnerable = invulnerable; }
    bool isInvulnerable() const { return m_invulnerable; }

private:
    int m_maxHealth;
    int m_currentHealth;
    bool m_invulnerable = false;
};