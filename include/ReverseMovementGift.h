#pragma once

#include "GameObject.h"
#include "ResourceManager.h"

class ReverseMovementGift : public GameObject {
public:
    ReverseMovementGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    void collect();
    bool isCollected() const;

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
};
