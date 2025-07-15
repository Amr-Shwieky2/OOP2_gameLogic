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
 * @brief EnemyEntity - Abstract base class for all enemy types in the game.
 *
 * This class encapsulates shared logic and setup for enemies, including
 * health, transform, collision, and physics. It also provides a virtual
 * method for custom behavior upon death (used in derived enemy types).
 */
class EnemyEntity : public Entity {
public:
    /**
     * @brief Enum listing the supported enemy types.
     */
    enum class EnemyType {
        Square,     ///< Splitting enemy (e.g., splits into smaller units).
        Falcon,     ///< Flying, shooting enemy.
        Smart       ///< AI-controlled enemy with advanced behavior.
    };

    /**
     * @brief Constructor to initialize a generic enemy entity.
     *
     * @param id        Unique identifier for the entity.
     * @param type      Specific type of enemy (Square, Falcon, etc.).
     * @param world     Reference to the Box2D physics world.
     * @param x         Initial X position.
     * @param y         Initial Y position.
     * @param textures  Reference to the texture manager.
     */
    EnemyEntity(IdType id, EnemyType type, b2World& world, float x, float y, TextureManager& textures);

    /**
     * @brief Get the specific type of this enemy.
     * @return Enum value representing the enemy type.
     */
    EnemyType getEnemyType() const { return m_enemyType; }

    /**
     * @brief Main update loop called each frame.
     * Used for syncing position and performing per-frame enemy logic.
     *
     * @param dt Delta time since last frame.
     */
    void update(float dt) override;

    /**
     * @brief Get reference to the shared texture manager.
     * Can be used by derived enemies to load specific textures.
     */
    TextureManager& getTextures() { return m_textures; }

    /**
     * @brief Virtual method called when the enemy is killed.
     * Can be overridden to add special effects (e.g., splitting into smaller enemies).
     *
     * @param killer Optional pointer to the entity that killed this enemy.
     */
    virtual void onDeath([[maybe_unused]] Entity* killer) override {
        // Default implementation does nothing.
        // Derived enemy types can override.
    }

protected:
    /**
     * @brief Setup common components (Transform, Health, Collision).
     *
     * Derived enemies can call this to reuse basic setup.
     *
     * @param world     Reference to Box2D world.
     * @param x         X position.
     * @param y         Y position.
     * @param textures  Reference to texture manager (optional override).
     */
    virtual void setupComponents(b2World& world, float x, float y, TextureManager& textures);

    EnemyType m_enemyType;         ///< Specific enemy type.
    TextureManager& m_textures;    ///< Reference to texture manager (for loading enemy textures).
};
