#pragma once
#include "GameObject.h"
#include "ResourceManager.h"
#include <SFML/Graphics.hpp>
#include "IUpdatable.h"

class RareCoinGift : public GameObject, public IUpdatable {
public:
    RareCoinGift(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;
    void update(float deltaTime) override;

    bool isCollected() const;
    void collect();

private:
    sf::Sprite m_sprite;
    bool m_collected = false;
};
