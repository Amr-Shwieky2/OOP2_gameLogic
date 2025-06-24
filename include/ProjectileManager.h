#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Projectile.h"

class ProjectileManager {
public:
    ProjectileManager() = default;

    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    void addProjectile(std::unique_ptr<Projectile> projectile);
    void clearDeadProjectiles();

    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const { return m_projectiles; }
    size_t getProjectileCount() const { return m_projectiles.size(); }

private:
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
};