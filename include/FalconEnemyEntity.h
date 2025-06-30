// FalconEnemyEntity.h
#pragma once
#include "EnemyEntity.h"
#include <SFML/Graphics.hpp>

/**
 * FalconEnemyEntity - Flying enemy that appears after 30 seconds
 * Switches between two images for animation and shoots projectiles
 */
class FalconEnemyEntity : public EnemyEntity {
public:
    FalconEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void update(float dt) override;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    void updateAnimation(float dt);
    void shootProjectile();
    void switchTexture();

    // Animation
    float m_animationTimer = 0.0f;
    float m_animationSpeed = 0.5f; // Switch every 0.5 seconds
    int m_currentFrame = 0;

    // Shooting
    float m_shootTimer = 0.0f;
    float m_shootCooldown = 2.0f; // Shoot every 2 seconds

    // Spawn delay
    float m_spawnTimer = 0.0f;
    static constexpr float SPAWN_DELAY = 30.0f; // 30 seconds
    bool m_spawned = false;

    // Store both textures
    sf::Texture* m_texture1 = nullptr;
    sf::Texture* m_texture2 = nullptr;
};