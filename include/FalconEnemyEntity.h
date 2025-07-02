// FalconEnemyEntity.h
#pragma once
#include "EnemyEntity.h"
#include <SFML/Graphics.hpp>

/**
 * FalconEnemyEntity - Flying enemy that appears and flies across the sky
 * Features: Wing animation, sine wave flight pattern, shoots projectiles
 */
class FalconEnemyEntity : public EnemyEntity {
public:
    FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void update(float dt) override;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    // Animation functions
    void updateAnimation(float dt);
    void switchTexture();
    
    // Flight behavior
    void updateFlightPattern(float dt);
    
    // Combat system
    void updateShooting(float dt);
    void shootProjectile();

    // Animation state
    float m_animationTimer = 0.0f;
    float m_animationSpeed = 0.4f; // Switch every 0.4 seconds
    int m_currentFrame = 0;

    // Shooting state
    float m_shootTimer = 0.0f;
    float m_shootCooldown = 2.0f; // Shoot every 2 seconds

    // Wing animation textures
    sf::Texture* m_texture1 = nullptr;
    sf::Texture* m_texture2 = nullptr;
};