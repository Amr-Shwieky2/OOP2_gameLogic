// LifeHeartGift.h
#pragma once

#include "StaticGameObject.h"
#include "ResourceManager.h"
#include "Player.h"
#include "GameObjectVisitor.h"

class LifeHeartGift : public StaticGameObject {
public:
    LifeHeartGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    void onCollect(Player& player);
    bool isCollected() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
};