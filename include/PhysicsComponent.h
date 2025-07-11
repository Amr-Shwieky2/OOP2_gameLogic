#pragma once
#include "Component.h"
#include <Box2D/Box2D.h>
#include <SFML/System/Vector2.hpp>
#include <iostream>

class Transform;

/**
 * PhysicsComponent - Handles Box2D physics for entities
 * Replaces the physics code scattered in Player, Enemy, etc.
 * Enhanced to work with specialized template methods in Entity class
 */
class PhysicsComponent : public Component {
public:
    PhysicsComponent(b2World& world, b2BodyType type = b2_dynamicBody);
    ~PhysicsComponent();

    // Component interface
    void update(float dt) override;
    void onDestroy() override;
    
    // Initialization method called by specialized template
    bool initialize();

    // Physics operations
    void setPosition(float x, float y);
    sf::Vector2f getPosition() const;

    void setVelocity(float x, float y);
    sf::Vector2f getVelocity() const;

    void applyForce(float x, float y);
    void applyImpulse(float x, float y);

    // Box2D body access
    b2Body* getBody() { return m_body; }
    const b2Body* getBody() const { return m_body; }

    // Configure physics body
    void createCircleShape(float radius, 
                          float density = 1.0f,
                          float friction = 0.3f,
                          float restitution = 0.1f);
                          
    void createBoxShape(float width, float height,
                        float density = 1.0f,
                        float friction = 0.3f,
                        float restitution = 0.1f);
                        
    void createSensorShape(float width, float height);

    // Enhanced features for specialized template
    void synchronizeWithTransform();
    bool validateDependencies() const;
    void configureBodyProperties(float density, float friction, float restitution);
    void setFixedRotation(bool fixed);
    void setGravityScale(float scale);
    
    // Debug helpers
    void debugDraw(bool enabled) { m_debugDraw = enabled; }
    bool isDebugDrawEnabled() const { return m_debugDraw; }

private:
    b2Body* m_body = nullptr;
    b2World& m_world;
    bool m_debugDraw = false;
    bool m_initialized = false;
    
    // Utility method to get transform component safely
    Transform* getTransformComponent() const;
};