#include "PlayerWeapon.h"
#include "Constants.h"
#include <algorithm>

PlayerWeapon::PlayerWeapon(b2World& world) : m_world(world) {}

void PlayerWeapon::shoot(sf::Vector2f playerPos, bool facingRight, TextureManager& textures) {
    if (m_shootCooldown > 0.f) return;

    float dir = facingRight ? 1.f : -1.f;

    auto proj = std::make_unique<Projectile>(
        m_world,
        playerPos.x / PPM,
        playerPos.y / PPM,
        dir,
        textures,
        false
    );

    m_projectiles.push_back(std::move(proj));
    m_shootCooldown = m_fireInterval;
}

void PlayerWeapon::updateProjectiles(float deltaTime) {
    if (m_shootCooldown > 0.f)
        m_shootCooldown -= deltaTime;

    // Update all projectiles
    for (auto& p : m_projectiles)
        p->update(deltaTime);

    // Remove dead projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) {
                return !p->isAlive();
            }),
        m_projectiles.end()
    );
}

void PlayerWeapon::renderProjectiles(sf::RenderTarget& target) const {
    for (const auto& p : m_projectiles)
        p->render(target);
}

const std::vector<std::unique_ptr<Projectile>>& PlayerWeapon::getProjectiles() const {
    return m_projectiles;
}