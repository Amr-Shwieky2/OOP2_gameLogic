#include "Ball.h"
#include <cmath>

Ball::Ball(b2World& world, float startX, float startY, TextureManager& textures) {
    // Create dynamic physics body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(startX, startY);
    m_body = world.CreateBody(&bodyDef);

    // Define circular shape
    b2CircleShape shape;
    shape.m_radius = BALL_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.4f;
    fixtureDef.restitution = 0.2f; // slight bounce
    m_body->CreateFixture(&fixtureDef);

    // Load texture and prepare sprite
    sf::Texture& texture = textures.getResource("NormalBall.png");
    m_sprite.setTexture(texture);

    float diameter = BALL_RADIUS * 2 * PPM;
    m_sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    m_sprite.setScale(diameter / texture.getSize().x, diameter / texture.getSize().y);
}

void Ball::handleInput(const InputService& input) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    float desiredVel = 0.f;

    if (input.isKeyDown(sf::Keyboard::Left)) {
        desiredVel = -MOVE_SPEED;
    }
    if (input.isKeyDown(sf::Keyboard::Right)) {
        desiredVel = MOVE_SPEED;
    }

    float velChange = desiredVel - velocity.x;
    float impulse = m_body->GetMass() * velChange;
    m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.f), true);

    if (input.isKeyPressed(sf::Keyboard::Space) && m_onGround) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0.f, -JUMP_IMPULSE), true);
        m_onGround = false;
    }
}

void Ball::update(float) {
    // Sync sprite to physics body
    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(pos.x * PPM, pos.y * PPM);
    m_sprite.setRotation(m_body->GetAngle() * 180.f / b2_pi);

    // Update ground contact
    b2Vec2 vel = m_body->GetLinearVelocity();
    m_onGround = std::abs(vel.y) < 0.05f;
}

void Ball::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
    
}

sf::Vector2f Ball::getPosition() const {
    b2Vec2 pos = m_body->GetPosition();
    return sf::Vector2f(pos.x * PPM, pos.y * PPM);
}
