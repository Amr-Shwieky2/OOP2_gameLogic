#pragma once
#include <vector>
#include <memory>
#include "GameObject.h"
#include "DynamicGameObject.h"
#include "StaticGameObject.h"

class GameObjectManager {
public:
    GameObjectManager() = default;
    ~GameObjectManager() = default;

    // Object management
    void addStatic(std::unique_ptr<StaticGameObject> obj);
    void addDynamic(std::unique_ptr<DynamicGameObject> obj);
    void addGeneric(std::unique_ptr<GameObject> obj);

    // Clear all objects
    void clear();

    // Update all dynamic objects
    void updateDynamicObjects(float deltaTime);

    // Getters
    std::vector<std::unique_ptr<GameObject>>& getAllObjects() { return m_allObjects; }
    const std::vector<std::unique_ptr<GameObject>>& getAllObjects() const { return m_allObjects; }

    size_t getStaticCount() const { return m_staticObjects.size(); }
    size_t getDynamicCount() const { return m_dynamicObjects.size(); }
    size_t getTotalCount() const { return m_allObjects.size(); }

private:
    // Unified storage for all objects
    std::vector<std::unique_ptr<GameObject>> m_allObjects;

    // Quick access lists (store raw pointers for performance)
    std::vector<StaticGameObject*> m_staticObjects;
    std::vector<DynamicGameObject*> m_dynamicObjects;

    void addToQuickAccessLists(GameObject* obj);
};