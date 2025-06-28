#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include "Entity.h"
#include "EntityManager.h"

/**
 * EntityFactory - Creates entities based on type strings
 * Replaces hardcoded object creation in LevelLoader
 */
class EntityFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<Entity>(float x, float y)>;

    // Singleton pattern (optional)
    static EntityFactory& instance();

    // Register entity creators
    void registerCreator(const std::string& typeName, CreatorFunc creator);

    // Create entity by type name
    std::unique_ptr<Entity> create(const std::string& typeName, float x, float y);

    // Create and add to EntityManager
    Entity* createInManager(const std::string& typeName, float x, float y, EntityManager& manager);

    // Check if creator exists
    bool hasCreator(const std::string& typeName) const;

private:
    std::unordered_map<std::string, CreatorFunc> m_creators;
};