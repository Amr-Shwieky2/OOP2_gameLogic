#include "ProjectileManager.h"
#include <algorithm>

void ProjectileManager::update(float deltaTime) {
    // Update all projectiles
    for (auto& proj : m_projectiles) {
        proj->update(deltaTime);
    }

    clearDeadProjectiles();
}

void ProjectileManager::render(sf::RenderTarget& target) const {
    for (const auto& proj : m_projectiles) {
        proj->render(target);
    }
}

void ProjectileManager::addProjectile(std::unique_ptr<Projectile> projectile) {
    if (projectile) {
        m_projectiles.push_back(std::move(projectile));
    }
}

void ProjectileManager::clearDeadProjectiles() {
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& proj) {
                return !proj->isAlive();
            }),
        m_projectiles.end()
    );
}