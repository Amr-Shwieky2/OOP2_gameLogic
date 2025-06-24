#pragma once
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"

class BackgroundRenderer {
public:
    BackgroundRenderer(TextureManager& textures);

    void render(sf::RenderWindow& window, const sf::View& camera) const;

private:
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    void setupBackground();
};
