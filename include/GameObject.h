// GameObject.h
#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"

class GameObjectVisitor;

class GameObject {
public:
    virtual ~GameObject() = default;

    virtual void render(sf::RenderTarget& target) const = 0;
    virtual sf::FloatRect getBounds() const = 0;

    virtual void update(float) {}
    // Double-dispatch entry point
    virtual void accept(GameObjectVisitor& visitor) = 0;
};
