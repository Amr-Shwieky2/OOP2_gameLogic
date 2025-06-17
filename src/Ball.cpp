#include "Ball.h"
#include <cmath>

Ball::Ball(b2World& world, float startX, float startY, TextureManager& textures)
    : m_textures(textures) {

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(startX, startY);
    m_body = world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = BALL_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.0f; // Less bounce for realism
    m_body->CreateFixture(&fixtureDef);

    // Load default texture and configure sprite
    sf::Texture& tex = textures.getResource("NormalBall.png");
    m_sprite.setTexture(tex);
    m_sprite.setOrigin(tex.getSize().x / 2.f, tex.getSize().y / 2.f);

    float diameter = BALL_RADIUS * 2 * PPM;
    m_sprite.setScale(diameter / tex.getSize().x, diameter / tex.getSize().y);

    updateVisualState();
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
    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(pos.x * PPM, pos.y * PPM);
    m_sprite.setRotation(m_body->GetAngle() * 180.f / b2_pi);

    b2Vec2 vel = m_body->GetLinearVelocity();
    m_onGround = std::abs(vel.y) < 0.05f; // Approximate ground check
}

void Ball::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect Ball::getBounds() const {
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Ball::getPosition() const {
    b2Vec2 pos = m_body->GetPosition();
    return sf::Vector2f(pos.x * PPM, pos.y * PPM);
}

void Ball::setMagnetActive(bool active) {
    if (m_isMagnetic != active) {
        m_isMagnetic = active;
        updateVisualState();
    }
}

void Ball::setTransparent(bool active) {
    if (m_isTransparent != active) {
        m_isTransparent = active;
        updateVisualState();
    }
}

bool Ball::isMagnetActive() const {
    return m_isMagnetic;
}

bool Ball::isTransparent() const {
    return m_isTransparent;
}

void Ball::updateVisualState() {
    if (m_isTransparent) {
        m_sprite.setTexture(m_textures.getResource("TransparentBall.png"));
        m_sprite.setColor(sf::Color(255, 255, 255, 120));
    }
    else if (m_isMagnetic) {
        m_sprite.setTexture(m_textures.getResource("MagneticBall.png"));
        m_sprite.setColor(sf::Color::White);
    }
    else {
        m_sprite.setTexture(m_textures.getResource("NormalBall.png"));
        m_sprite.setColor(sf::Color::White);
    }
}
