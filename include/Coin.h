#pragma once

#include "DynamicGameObject.h"
#include "ResourceManager.h"
#include "Player.h"

class Coin : public DynamicGameObject {
public:
    Coin(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    void update(float deltaTime) override;

    void collect();
    bool isCollected() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
    TextureManager& m_textures;
};
