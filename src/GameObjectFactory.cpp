#include "GameObjectFactory.h"
#include "Constants.h"
#include "GroundTile.h"
#include "Flag.h"
#include "LifeHeartGift.h"
#include "SpeedGift.h"
#include "ReverseMovementGift.h"
#include "ProtectiveShieldGift.h"
#include "HeadwindStormGift.h"
#include "RareCoinGift.h"
#include "Coin.h"
#include "MovableBox.h"
#include <iostream>

std::unique_ptr<GameObject> GameObjectFactory::createFromChar(
    char character,
    float x,
    float y,
    b2World& world,
    TextureManager& textures) {

    // Adjust positions based on object type
    float adjustedX = adjustXPosition(character, x);
    float adjustedY = adjustYPosition(character, y);

    try {
        // Dynamic objects 
        if (auto dynamic = createDynamicObject(character, adjustedX, adjustedY, world, textures)) {
            return dynamic;
        }
        // Tiles 
        if (auto tile = createTile(character, adjustedX, adjustedY, world, textures)) {
            return tile;
        }
        // Collectables 
        if (auto collectable = createCollectable(character, adjustedX, adjustedY, textures)) {
            return collectable;
        }
        // Empty space or unknown character
        if (character != ' ' && character != '-') {
            std::cout << " Unknown character: '" << character << "' at (" << x << ", " << y << ")" << std::endl;
        }
        return nullptr;
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating object for character '" << character
            << "' at (" << x << ", " << y << "): " << e.what() << std::endl;
        return nullptr;
    }
}

std::unique_ptr<GameObject> GameObjectFactory::createTile(
    char tileType,
    float x,
    float y,
    b2World& world,
    TextureManager& textures) {

    switch (tileType) {
    case 'G':
        return std::make_unique<GroundTile>(world, x, y, TileType::Ground, textures);
    case 'L':
        return std::make_unique<GroundTile>(world, x, y, TileType::Left, textures);
    case 'R':
        return std::make_unique<GroundTile>(world, x, y, TileType::Right, textures);
    case 'M':
        return std::make_unique<GroundTile>(world, x, y, TileType::Middle, textures);
    case 'E':
        return std::make_unique<GroundTile>(world, x, y - TILE_SIZE, TileType::Edge, textures);
    case 'S':  
        return std::make_unique<GroundTile>(world, x, y, TileType::Sea, textures);
    default:
        return nullptr;
    }
}

std::unique_ptr<GameObject> GameObjectFactory::createCollectable(
    char collectableType,
    float x,
    float y,
    TextureManager& textures) {

    switch (collectableType) {
    case 'X':
        return std::make_unique<Flag>(x + TILE_SIZE / 2.f, y , textures);
        // Gifts
    case 'H':  // Heart
        return std::make_unique<LifeHeartGift>(x, y, textures);
    case 'P':  // Protective shield
        return std::make_unique<ProtectiveShieldGift>(x, y, textures);
    case 'W':  // Wind storm (HeadwindStorm - changed to W)
        return std::make_unique<HeadwindStormGift>(x, y, textures);
    case 'T':  // Turbo speed
        return std::make_unique<SpeedGift>(x, y, textures);
    case 'V':  // reVerse movement
        return std::make_unique<ReverseMovementGift>(x, y, textures);
    case '*':  // Rare coin
        return std::make_unique<RareCoinGift>(x, y, textures);

    default:
        return nullptr;
    }
}

std::unique_ptr<GameObject> GameObjectFactory::createDynamicObject(
    char objectType,
    float x,
    float y,
    b2World& world,
    TextureManager& textures) {

    switch (objectType) {
    case 'C':
        return std::make_unique<Coin>(x + TILE_SIZE / 4.f, y + TILE_SIZE / 4.f, textures);
    case 'B':
        return std::make_unique<MovableBox>(world, x, y, TileType::Box, textures);
    default:
        return nullptr;
    }
}

bool GameObjectFactory::isDynamicObject(char character) {
    switch (character) {
    case 'C':  // Coin
    case 'B':  // MovableBox
        return true;
    default:
        return false;
    }
}

bool GameObjectFactory::isStaticObject(char character) {
    switch (character) {
    case 'G': case 'L': case 'R': case 'M': case 'E': case 'S':
    case 'X': case 'H': case 'P': case 'W': case 'T': case 'V': case '*':
        return true;
    default:
        return false;
    }
}

float GameObjectFactory::adjustXPosition(char character, float baseX) {
    switch (character) {
    case 'C':  // Coin - offset for better visual placement
        return baseX + TILE_SIZE / 4.f;
    case 'X':  // Flag - center of tile
        return baseX + TILE_SIZE / 2.f;
    default:
        return baseX;
    }
}

float GameObjectFactory::adjustYPosition(char character, float baseY) {
    switch (character) {
    case 'E': 
        return baseY ;
    case 'X':  
        return baseY ;
    case 'C':  // Coin - slight offset
        return baseY + TILE_SIZE / 4.f;
        // Gifts remain at ground level
    case 'H': case 'P': case 'W': case 'T': case 'V': case '*':
        return baseY;
    default:
        return baseY;
    }
}