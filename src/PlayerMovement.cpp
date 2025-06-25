#include "PlayerMovement.h"
#include "Constants.h"

PlayerMovement::PlayerMovement(b2World& world, float x, float y) {
    // Box2D body setup
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    m_body = world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = PLAYER_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.4f;
    fixtureDef.restitution = 0.2f;
    m_body->CreateFixture(&fixtureDef);
}

PlayerMovement::~PlayerMovement() {
    if (m_body) {
        m_body->GetWorld()->DestroyBody(m_body);
    }
}

void PlayerMovement::handleInput(const InputService& input, bool hasReverseEffect, bool hasSpeedBoost, bool hasHeadwind) {
    float desiredVel = 0.f;
    float moveSpeed = PLAYER_MOVE_SPEED;

    // Apply effects to speed
    if (hasSpeedBoost) moveSpeed *= 1.3f;
    if (hasHeadwind) moveSpeed *= 0.5f;

    // Handle left/right movement
    if (input.isKeyDown(sf::Keyboard::Left)) {
        desiredVel = hasReverseEffect ? moveSpeed : -moveSpeed;
        m_facingRight = false;
    }
    if (input.isKeyDown(sf::Keyboard::Right)) {
        desiredVel = hasReverseEffect ? -moveSpeed : moveSpeed;
        m_facingRight = true;
    }

    // Apply movement
    b2Vec2 vel = m_body->GetLinearVelocity();
    float velChange = desiredVel - vel.x;
    float impulse = m_body->GetMass() * velChange;
    m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.f), true);

    // Handle jumping
    bool shouldJump = hasReverseEffect ?
        input.isKeyPressed(sf::Keyboard::Down) :
        input.isKeyPressed(sf::Keyboard::Up);

    if (shouldJump && m_onGround) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0.f, -PLAYER_JUMP_IMPULSE), true);
        m_onGround = false;
    }
}

void PlayerMovement::updatePhysics(float deltaTime) {
    m_onGround = std::abs(m_body->GetLinearVelocity().y) < 0.05f;
}

void PlayerMovement::moveForward(float strength) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    velocity.x = strength * 5.0f;
    m_facingRight = velocity.x >= 0.f;
    m_body->SetLinearVelocity(velocity);
}

void PlayerMovement::jump() {
    if (isOnGround()) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.5f), true);
    }
}

void PlayerMovement::applyJumpImpulse() {
    if (m_body) {
        b2Vec2 impulse(0.f, -PLAYER_JUMP_IMPULSE / 1.5f);
        m_body->ApplyLinearImpulseToCenter(impulse, true);
    }
}

void PlayerMovement::applyImpulse(const sf::Vector2f& impulse)
{
    m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), true);
}

sf::Vector2f PlayerMovement::getPosition() const {
    b2Vec2 pos = m_body->GetPosition();
    return sf::Vector2f(pos.x * PPM, pos.y * PPM);
}

void PlayerMovement::setPosition(const sf::Vector2f& position)
{
    m_body->SetTransform(b2Vec2(position.x, position.y), m_body->GetAngle());
}

sf::Vector2f PlayerMovement::getVelocity() const {
    b2Vec2 vel = m_body->GetLinearVelocity();
    return sf::Vector2f(vel.x * PPM, vel.y * PPM);
}

float PlayerMovement::getRotation() const {
    return m_body->GetAngle() * 180.f / b2_pi;
}

bool PlayerMovement::isFacingRight() const {
    return m_facingRight;
}

bool PlayerMovement::isOnGround() const {
    return m_groundContacts > 0;
}

void PlayerMovement::beginContact() {
    ++m_groundContacts;
}

void PlayerMovement::endContact() {
    if (m_groundContacts > 0)
        --m_groundContacts;
}