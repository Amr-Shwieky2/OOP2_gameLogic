#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"  
#include "ResourceManager.h"
#include "GameState.h"     

class LifeHeartGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

    void setupHeartAnimation();

public:
    LifeHeartGift(float x, float y, TextureManager& textures);
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 1; } // قيمة حياة واحدة
    CollectableType getType() const override { return CollectableType::LifeHeart; }

    PlayerEffect getEffect() const override { return PlayerEffect::None; }
    float getEffectDuration() const override { return 0.0f; }
};