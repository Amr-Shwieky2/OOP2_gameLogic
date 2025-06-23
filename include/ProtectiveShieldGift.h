// ProtectiveShieldGift.h (محدث)
#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class ProtectiveShieldGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

    // للتأثيرات البصرية
    mutable float m_animationTimer = 0.0f;

    void setupShieldEffects();
    void updateAnimation(float deltaTime) const;

public:
    ProtectiveShieldGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 20; } // نقاط إضافية للدرع
    CollectableType getType() const override { return CollectableType::Shield; }

    PlayerEffect getEffect() const override { return PlayerEffect::Shield; }
    float getEffectDuration() const override { return 8.0f; }
};