#include "PhysicsComponent.h"
#include "Entity.h"
#include "Transform.h"
#include "Constants.h"

PhysicsComponent::PhysicsComponent(b2World& world, b2BodyType type)
    : m_world(world) {

    // Create body definition
    b2BodyDef bodyDef;
    bodyDef.type = type;
    bodyDef.position.Set(0.0f, 0.0f);

    // Create the body
    m_body = world.CreateBody(&bodyDef);

    // Store pointer to owner entity in body user data
    // This will be set when component is added to entity
}

PhysicsComponent::~PhysicsComponent() {
    if (m_body) {
        m_world.DestroyBody(m_body);
        m_body = nullptr;
    }
}

void PhysicsComponent::update(float) {
    if (!m_body || !m_owner) return;

    // Sync transform with physics body
    if (auto* transform = m_owner->getComponent<Transform>()) {
        b2Vec2 pos = m_body->GetPosition();
        transform->setPosition(pos.x * PPM, pos.y * PPM);
        transform->setRotation(m_body->GetAngle() * 180.0f / b2_pi);
    }
}

void PhysicsComponent::onDestroy() {
    if (m_body) {
        m_world.DestroyBody(m_body);
        m_body = nullptr;
    }
}

void PhysicsComponent::setPosition(float x, float y) {
    if (m_body) {
        m_body->SetTransform(b2Vec2(x / PPM, y / PPM), m_body->GetAngle());
    }
}

sf::Vector2f PhysicsComponent::getPosition() const {
    if (m_body) {
        b2Vec2 pos = m_body->GetPosition();
        return sf::Vector2f(pos.x * PPM, pos.y * PPM);
    }
    return sf::Vector2f(0.0f, 0.0f);
}

void PhysicsComponent::setVelocity(float x, float y) {
    if (m_body) {
        m_body->SetLinearVelocity(b2Vec2(x, y));
    }
}

sf::Vector2f PhysicsComponent::getVelocity() const {
    if (m_body) {
        b2Vec2 vel = m_body->GetLinearVelocity();
        return sf::Vector2f(vel.x, vel.y);
    }
    return sf::Vector2f(0.0f, 0.0f);
}

void PhysicsComponent::applyForce(float x, float y) {
    if (m_body) {
        m_body->ApplyForceToCenter(b2Vec2(x, y), true);
    }
}

void PhysicsComponent::applyImpulse(float x, float y) {
    if (m_body) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(x, y), true);
    }
}

void PhysicsComponent::createCircleShape(float radius) {
    if (!m_body) return;

    // Remove existing fixtures
    b2Fixture* fixture = m_body->GetFixtureList();
    while (fixture) {
        b2Fixture* next = fixture->GetNext();
        m_body->DestroyFixture(fixture);
        fixture = next;
    }

    // Create circle shape
    b2CircleShape circle;
    circle.m_radius = radius / PPM;

    // Create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.1f;

    m_body->CreateFixture(&fixtureDef);
}

void PhysicsComponent::createBoxShape(float width, float height) {
    if (!m_body) return;

    // Remove existing fixtures
    b2Fixture* fixture = m_body->GetFixtureList();
    while (fixture) {
        b2Fixture* next = fixture->GetNext();
        m_body->DestroyFixture(fixture);
        fixture = next;
    }

    // Create box shape
    b2PolygonShape box;
    box.SetAsBox(width / (2.0f * PPM), height / (2.0f * PPM));

    // Create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.1f;

    m_body->CreateFixture(&fixtureDef);
}