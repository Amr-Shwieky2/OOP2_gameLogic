#include "PhysicsManager.h"
#include <iostream>

//-------------------------------------------------------------------------------------
PhysicsManager::PhysicsManager() {
    // Create physics world with standard gravity
    b2Vec2 gravity(0.0f, 9.8f);
    m_world = std::make_unique<b2World>(gravity);
}
//-------------------------------------------------------------------------------------
PhysicsManager::~PhysicsManager() {
}
//-------------------------------------------------------------------------------------
void PhysicsManager::update(float deltaTime) {
    if (m_paused || !m_world) {
        return;
    }
    m_world->Step(deltaTime, m_velocityIterations, m_positionIterations);
}
//-------------------------------------------------------------------------------------
void PhysicsManager::setGravity(const b2Vec2& gravity) {
    if (m_world) {
        m_world->SetGravity(gravity);
    }
}
//-------------------------------------------------------------------------------------
b2Vec2 PhysicsManager::getGravity() const {
    if (m_world) {
        return m_world->GetGravity();
    }
    return b2Vec2(0.0f, 0.0f);
}
//-------------------------------------------------------------------------------------
void PhysicsManager::setIterations(int velocityIterations, int positionIterations) {
    m_velocityIterations = velocityIterations;
    m_positionIterations = positionIterations;
}
//-------------------------------------------------------------------------------------