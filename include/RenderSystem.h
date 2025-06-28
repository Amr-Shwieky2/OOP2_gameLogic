#pragma once
#include "EntityManager.h"
#include <SFML/Graphics.hpp>

class RenderSystem {
public:
    void render(EntityManager& entityManager, sf::RenderWindow& window);
};