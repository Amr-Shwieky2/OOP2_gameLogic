#pragma once
#include "Component.h"
#include <Box2D/Box2D.h>
#include <SFML/System/Vector2.hpp>

/**
 * PhysicsComponent - Handles Box2D physics for entities
 * Replaces the physics code scattered in Player, Enemy, etc.
 */
class PhysicsComponent : public Component {
public:
    PhysicsComponent(b2World& world, b2BodyType type = b2_dynamicBody);
    ~PhysicsComponent();

    // Component interface
    void update(float) override;
    void onDestroy() override;

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
    b2World& getWorld() { return m_world; }
    const b2World& getWorld() const { return m_world; }

    // Configure physics body
    void createCircleShape(float radius);
    void createBoxShape(float width, float height,
                        float density = 1.0f,
                        float friction = 0.3f,
                        float restitution = 0.1f);

private:
    b2Body* m_body = nullptr;
    b2World& m_world;
};