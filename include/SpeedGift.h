#pragma once
#include "Collectible.h"

class SpeedGift : public Collectible {
public:
    SpeedGift(float x, float y, TextureManager& textures);
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void onCollect(Player& player) override;

private:
    sf::Sprite m_sprite;
};
