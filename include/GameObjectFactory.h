#pragma once

#include <memory>
#include <Box2D/Box2D.h>
#include "GameObject.h"
#include "DynamicGameObject.h"
#include "ResourceManager.h"

class GameObjectFactory {
public:
    // Factory method for creating objects from level file characters
    static std::unique_ptr<GameObject> createFromChar(
        char character,
        float x,
        float y,
        b2World& world,
        TextureManager& textures
    );

    // Specific factory methods for different categories
    static std::unique_ptr<GameObject> createTile(
        char tileType,
        float x,
        float y,
        b2World& world,
        TextureManager& textures
    );

    static std::unique_ptr<GameObject> createCollectable(
        char collectableType,
        float x,
        float y,
        TextureManager& textures
    );

    static std::unique_ptr<GameObject> createDynamicObject(
        char objectType,
        float x,
        float y,
        b2World& world,
        TextureManager& textures
    );

    // Helper methods
    static bool isDynamicObject(char character);
    static bool isStaticObject(char character);

private:
    // Private constructor - this is a static factory
    GameObjectFactory() = delete;

    // Helper methods for position adjustment
    static float adjustXPosition(char character, float baseX);
    static float adjustYPosition(char character, float baseY);
};