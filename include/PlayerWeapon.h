#pragma once
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <memory>
#include "Projectile.h"
#include "ResourceManager.h"

class PlayerWeapon {
public:
    PlayerWeapon(b2World& world);

    void shoot(sf::Vector2f playerPos, bool facingRight, TextureManager& textures);
    void updateProjectiles(float deltaTime);
    void renderProjectiles(sf::RenderTarget& target) const;

    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const;

private:
    b2World& m_world;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    float m_shootCooldown = 0.f;
    static constexpr float m_fireInterval = 0.5f;
};
