#include "LevelLoader.h"
#include "EntityManager.h"
#include "EntityFactory.h"
#include "Constants.h"
#include <fstream>
#include <iostream>
#include "WellEntity.h"

// For ground tiles that aren't entities yet, we'll need a temporary solution
#include "GameCollisionSetup.h"

bool LevelLoader::loadFromFile(const std::string& path,
    EntityManager& entityManager,
    b2World& world,
    TextureManager& textures) {

    std::vector<std::string> lines = readLevelFile(path);
    if (lines.empty()) {
        std::cerr << "Error: Cannot read level file: " << path << "\n";
        return false;
    }

    // Make sure entities are registered
    //registerGameEntities(world, textures);

    entityManager.clear();

    int mapHeight = static_cast<int>(lines.size());

    for (int y = 0; y < mapHeight; ++y) {
        const std::string& row = lines[y];
        for (int x = 0; x < static_cast<int>(row.length()); ++x) {
            char tileChar = row[x];
            sf::Vector2f pos = calculatePosition(x, y);

            auto entity = createEntityForChar(tileChar, pos.x, pos.y, world, textures);
            if (entity) {
                entityManager.addEntity(std::move(entity));
            }
        }
    }

    return true;
}

std::unique_ptr<Entity> LevelLoader::createEntityForChar(char tileChar, float x, float y,
    b2World& world, TextureManager& textures) {

    EntityFactory& factory = EntityFactory::instance();

    std::cout << "[LEVEL DEBUG] Processing tile '" << tileChar << "' at grid position ("
        << (x / TILE_SIZE) << ", " << ((WINDOW_HEIGHT - y) / TILE_SIZE)
        << ") world position (" << x << ", " << y << ")" << std::endl;

    switch (tileChar) {
        // Collectibles and entities
    case 'C': // Coin
        std::cout << "[LEVEL DEBUG] Creating Coin" << std::endl;
        return factory.create("C", x + TILE_SIZE / 4.f, y + TILE_SIZE / 4.f);

    case 'z': // Square Enemy
        std::cout << "[LEVEL DEBUG] Creating Square Enemy at (" << x << ", " << y << ")" << std::endl;
        return factory.create("z", x, y);

    case 'Z': // Smart Enemy
        std::cout << "[LEVEL DEBUG] Creating Smart Enemy at (" << x << ", " << y << ")" << std::endl;
        return factory.create("Z", x, y);

    case 'F': // Falcon Enemy
        std::cout << "[LEVEL DEBUG] Creating Falcon Enemy at (" << x << ", " << y << ")" << std::endl;
        return factory.create("F", x, y);

    case 's': // Speed Gift
        std::cout << "[LEVEL DEBUG] Creating Speed Gift" << std::endl;
        return factory.create("s", x, y);

    case 'h': // Life Heart Gift
        std::cout << "[LEVEL DEBUG] Creating Life Heart Gift" << std::endl;
        return factory.create("h", x, y);

    case 'W': // Well - ADD THIS CASE
        std::cout << "[LEVEL DEBUG] Creating Well at (" << x << ", " << y << ")" << std::endl;
        return factory.create("W", x, y);

    case 'r': // Reverse Movement Gift
        return factory.create("r", x, y);

    case 'p': // Protective Shield Gift
        return factory.create("p", x, y);

    case 'w': // Headwind Storm Gift
        return factory.create("w", x, y);

    case '*': // Rare Coin Gift
        return factory.create("*", x, y);

    case 'm': // Magnetic Gift
        return factory.create("m", x, y);

        // TODO: Handle ground tiles, obstacles, etc.
        // For now, return nullptr for non-entity tiles
    case 'G': // Ground
        return factory.create("G", x, y);
    case 'L': // Left ground
        return factory.create("L", x, y);
    case 'R': // Right ground
        return factory.create("R", x, y);
    case 'M': // Middle ground
        return factory.create("M", x, y);
    case 'E': // Edge
        return factory.create("E", x, y);
    case 'B': // Box
        std::cout << "[LEVEL DEBUG] Creating Box at (" << x << ", " << y << ")" << std::endl;
        return factory.create("B", x, y);
    case 'S': // Sea
        return factory.create("S", x, y);
    case 'X': // Flag
        std::cout << "[LEVEL DEBUG] Creating Flag" << std::endl;
        return factory.create("X", x, y);
    case 'c': // Cactus
        std::cout << "[LEVEL DEBUG] Creating Cactus" << std::endl;
        return factory.create("c", x, y);

    case ' ':
    case '-':
        // Empty space - no debug output to reduce spam
        return nullptr;

    default:
        std::cout << "[LEVEL WARNING] Unknown tile character: '" << tileChar << "'" << std::endl;
        return nullptr;
    }
}
std::vector<std::string> LevelLoader::readLevelFile(const std::string& path) const {
    std::vector<std::string> lines;
    std::ifstream file(path);

    if (!file.is_open()) {
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

sf::Vector2f LevelLoader::calculatePosition(int x, int y) const {
    float posX = static_cast<float>(x * TILE_SIZE);
    float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);
    return sf::Vector2f(posX, posY);
}

bool LevelLoader::isValidTileChar(char c) const {
    const std::string validChars = "GLERMSBXc Cshprw*mzZ-";
    return validChars.find(c) != std::string::npos;
}

LevelLoader::LevelInfo LevelLoader::getLevelInfo(const std::string& path) const {
    LevelInfo info;
    info.name = path;
    info.description = "Level loaded from " + path;
    info.difficulty = 1;
    info.size = sf::Vector2i(0, 0);
    return info;
}