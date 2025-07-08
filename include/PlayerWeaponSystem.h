#pragma once
#include "ResourceManager.h"
#include <Box2D/Box2D.h>

class PlayerEntity;

enum class WeaponType {
    Basic,
    Rapid,
    Spread,
    Laser
};

/**
 * PlayerWeaponSystem - Single Responsibility: Handle player weapons and shooting
 */
class PlayerWeaponSystem {
public:
    PlayerWeaponSystem(PlayerEntity& player, b2World& world, TextureManager& textures);

    void update(float dt);
    void shoot();
    void setWeaponType(WeaponType type);
    bool canShoot() const;

    WeaponType getWeaponType() const { return m_weaponType; }

private:
    PlayerEntity& m_player;
    b2World& m_world;
    TextureManager& m_textures;

    float m_lastShotTime = 0.0f;
    float m_shotCooldown = 0.3f;
    WeaponType m_weaponType = WeaponType::Basic;

    void createProjectile(const sf::Vector2f& position, const sf::Vector2f& direction);
    float getCooldownForWeapon(WeaponType type) const;
};