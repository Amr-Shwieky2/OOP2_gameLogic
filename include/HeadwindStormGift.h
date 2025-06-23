// HeadwindStormGift.h (محدث)
#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class HeadwindStormGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

    // للتأثيرات البصرية
    mutable float m_stormTimer = 0.0f;

    void updateStormAnimation(float deltaTime) const;

public:
    HeadwindStormGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 8; } // نقاط قليلة لأنه تأثير تحدي
    CollectableType getType() const override { return CollectableType::HeadwindStorm; }

    PlayerEffect getEffect() const override { return PlayerEffect::Headwind; }
    float getEffectDuration() const override { return 4.0f; }
};