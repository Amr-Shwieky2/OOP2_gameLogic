#pragma once
#include <Box2D/Box2D.h>
#include <memory>

/**
 * PhysicsManager - Single Responsibility: Manage the physics world
 */
class PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    // Core physics operations
    void update(float deltaTime);
    b2World& getWorld() { return *m_world; }
    const b2World& getWorld() const { return *m_world; }

    // Physics world configuration
    void setGravity(const b2Vec2& gravity);
    b2Vec2 getGravity() const;

    // Simulation control
    void pausePhysics() { m_paused = true; }
    void resumePhysics() { m_paused = false; }
    bool isPaused() const { return m_paused; }

    // Performance tuning
    void setIterations(int velocityIterations, int positionIterations);

private:
    std::unique_ptr<b2World> m_world;
    bool m_paused = false;

    // Physics simulation parameters
    int m_velocityIterations = 8;
    int m_positionIterations = 3;
};
