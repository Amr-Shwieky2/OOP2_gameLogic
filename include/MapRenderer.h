#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameObject.h"

class MapRenderer {
public:
    MapRenderer() = default;

    // Render all objects
    void renderAll(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& objects) const;

    // Render with culling (only visible objects)
    void renderWithCulling(sf::RenderTarget& target,
        const std::vector<std::unique_ptr<GameObject>>& objects,
        const sf::FloatRect& viewBounds) const;

    // Layer-based rendering
    void renderByLayers(sf::RenderTarget& target,
        const std::vector<std::unique_ptr<GameObject>>& objects) const;

private:
    bool isVisible(const GameObject& obj, const sf::FloatRect& viewBounds) const;
    int getObjectLayer(const GameObject& obj) const;
};