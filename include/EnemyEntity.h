#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <functional>
#include "ResourceManager.h"


/**
 * EnemyEntity - Base class for all enemies
 * Replaces SquareEnemy, FalconEnemy, etc.
 */
class EnemyEntity : public Entity {
public:
    enum class EnemyType {
        Square,
        Falcon,
        Smart
    };

    EnemyEntity(IdType id, EnemyType type, b2World& world, float x, float y, TextureManager& textures);

    EnemyType getEnemyType() const { return m_enemyType; }

protected:
    virtual void setupComponents(b2World& world, float x, float y, TextureManager& textures);

    EnemyType m_enemyType;
    TextureManager& m_textures;
};

/**
 * SquareEnemyEntity - Walking enemy
 */
class SquareEnemyEntity : public EnemyEntity {
public:
    SquareEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;
};