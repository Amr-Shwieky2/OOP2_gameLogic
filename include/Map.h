#pragma once
#include <string>
#include <memory>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "GameObjectManager.h"
#include "MapRenderer.h"
#include "LevelLoader.h"
#include "ResourceManager.h"

class Map {
public:
    Map(b2World& world, TextureManager& textures);
    ~Map() = default;

    // Level management
    bool loadFromFile(const std::string& path);
    void clear();

    // Object management (delegated)
    void addStatic(std::unique_ptr<StaticGameObject> obj);
    void addDynamic(std::unique_ptr<DynamicGameObject> obj);
    void addGeneric(std::unique_ptr<GameObject> obj);

    // Game loop
    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;
    void renderWithCulling(sf::RenderTarget& target, const sf::FloatRect& viewBounds) const;

    // Getters
    std::vector<std::unique_ptr<GameObject>>& getObjects() { return m_objectManager->getAllObjects(); }
    const std::vector<std::unique_ptr<GameObject>>& getObjects() const { return m_objectManager->getAllObjects(); }

    // Map info
    LevelLoader::LevelInfo getCurrentLevelInfo() const { return m_currentLevelInfo; }
    bool isLoaded() const { return m_loaded; }

private:
    // Components
    std::unique_ptr<GameObjectManager> m_objectManager;
    std::unique_ptr<MapRenderer> m_renderer;
    std::unique_ptr<LevelLoader> m_levelLoader;

    // Dependencies
    b2World& m_world;
    TextureManager& m_textures;

    // State
    bool m_loaded = false;
    std::string m_currentLevelPath;
    LevelLoader::LevelInfo m_currentLevelInfo;
};