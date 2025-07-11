#include "PhysicsComponent.h"
#include "Entity.h"
#include "Transform.h"
#include "Constants.h"
#include <CollisionComponent.h>
#include <iostream>

// Define a constant for pixels per meter if not defined elsewhere
#ifndef PIXEL_PER_METER
#define PIXEL_PER_METER 30.0f
#endif

PhysicsComponent::PhysicsComponent(b2World& world, b2BodyType type)
    : m_world(world), m_debugDraw(false), m_initialized(false) {

    // Create body definition
    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(0.0f, 0.0f);

    // Create the body
    m_body = world.CreateBody(&bodyDef);
    m_body->SetUserData(this);
}

PhysicsComponent::~PhysicsComponent() {
    if (m_body) {
        m_body->SetUserData(nullptr);
        m_world.DestroyBody(m_body);
        m_body = nullptr;
    }
}

bool PhysicsComponent::initialize() {
    if (!m_owner) {
        std::cerr << "[ERROR] PhysicsComponent::initialize - No owner entity set" << std::endl;
        return false;
    }

    // Set user data to point to owner entity for collision callbacks
    if (m_body) {
        m_body->SetUserData(static_cast<void*>(m_owner));

        // Synchronize with transform if it exists
        synchronizeWithTransform();
        m_initialized = true;
        return true;
    }

    std::cerr << "[ERROR] PhysicsComponent::initialize - No Box2D body created" << std::endl;
    return false;
}

void PhysicsComponent::update(float dt) {
    if (!m_body || !m_initialized) {
        return;
    }

    synchronizeWithTransform();
}

void PhysicsComponent::onDestroy() {
    if (m_body) {
        m_body->SetUserData(nullptr);
        m_world.DestroyBody(m_body);
        m_body = nullptr;
    }
}

void PhysicsComponent::setPosition(float x, float y) {
    if (!m_body) {
        return;
    }

    // Convert to Box2D coordinates (divide by pixels per meter)
    b2Vec2 position(x / PIXEL_PER_METER, y / PIXEL_PER_METER);
    m_body->SetTransform(position, m_body->GetAngle());

    // Update transform component directly for immediate visual feedback
    if (auto* transform = getTransformComponent()) {
        transform->setPosition(x, y);
    }
}

sf::Vector2f PhysicsComponent::getPosition() const {
    if (!m_body) {
        return sf::Vector2f(0, 0);
    }

    b2Vec2 position = m_body->GetPosition();

    // Convert to pixel coordinates
    return sf::Vector2f(
        position.x * PIXEL_PER_METER,
        position.y * PIXEL_PER_METER
    );
}

void PhysicsComponent::setVelocity(float x, float y) {
    if (!m_body) {
        return;
    }

    // Convert to Box2D units
    b2Vec2 velocity(x / PIXEL_PER_METER, y / PIXEL_PER_METER);
    m_body->SetLinearVelocity(velocity);
}

sf::Vector2f PhysicsComponent::getVelocity() const {
    if (!m_body) {
        return sf::Vector2f(0, 0);
    }

    b2Vec2 velocity = m_body->GetLinearVelocity();

    // Convert to pixel units
    return sf::Vector2f(
        velocity.x * PIXEL_PER_METER,
        velocity.y * PIXEL_PER_METER
    );
}

void PhysicsComponent::applyForce(float x, float y) {
    if (!m_body) {
        return;
    }

    // Convert to Box2D units
    b2Vec2 force(x / PIXEL_PER_METER, y / PIXEL_PER_METER);
    m_body->ApplyForceToCenter(force, true);
}

void PhysicsComponent::applyImpulse(float x, float y) {
    if (!m_body) {
        return;
    }

    // Convert to Box2D units
    b2Vec2 impulse(x / PIXEL_PER_METER, y / PIXEL_PER_METER);
    m_body->ApplyLinearImpulse(impulse, m_body->GetWorldCenter(), true);
}

void PhysicsComponent::createCircleShape(float radius, float density, float friction, float restitution) {
    if (!m_body) return;

    // Create circle shape
    b2CircleShape circleShape;
    circleShape.m_radius = radius / PIXEL_PER_METER;

    // Create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;

    // Add fixture to body
    m_body->CreateFixture(&fixtureDef);

    // Log creation
    if (m_debugDraw) {
        std::cout << "[PhysicsComponent] Created circle shape with radius: "
                  << radius << " pixels (" << circleShape.m_radius << " meters)" << std::endl;
    }
}

void PhysicsComponent::createBoxShape(float width, float height, float density, float friction, float restitution) {
    if (!m_body) return;

    // Convert to Box2D units (meters)
    float halfWidth = (width / 2) / PIXEL_PER_METER;
    float halfHeight = (height / 2) / PIXEL_PER_METER;

    // Create box shape
    b2PolygonShape boxShape;
    boxShape.SetAsBox(halfWidth, halfHeight);

    // Create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;

    // Add fixture to body
    m_body->CreateFixture(&fixtureDef);

    // Log creation
    if (m_debugDraw) {
        std::cout << "[PhysicsComponent] Created box shape: "
                  << width << "x" << height << " pixels ("
                  << halfWidth * 2 << "x" << halfHeight * 2 << " meters)" << std::endl;
    }
}

void PhysicsComponent::createSensorShape(float width, float height) {
    if (!m_body) return;

    // Convert to Box2D units (meters)
    float halfWidth = (width / 2) / PIXEL_PER_METER;
    float halfHeight = (height / 2) / PIXEL_PER_METER;

    // Create sensor shape
    b2PolygonShape sensorShape;
    sensorShape.SetAsBox(halfWidth, halfHeight);

    // Create sensor fixture (isSensor = true)
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &sensorShape;
    fixtureDef.isSensor = true;
    fixtureDef.density = 0.0f;

    // Add fixture to body
    m_body->CreateFixture(&fixtureDef);

    // Log creation
    if (m_debugDraw) {
        std::cout << "[PhysicsComponent] Created sensor shape: "
                  << width << "x" << height << " pixels" << std::endl;
    }
}

