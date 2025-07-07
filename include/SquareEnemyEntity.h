#pragma once
#include "EnemyEntity.h"
#include <SFML/Graphics.hpp>

/**
 * SquareEnemyEntity - Splits into 3 small enemies when large enemy dies
 */
class SquareEnemyEntity : public EnemyEntity {
public:
    enum class SizeType {
        Large,   // Original size (can split)
        Small    // Final size (no more splitting)
    };

    SquareEnemyEntity(IdType id, b2World& world, float x, float y,
        TextureManager& textures, SizeType size = SizeType::Large);

    void onDeath(Entity* killer) override;

    SizeType getSizeType() const { return m_sizeType; }
    bool canSplit() const { return m_sizeType == SizeType::Large; }
    float getSizeMultiplier() const;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    void spawnSplitEnemies(const sf::Vector2f& deathPosition);
    float getHealthForSize() const;
    float getSpeedForSize() const;

    SizeType m_sizeType;
    static constexpr float LARGE_SIZE = 0.7f;
    static constexpr float SMALL_SIZE = 0.4f;
};
