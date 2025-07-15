#include "FalconWeaponSystem.h"
#include "FalconEnemyEntity.h"
#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "GameSession.h"
#include "Constants.h"
#include <iostream>
#include "AudioManager.h"

extern GameSession* g_currentSession;
//-------------------------------------------------------------------------------------
FalconWeaponSystem::FalconWeaponSystem(FalconEnemyEntity& falcon)
    : m_falcon(falcon) {
}
//-------------------------------------------------------------------------------------
void FalconWeaponSystem::reset() {
    m_shootTimer = 0.0f;
    m_readyToShoot = false;
    m_hasEnteredScreen = false;
}
//-------------------------------------------------------------------------------------
void FalconWeaponSystem::setReadyToShoot(bool ready) {
    if (ready && !m_hasEnteredScreen) {
        m_hasEnteredScreen = true;
        AudioManager::instance().playSoundLoop("falcon");
    }

    if (!ready && m_hasEnteredScreen) {
        m_hasEnteredScreen = false;
        AudioManager::instance().stopSound("falcon");
    }

    m_readyToShoot = ready;
}
//-------------------------------------------------------------------------------------
void FalconWeaponSystem::update(float dt) {
    if (!m_readyToShoot)
        return;

    m_shootTimer += dt;
    if (m_shootTimer >= m_shootCooldown) {
        shootProjectile();
        m_shootTimer = 0.0f;
    }
}
//-------------------------------------------------------------------------------------
void FalconWeaponSystem::shootProjectile() {
    if (!g_currentSession)
        return;

    auto* transform = m_falcon.getComponent<Transform>();
    auto* physics = m_falcon.getComponent<PhysicsComponent>();
    if (!transform || !physics || !physics->getBody())
        return;

    b2World& world = *physics->getBody()->GetWorld();
    sf::Vector2f falconPos = transform->getPosition();

    // Spawn bullet below the falcon
    sf::Vector2f bulletSpawnPos = falconPos + sf::Vector2f(0.f, 60.f);
    sf::Vector2f shootDir(0.f, 1.f);

    try {
        auto projectile = std::make_unique<ProjectileEntity>(
            g_currentSession->getEntityManager().generateId(), world,
            bulletSpawnPos.x, bulletSpawnPos.y,
            shootDir, m_falcon.getTextures(), false);

        projectile->setActive(true);
        g_currentSession->spawnEntity(std::move(projectile));
    }
    catch (const std::exception& e) {
        std::cerr << "[FALCON] Error shooting: " << e.what() << std::endl;
    }
}
//-------------------------------------------------------------------------------------