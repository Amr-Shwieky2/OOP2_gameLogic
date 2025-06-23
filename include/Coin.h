#pragma once

#include "DynamicGameObject.h"
#include "ICollectable.h"
#include "ResourceManager.h"
#include "GameState.h"

class Coin : public DynamicGameObject, public ICollectable {
private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    int m_value;
    TextureManager& m_textures;
    CollectableType m_type;

public:
    // Regular coin
    Coin(float x, float y, TextureManager& textures, int value = 10);

    // Rare coin constructor
    static std::unique_ptr<Coin> createRareCoin(float x, float y, TextureManager& textures);

    // From DynamicGameObject
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    // From ICollectable
    void onCollect(GameState& gameState) override;
    bool isCollected() const override { return m_collected; }
    int getValue() const override { return m_value; }
    CollectableType getType() const override { return m_type; }

    // Coin-specific effects for rare coins
    PlayerEffect getEffect() const override;
    float getEffectDuration() const override;

private:
    void setupSprite();
};