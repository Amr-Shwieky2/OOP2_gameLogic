// ReverseMovementGift.h (محدث)
#pragma once

#include "StaticGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class ReverseMovementGift : public StaticGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;

    // للتأثيرات البصرية
    mutable float m_rotationTimer = 0.0f;

    void updateAnimation(float deltaTime) const;

public:
    ReverseMovementGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return 5; } // نقاط أقل لأنه تأثير سلبي/تحدي
    CollectableType getType() const override { return CollectableType::ReverseControl; }

    PlayerEffect getEffect() const override { return PlayerEffect::ReverseControl; }
    float getEffectDuration() const override { return 5.0f; }
};