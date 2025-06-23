#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class RareCoinGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

public:
    RareCoinGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 50; } // عملة نادرة = 50 نقطة
    CollectableType getType() const override { return CollectableType::RareCoin; }

    PlayerEffect getEffect() const override { return PlayerEffect::Magnetic; }
    float getEffectDuration() const override { return 5.0f; }
};