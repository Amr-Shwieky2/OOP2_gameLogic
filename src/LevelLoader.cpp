#include "LevelLoader.h"
#include "Map.h"
#include "GameObjectFactory.h"  
#include <fstream>
#include <iostream>

bool LevelLoader::loadFromFile(const std::string& path, Map& map, b2World& world, TextureManager& textures) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << " Error: Cannot open level file: " << path << std::endl;
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    int mapHeight = static_cast<int>(lines.size());
    int objectsCreated = 0;
    int objectsFailed = 0;

    for (int y = 0; y < mapHeight; ++y) {
        const std::string& row = lines[y];
        for (int x = 0; x < static_cast<int>(row.length()); ++x) {
            float posX = static_cast<float>(x * TILE_SIZE);
            float posY = WINDOW_HEIGHT - TILE_SIZE - static_cast<float>(y * TILE_SIZE);
            char tileChar = row[x];

            auto gameObject = GameObjectFactory::createFromChar(tileChar, posX, posY, world, textures);

            if (gameObject) {
                if (GameObjectFactory::isDynamicObject(tileChar)) {
                    // Convert to DynamicGameObject and add
                    if (auto dynamicObj = dynamic_cast<DynamicGameObject*>(gameObject.get())) {
                        auto ptr = std::unique_ptr<DynamicGameObject>(
                            static_cast<DynamicGameObject*>(gameObject.release())
                        );
                        map.addDynamic(std::move(ptr));
                        objectsCreated++;
                    }
                    else {
                        objectsFailed++;
                    }
                }
                else {
                    // Add as static object
                    map.addStatic(std::move(gameObject));
                    objectsCreated++;
                }
            }
            else if (tileChar != ' ' && tileChar != '-') {
                // Only count non-empty characters as potential objects
                objectsFailed++;
            }
        }
    }
    return true;
}