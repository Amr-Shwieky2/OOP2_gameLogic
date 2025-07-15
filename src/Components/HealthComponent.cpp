#include "HealthComponent.h"
#include <algorithm>
//-------------------------------------------------------------------------------------
HealthComponent::HealthComponent(int maxHealth)
    : m_maxHealth(maxHealth)
    , m_currentHealth(maxHealth) {
}
//-------------------------------------------------------------------------------------
void HealthComponent::takeDamage(int amount) {
    if (m_invulnerable || amount <= 0) return;
    m_currentHealth = std::max(0, m_currentHealth - amount);
}
//-------------------------------------------------------------------------------------
void HealthComponent::heal(int amount) {
    if (amount <= 0) return;
    m_currentHealth = std::min(m_maxHealth, m_currentHealth + amount);
}
//-------------------------------------------------------------------------------------
void HealthComponent::setHealth(int health) {
    m_currentHealth = std::clamp(health, 0, m_maxHealth);
}
//-------------------------------------------------------------------------------------
int HealthComponent::getHealth() const {
    return m_currentHealth;
}
//-------------------------------------------------------------------------------------
int HealthComponent::getMaxHealth() const {
    return m_maxHealth;
}
//-------------------------------------------------------------------------------------
bool HealthComponent::isAlive() const {
    return m_currentHealth > 0;
}
//-------------------------------------------------------------------------------------
void HealthComponent::setInvulnerable(bool invulnerable) {
    m_invulnerable = invulnerable;
}
//-------------------------------------------------------------------------------------
bool HealthComponent::isInvulnerable() const {
    return m_invulnerable;
}
//-------------------------------------------------------------------------------------