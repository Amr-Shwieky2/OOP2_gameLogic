#pragma once

#include "StaticGameObject.h"
#include "ResourceManager.h"
#include "PlayerEffectManager.h"
#include "Player.h"

class RareCoinGift : public StaticGameObject {
public:
    RareCoinGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(Player& player);
    bool isCollected() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
};
