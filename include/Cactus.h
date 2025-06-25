#pragma once

#include <SFML/Graphics.hpp>
#include "StaticGameObject.h"
#include "ResourceManager.h"

class Cactus : public StaticGameObject {
public:
    Cactus(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

private:
    sf::Sprite m_sprite;
    sf::FloatRect m_bounds;
};
