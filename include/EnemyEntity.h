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
 * Now includes onDeath virtual method for special death behaviors
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
    void update(float dt) override;

    // Make textures accessible to derived classes
    TextureManager& getTextures() { return m_textures; }

    // NEW: Virtual onDeath method for special death behaviors
    virtual void onDeath([[maybe_unused]] Entity* killer) override {
        // Default enemy death behavior
        // Derived classes can override for special effects (like splitting)
    }

protected:
    virtual void setupComponents(b2World& world, float x, float y, TextureManager& textures);

    EnemyType m_enemyType;
    TextureManager& m_textures;
};
