#pragma once
#include "EnemyEntity.h"
#include <SFML/Graphics.hpp>

/**
 * SquareEnemyEntity - Enhanced with splitting mechanic
 * When killed by player jump, spawns 3 smaller versions
 */
class SquareEnemyEntity : public EnemyEntity {
public:
    enum class SizeType {
        Large,   // Original size (can split)
        Medium,  // First split (can split again)
        Small    // Final size (no more splitting)
    };

    SquareEnemyEntity(IdType id, b2World& world, float x, float y,
        TextureManager& textures, SizeType size = SizeType::Large);

    // Override to handle splitting
    void onDeath(Entity* killer) override;

    // Get current size
    SizeType getSizeType() const { return m_sizeType; }

    // Check if this enemy can split
    bool canSplit() const { return m_sizeType != SizeType::Small; }

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    void spawnSplitEnemies(const sf::Vector2f& deathPosition);
    float getSizeMultiplier() const;
    float getHealthForSize() const;
    float getSpeedForSize() const;

    SizeType m_sizeType;
    static constexpr float LARGE_SIZE = 0.8f;
    static constexpr float MEDIUM_SIZE = 0.5f;
    static constexpr float SMALL_SIZE = 0.3f;
};