void PhysicsComponent::synchronizeWithTransform() {
    auto* transform = getTransformComponent();
    if (transform && m_body) {
        // Update transform based on physics position
        b2Vec2 position = m_body->GetPosition();
        float angle = m_body->GetAngle() * 180.0f / b2_pi; // Convert to degrees

        // Convert to pixel coordinates
        float x = position.x * PIXEL_PER_METER;
        float y = position.y * PIXEL_PER_METER;

        // Update transform (without triggering physics update)
        transform->setPosition(x, y);
        transform->setRotation(angle);
    }
}

bool PhysicsComponent::validateDependencies() const {
    if (!m_owner) {
        std::cerr << "[ERROR] PhysicsComponent has no owner entity" << std::endl;
        return false;
    }

    if (!m_body) {
        std::cerr << "[ERROR] PhysicsComponent has no Box2D body" << std::endl;
        return false;
    }

    if (!getTransformComponent()) {
        std::cerr << "[ERROR] Entity requires a Transform component for PhysicsComponent" << std::endl;
        return false;
    }

    return true;
}

void PhysicsComponent::configureBodyProperties(float density, float friction, float restitution) {
    if (!m_body) return;

    // Apply properties to all fixtures
    for (b2Fixture* fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        fixture->SetDensity(density);
        fixture->SetFriction(friction);
        fixture->SetRestitution(restitution);
    }

    // Recalculate mass data after changing density
    m_body->ResetMassData();
}

void PhysicsComponent::setFixedRotation(bool fixed) {
    if (m_body) {
        m_body->SetFixedRotation(fixed);
    }
}

void PhysicsComponent::setGravityScale(float scale) {
    if (m_body) {
        m_body->SetGravityScale(scale);
    }
}

Transform* PhysicsComponent::getTransformComponent() const {
    Entity* owner = getOwner();
    return owner ? owner->getComponent<Transform>() : nullptr;
}

// Property registration for PhysicsComponent
void PhysicsComponent::registerProperties() {
    // Call base implementation
    Component::registerProperties();

    // Register physics properties
    registerProperty<PhysicsComponent, sf::Vector2f>(
        "velocity",
        PropertyValue::Type::Vector2,
        &PhysicsComponent::getVelocity,
        &PhysicsComponent::setVelocity,
        PropertyDescriptor::Category::Physics
    );

    // Register individual velocity components
    registerProperty<PhysicsComponent, float>(
        "velocityX",
        PropertyValue::Type::Float,
        [](const PhysicsComponent* c) -> float { return c->getVelocity().x; },
        [](PhysicsComponent* c, float value) {
            sf::Vector2f vel = c->getVelocity();
            c->setVelocity(value, vel.y);
        },
        PropertyDescriptor::Category::Physics
    );

    registerProperty<PhysicsComponent, float>(
        "velocityY",
        PropertyValue::Type::Float,
        [](const PhysicsComponent* c) -> float { return c->getVelocity().y; },
        [](PhysicsComponent* c, float value) {
            sf::Vector2f vel = c->getVelocity();
            c->setVelocity(vel.x, value);
        },
        PropertyDescriptor::Category::Physics
    );

    // Register body type (read-only since changing it would require recreating the body)
    registerReadOnlyProperty<PhysicsComponent, int>(
        "bodyType",
        PropertyValue::Type::Int,
        [](const PhysicsComponent* c) -> int {
            return c->m_body ? static_cast<int>(c->m_body->GetType()) : 0;
        },
        PropertyDescriptor::Category::Physics
    );

    // Register gravity scale
    registerProperty<PhysicsComponent, float>(
        "gravityScale",
        PropertyValue::Type::Float,
        [](const PhysicsComponent* c) -> float {
            return c->m_body ? c->m_body->GetGravityScale() : 1.0f;
        },
        &PhysicsComponent::setGravityScale,
        PropertyDescriptor::Category::Physics
    );

    // Register fixed rotation
    registerProperty<PhysicsComponent, bool>(
        "fixedRotation",
        PropertyValue::Type::Bool,
        [](const PhysicsComponent* c) -> bool {
            return c->m_body ? c->m_body->IsFixedRotation() : false;
        },
        &PhysicsComponent::setFixedRotation,
        PropertyDescriptor::Category::Physics
    );

    // Register debug draw
    registerProperty<PhysicsComponent, bool>(
        "debugDraw",
        PropertyValue::Type::Bool,
        &PhysicsComponent::isDebugDrawEnabled,
        &PhysicsComponent::debugDraw,
        PropertyDescriptor::Category::Debug
    );

    // Configure property display
    configureProperty("velocity", "Velocity", "The current velocity in pixels per second");
    configureProperty("velocityX", "X Velocity", "The horizontal velocity in pixels per second");
    configureProperty("velocityY", "Y Velocity", "The vertical velocity in pixels per second");
    configureProperty("bodyType", "Body Type", "The Box2D body type (0=static, 1=kinematic, 2=dynamic)");
    configureProperty("gravityScale", "Gravity Scale", "How much gravity affects this body");
    configureProperty("fixedRotation", "Fixed Rotation", "Prevents the body from rotating");
    configureProperty("debugDraw", "Debug Draw", "Show physics debug information");

    // Set ranges for numeric properties
    setPropertyRange("gravityScale", -2.0, 2.0);
}
