// PlayerEntity.h - Enhanced with proper input handling
#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "ResourceManager.h"
#include "InputService.h"

/**
 * PlayerEntity - Enhanced version with proper component-based input handling
 */
class PlayerEntity : public Entity {
public:
    PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    // Override update to handle input
    void update(float dt) override;

    // Input handling
    void handleInput(const InputService& input);

    // Movement methods
    void jump();
    void moveLeft();
    void moveRight();
    void shoot();

    // Score/Lives management
    void addScore(int points);
    int getScore() const { return m_score; }

    // Effects (temporary - will be replaced with proper effect system)
    void applySpeedBoost(float duration);
    void applyShield(float duration);

    // Getters for compatibility
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    bool isOnGround() const;

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
    void updateVisuals();
    void updatePhysics();

    int m_score = 0;
    TextureManager& m_textures;

    // Effect timers (temporary)
    float m_speedBoostTimer = 0.0f;
    float m_shieldTimer = 0.0f;

    // Ground detection
    int m_groundContacts = 0;
};
