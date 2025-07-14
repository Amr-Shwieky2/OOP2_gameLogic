#pragma once
#include <SFML/Graphics.hpp>
class FalconEnemyEntity;

/**
 * FalconWeaponSystem - Single Responsibility: handle Falcon enemy shooting logic
 */
class FalconWeaponSystem {
public:
    explicit FalconWeaponSystem(FalconEnemyEntity& falcon);

    void update(float dt);
    void reset();
    void setReadyToShoot(bool ready);
    bool isReadyToShoot() const { return m_readyToShoot; }

private:
    FalconEnemyEntity& m_falcon;
    float m_shootTimer = 0.0f;
    float m_shootCooldown = 1.0f; // Shoot every 1 second
    bool m_readyToShoot = false;
    bool m_hasEnteredScreen = false;

    void shootProjectile();
};
