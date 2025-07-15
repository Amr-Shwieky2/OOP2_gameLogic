#pragma once

#include "CollectibleEntity.h"
#include "Transform.h"
#include <SFML/System/Vector2.hpp>

/**
 * @brief CoinEntity - Represents a collectible coin in the game.
 *
 * Inherits from CollectibleEntity and adds behavior for circular motion.
 * The coin can be collected by entities such as the player.
 */
class CoinEntity : public CollectibleEntity {
public:
    /**
     * @brief Constructs a new CoinEntity with a unique identifier.
     * @param id The unique ID of the coin entity.
     */
    explicit CoinEntity(IdType id);

    /**
     * @brief Called when the coin is collected by another entity.
     * @param collector Pointer to the entity that collected the coin.
     */
    void onCollect(Entity* collector) override;

    /**
     * @brief Configures circular motion for the coin around a given center.
     * Should be called after setting the coin's initial position.
     *
     * @param centerPosition The center point of the circular motion.
     */
    void setupCircularMotion(const sf::Vector2f& centerPosition);

private:
    float m_circleRadius = 50.0f;    ///< Radius of the circular path.
    float m_rotationSpeed = 2.0f;    ///< Angular speed of rotation.
};
