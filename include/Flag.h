#pragma once

#include <SFML/Graphics.hpp>
#include "StaticGameObject.h"
#include "ResourceManager.h"

class Flag : public StaticGameObject {
public:
    Flag(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

private:
    sf::Sprite m_sprite;
    sf::FloatRect m_bounds;
};
