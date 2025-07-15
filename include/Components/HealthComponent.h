#pragma once
#include "Component.h"

/**
 * @class HealthComponent
 * @brief Manages health-related logic for an entity, including damage, healing, and invulnerability.
 *
 * This component tracks the current and maximum health of an entity.
 * It also supports invulnerability flags (e.g. for shields, power-ups) to ignore damage temporarily.
 * Suitable for both player and enemy entities.
 */
class HealthComponent : public Component {
public:
    /**
     * @brief Constructs a HealthComponent with a specified maximum health.
     * @param maxHealth The maximum health value for the entity.
     */
    explicit HealthComponent(int maxHealth);

    /**
     * @brief Reduces the current health by the specified amount.
     *        Damage is ignored if the entity is invulnerable.
     * @param amount The amount of damage to apply.
     */
    void takeDamage(int amount);

    /**
     * @brief Increases current health by the specified amount, up to the maximum.
     * @param amount The amount of health to restore.
     */
    void heal(int amount);

    /**
     * @brief Sets the current health directly, clamped between 0 and maxHealth.
     * @param health The new health value.
     */
    void setHealth(int health);

    /**
     * @brief Gets the current health of the entity.
     * @return The current health value.
     */
    int getHealth() const;

    /**
     * @brief Gets the maximum health value.
     * @return The maximum health of the entity.
     */
    int getMaxHealth() const;

    /**
     * @brief Checks if the entity is still alive (health > 0).
     * @return True if alive, false if dead.
     */
    bool isAlive() const;

    /**
     * @brief Enables or disables invulnerability.
     * @param invulnerable True to make the entity immune to damage.
     */
    void setInvulnerable(bool invulnerable);

    /**
     * @brief Checks if the entity is currently invulnerable.
     * @return True if invulnerable.
     */
    bool isInvulnerable() const;

private:
    int m_maxHealth;         ///< Maximum health limit
    int m_currentHealth;     ///< Current health value
    bool m_invulnerable = false; ///< If true, damage is ignored
};
