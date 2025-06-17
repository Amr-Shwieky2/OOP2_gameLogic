#pragma once

#include "GameObject.h"
#include "ResourceManager.h"
#include "Player.h"
#include "IUpdatable.h"

class Coin : public GameObject, public IUpdatable {
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
};
