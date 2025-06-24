#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "PlayerEffect.h"
#include "Constants.h" 

class PlayerRenderer {
public:
    PlayerRenderer(TextureManager& textures);

    void updateVisuals(sf::Vector2f position, float rotation, bool hasTransparent, bool hasMagnetic);
    void render(sf::RenderTarget& target) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getSpriteCenter() const;

private:
    sf::Sprite m_sprite;
    TextureManager& m_textures;

    void updateTexture(bool hasTransparent, bool hasMagnetic);
    void setupSprite();
};
