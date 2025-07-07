#pragma once
#include "EnemyEntity.h"
#include <SFML/Graphics.hpp>

/**
 * FalconEnemyEntity - Flying enemy that appears and flies across the sky
 * Features: Wing animation, horizontal flight, shooting projectiles
 */
class FalconEnemyEntity : public EnemyEntity {
public:
    FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);
    void update(float dt) override;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    // Animation methods
    void updateAnimation(float dt);
    void switchTexture();

    // Flight behavior
    void updateFlightPattern(float dt);

    // Combat methods
    void updateShooting(float dt);
    void shootProjectile();

    // State variables
    bool m_isSpawned = true;
    bool m_readyToShoot = false;        // Ready to shoot flag
    bool m_hasEnteredScreen = false;    // Has entered camera view
    float m_flightAltitude = 150.0f;    // Flight height

    // Animation variables
    float m_animationTimer = 0.0f;
    float m_animationSpeed = 0.4f;
    int m_currentFrame = 0;

    // Shooting variables
    float m_shootTimer = 0.0f;
    float m_shootCooldown = 1.0f;       // Shoot every 1 second

    // Texture resources
    sf::Texture* m_texture1 = nullptr;
    sf::Texture* m_texture2 = nullptr;
};