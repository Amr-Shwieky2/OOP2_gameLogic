#pragma once

#include <SFML/Graphics.hpp>
#include "StaticGameObject.h"
#include "ResourceManager.h"
#include "GameObjectVisitor.h"
#include <RareCoinGift.h>

class CloseBox : public StaticGameObject {
public:
    CloseBox(b2World& world, float x, float y, TextureManager& textures);


    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    bool isOpened() const;
    void open(); // Triggers the opening state

private:
    sf::Sprite m_sprite;
    bool m_opened = false;
    sf::FloatRect m_bounds;
};
