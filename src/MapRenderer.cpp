#include "MapRenderer.h"
#include <algorithm>

void MapRenderer::renderAll(sf::RenderTarget& target, const std::vector<std::unique_ptr<GameObject>>& objects) const {
    for (const auto& obj : objects) {
        if (obj) {
            obj->render(target);
        }
    }
}

void MapRenderer::renderWithCulling(sf::RenderTarget& target,
    const std::vector<std::unique_ptr<GameObject>>& objects,
    const sf::FloatRect& viewBounds) const {
    for (const auto& obj : objects) {
        if (obj && isVisible(*obj, viewBounds)) {
            obj->render(target);
        }
    }
}

void MapRenderer::renderByLayers(sf::RenderTarget& target,
    const std::vector<std::unique_ptr<GameObject>>& objects) const {
    // Create a sorted copy for layer-based rendering
    std::vector<GameObject*> sortedObjects;
    sortedObjects.reserve(objects.size());

    for (const auto& obj : objects) {
        if (obj) {
            sortedObjects.push_back(obj.get());
        }
    }

    // Sort by layer (background to foreground)
    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [this](const GameObject* a, const GameObject* b) {
            return getObjectLayer(*a) < getObjectLayer(*b);
        });

    // Render in layer order
    for (const auto* obj : sortedObjects) {
        obj->render(target);
    }
}

bool MapRenderer::isVisible(const GameObject& obj, const sf::FloatRect& viewBounds) const {
    sf::FloatRect objBounds = obj.getBounds();
    return viewBounds.intersects(objBounds);
}

int MapRenderer::getObjectLayer(const GameObject& obj) const {
    // Define rendering layers
    // Background tiles: 0
    // Ground tiles: 1  
    // Objects/Items: 2
    // Enemies: 3
    // Player: 4 (rendered elsewhere)
    // Effects: 5

    // This could be improved with a proper component system
    // For now, use type names or add a getLayer() method to GameObject
    return 1; // Default layer
}