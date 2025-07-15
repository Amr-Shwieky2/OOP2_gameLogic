#pragma once

#include "Entity.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <functional>
#include "ResourceManager.h"

/**
 * @brief GiftEntity - Represents a collectible gift entity in the game.
 *
 * This class unifies all gift types (e.g., LifeHeart, SpeedBoost, etc.) into a single
 * configurable class using the GiftType enum. It supports texture management and
 * collection state tracking.
 */
class GiftEntity : public Entity {
public:
    /**
     * @brief Enum of all possible gift types.
     */
    enum class GiftType {
        LifeHeart,
        SpeedBoost,
        Shield,
        RareCoin,
        ReverseMovement,
        HeadwindStorm,
        Magnetic
    };

    /**
     * @brief Constructs a new GiftEntity with a specific type and position.
     *
     * @param id        Unique identifier of the gift entity.
     * @param type      The type of gift (e.g., LifeHeart, SpeedBoost).
     * @param x         X position in the game world.
     * @param y         Y position in the game world.
     * @param textures  Reference to the texture manager for loading sprites.
     */
    GiftEntity(IdType id, GiftType type, float x, float y, TextureManager& textures);

    /**
     * @brief Gets the type of the gift.
     * @return GiftType enum representing the current gift type.
     */
    GiftType getGiftType() const { return m_giftType; }

    /**
     * @brief Marks the gift as collected and disables it.
     */
    void collect();

    /**
     * @brief Checks whether the gift has been collected.
     * @return true if collected, false otherwise.
     */
    bool isCollected() const { return m_collected; }

private:
    /**
     * @brief Sets up core components like Transform, Render, and Collision.
     *
     * @param x         Initial X position.
     * @param y         Initial Y position.
     * @param textures  Reference to the texture manager.
     */
    void setupComponents(float x, float y, TextureManager& textures);

    /**
     * @brief Maps a gift type to the appropriate texture name.
     *
     * @param type  The gift type to get texture for.
     * @return Filename string of the associated texture.
     */
    std::string getTextureNameForType(GiftType type) const;

private:
    GiftType m_giftType;     ///< Type of the gift (e.g., Speed, LifeHeart, etc.)
    bool m_collected = false; ///< Indicates if the gift has been collected.
};
