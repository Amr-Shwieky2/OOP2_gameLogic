#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <memory>

/**
 * @class RenderComponent
 * @brief Component responsible for rendering a visual sprite.
 *
 * This component wraps an `sf::Sprite` and allows setting textures or custom sprites.
 * It is used by the rendering system to draw the entity on screen.
 */
class RenderComponent : public Component {
public:
    RenderComponent() = default;

    /**
     * @brief Sets the texture used by the internal sprite.
     * @param texture Reference to a loaded SFML texture.
     */
    void setTexture(const sf::Texture& texture);

    /**
     * @brief Replaces the internal sprite with a custom one.
     * @param sprite A fully configured SFML sprite.
     */
    void setSprite(const sf::Sprite& sprite);

    /**
     * @brief Access the internal sprite for manipulation or rendering.
     * @return Reference to the sprite.
     */
    sf::Sprite& getSprite();

    /**
     * @brief Const access to the internal sprite (read-only).
     * @return Const reference to the sprite.
     */
    const sf::Sprite& getSprite() const;

private:
    sf::Sprite m_sprite; ///< Sprite used for rendering the entity.
};
