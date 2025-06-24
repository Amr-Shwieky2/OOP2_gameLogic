#pragma once

#include "StaticGameObject.h"
#include "ResourceManager.h"
#include "Player.h"

class MagneticGift : public StaticGameObject {
public:
    MagneticGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(Player& player);
    bool isCollected() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
};
