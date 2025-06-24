#include "GameObjectManager.h"
#include <algorithm>

void GameObjectManager::addStatic(std::unique_ptr<StaticGameObject> obj) {
    if (!obj) return;

    GameObject* rawPtr = obj.get();
    m_allObjects.push_back(std::move(obj));
    addToQuickAccessLists(rawPtr);
}

void GameObjectManager::addDynamic(std::unique_ptr<DynamicGameObject> obj) {
    if (!obj) return;

    GameObject* rawPtr = obj.get();
    m_allObjects.push_back(std::move(obj));
    addToQuickAccessLists(rawPtr);
}

void GameObjectManager::addGeneric(std::unique_ptr<GameObject> obj) {
    if (!obj) return;

    GameObject* rawPtr = obj.get();
    m_allObjects.push_back(std::move(obj));
    addToQuickAccessLists(rawPtr);
}

void GameObjectManager::clear() {
    m_allObjects.clear();
    m_staticObjects.clear();
    m_dynamicObjects.clear();
}

void GameObjectManager::updateDynamicObjects(float deltaTime) {
    // Remove dead objects first
    m_dynamicObjects.erase(
        std::remove_if(m_dynamicObjects.begin(), m_dynamicObjects.end(),
            [](DynamicGameObject* obj) { return obj == nullptr; }),
        m_dynamicObjects.end()
    );

    // Update all dynamic objects
    for (auto* obj : m_dynamicObjects) {
        if (obj) {
            obj->update(deltaTime);
        }
    }
}

void GameObjectManager::addToQuickAccessLists(GameObject* obj) {
    if (!obj) return;

    if (auto* staticObj = dynamic_cast<StaticGameObject*>(obj)) {
        m_staticObjects.push_back(staticObj);
    }
    else if (auto* dynamicObj = dynamic_cast<DynamicGameObject*>(obj)) {
        m_dynamicObjects.push_back(dynamicObj);
    }
}
