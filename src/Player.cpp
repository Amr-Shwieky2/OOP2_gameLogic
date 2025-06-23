#include "Player.h"

Player::Player(b2World& world, float x, float y, TextureManager& textures)
    : m_textures(textures)
{
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

    // Visual setup
    m_sprite.setTexture(textures.getResource("NormalBall.png"));

    // Set origin to center of the texture
    sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
    m_sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);

    // Compute desired display size
    float desiredDiameter = PLAYER_RADIUS * 2 * PPM;
    float scaleX = desiredDiameter / textureSize.x;
    float scaleY = desiredDiameter / textureSize.y;
    m_sprite.setScale(scaleX, scaleY);

    updateVisuals();
}

void Player::handleInput(const InputService& input) {
    float desiredVel = 0.f;

    if (input.isKeyDown(sf::Keyboard::Left)) {
        desiredVel = hasEffect(PlayerEffect::ReverseControl) ? PLAYER_MOVE_SPEED : -PLAYER_MOVE_SPEED;
    }
    if (input.isKeyDown(sf::Keyboard::Right)) {
        desiredVel = hasEffect(PlayerEffect::ReverseControl) ? -PLAYER_MOVE_SPEED : PLAYER_MOVE_SPEED;
    }

    // Apply speed boost effect
    if (hasEffect(PlayerEffect::SpeedBoost)) {
        desiredVel *= 1.5f;  // 50% speed increase
    }

    b2Vec2 vel = m_body->GetLinearVelocity();
    float velChange = desiredVel - vel.x;
    float impulse = m_body->GetMass() * velChange;
    m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.f), true);

    if (input.isKeyPressed(sf::Keyboard::Space) && m_onGround) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0.f, -PLAYER_JUMP_IMPULSE), true);
        m_onGround = false;
    }
}

void Player::update(float deltaTime) {
    m_effects.update(deltaTime);
    updateVisuals();
    updatePhysics(deltaTime);
}

void Player::updatePhysics(float deltaTime) {
    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(pos.x * PPM, pos.y * PPM);
    m_sprite.setRotation(m_body->GetAngle() * 180.f / b2_pi);

    m_onGround = std::abs(m_body->GetLinearVelocity().y) < 0.05f;
}

void Player::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect Player::getBounds() const {
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
    b2Vec2 pos = m_body->GetPosition();
    return sf::Vector2f(pos.x * PPM, pos.y * PPM);
}

sf::Vector2f Player::getVelocity() const {
    b2Vec2 vel = m_body->GetLinearVelocity();
    return sf::Vector2f(vel.x * PPM, vel.y * PPM);
}

void Player::moveForward(float strength) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    velocity.x = strength * 5.0f;
    m_body->SetLinearVelocity(velocity);
}

void Player::jump() {
    if (isOnGround()) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0, -0.5f), true);
    }
}

bool Player::isOnGround() const {
    return m_groundContacts > 0;
}

void Player::beginContact() {
    ++m_groundContacts;
}

void Player::endContact() {
    if (m_groundContacts > 0)
        --m_groundContacts;
}

void Player::applyEffect(PlayerEffect effect, float duration) {
    m_effects.applyEffect(effect, duration);
}

bool Player::hasEffect(PlayerEffect effect) const {
    return m_effects.hasEffect(effect);
}

void Player::updateVisuals() {
    // Change sprite based on active effects
    if (hasEffect(PlayerEffect::Transparent)) {
        m_sprite.setTexture(m_textures.getResource("TransparentBall.png"));
    }
    else if (hasEffect(PlayerEffect::Magnetic)) {
        m_sprite.setTexture(m_textures.getResource("MagneticBall.png"));
    }
    else if (hasEffect(PlayerEffect::Shield)) {
        // Could add shield visual effect here
        m_sprite.setTexture(m_textures.getResource("NormalBall.png"));
    }
    else {
        m_sprite.setTexture(m_textures.getResource("NormalBall.png"));
    }
}