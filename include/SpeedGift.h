// SpeedGift.h (محدث)
#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class SpeedGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

public:
    SpeedGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 15; } // نقاط إضافية للهدية
    CollectableType getType() const override { return CollectableType::SpeedBoost; }

    PlayerEffect getEffect() const override { return PlayerEffect::SpeedBoost; }
    float getEffectDuration() const override { return 6.0f; }

};