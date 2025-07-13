#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <memory>

class RenderComponent : public Component {
public:
    RenderComponent();
    void setTexture(const sf::Texture& texture);
    void setSprite(const sf::Sprite& sprite); // Optional, or just expose sf::Sprite public

    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;

private:
    sf::Sprite m_sprite;
};