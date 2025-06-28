#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <functional>
#include "ResourceManager.h"



/**
 * PlayerEntity - Replaces the old Player class
 * Uses components instead of inheritance
 */
class PlayerEntity : public Entity {
public:
    PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    // Player-specific methods
    void jump();
    void moveLeft();
    void moveRight();
    void shoot();

    // Score/Lives management
    void addScore(int points);
    int getScore() const { return m_score; }

    // Effects
    void applySpeedBoost(float duration);
    void applyShield(float duration);

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);

    int m_score = 0;
    TextureManager& m_textures;
};