#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
class Player; // Forward declaration

class Collectible {
public:
    virtual ~Collectible() = default;

    virtual void update(float) {}
    virtual void render(sf::RenderTarget& target) const = 0;
    virtual sf::FloatRect getBounds() const = 0;

    virtual void onCollect(Player& player) = 0;
    bool isCollected() const { return m_collected; }
    void collect() { m_collected = true; }

protected:
    bool m_collected = false;
};
