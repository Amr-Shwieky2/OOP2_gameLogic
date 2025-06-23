#include "Player.h"

Player::Player(b2World& world, float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    // Box2D body
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
        m_facingRight = false;
    }
    if (input.isKeyDown(sf::Keyboard::Right)) {
        desiredVel = hasEffect(PlayerEffect::ReverseControl) ? -PLAYER_MOVE_SPEED : PLAYER_MOVE_SPEED;
        m_facingRight = true;
    }
    

    b2Vec2 vel = m_body->GetLinearVelocity();
    float velChange = desiredVel - vel.x;
    float impulse = m_body->GetMass() * velChange;
    m_body->ApplyLinearImpulseToCenter(b2Vec2(impulse, 0.f), true);

    if (input.isKeyPressed(sf::Keyboard::Up) && m_onGround) {
        m_body->ApplyLinearImpulseToCenter(b2Vec2(0.f, -PLAYER_JUMP_IMPULSE), true);
        m_onGround = false;
    }
}


void Player::update(float deltaTime) {
    m_effects.update(deltaTime);
    updateVisuals();
    updatePhysics(deltaTime);

    if (m_shootCooldown > 0.f)
        m_shootCooldown -= deltaTime;

    updateProjectiles(deltaTime);
}

void Player::updatePhysics(float) {
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

void Player::addLife()
{
    if (m_lives < 3) {
        ++m_lives;
    }
}

void Player::increaseScore(int amount) {
    m_score += amount;
}

void Player::loseLife() {
    if (m_lives > 0) --m_lives;
}

int Player::getScore() const {
    return m_score;
}

int Player::getLives() const {
    return m_lives;
}

void Player::applyEffect(PlayerEffect effect, float duration) {
    m_effects.applyEffect(effect, duration);
}

bool Player::hasEffect(PlayerEffect effect) const {
    return m_effects.hasEffect(effect);
}

TextureManager& Player::getTextureManager() {
    return m_textures;
}

void Player::updateVisuals() {
    if (hasEffect(PlayerEffect::Transparent)) {
        m_sprite.setTexture(m_textures.getResource("TransparentBall.png"));
    }
    else if (hasEffect(PlayerEffect::Magnetic)) {
        m_sprite.setTexture(m_textures.getResource("MagneticBall.png"));
    }
    else {
        m_sprite.setTexture(m_textures.getResource("NormalBall.png"));
    }
}

sf::Vector2f Player::getVelocity() const {
    b2Vec2 vel = m_body->GetLinearVelocity();
    return sf::Vector2f(vel.x * PPM, vel.y * PPM);
}

void Player::moveForward(float strength) {
    b2Vec2 velocity = m_body->GetLinearVelocity();
    velocity.x = strength * 5.0f;

    m_facingRight = velocity.x >= 0.f;
    m_body->SetLinearVelocity(velocity);
}


void Player::jump() {
    if (isOnGround()) { // You may already have this method
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
void Player::applyJumpImpulse() {
    if (m_body) {
        b2Vec2 impulse(0.f, -PLAYER_JUMP_IMPULSE / 1.5f); // Use weaker impulse than a normal jump
        m_body->ApplyLinearImpulseToCenter(impulse, true);
    }
}

bool Player::isFacingRight() const
{
    return m_facingRight;
}

void Player::shoot(TextureManager& textures) {
    if (m_shootCooldown > 0.f) return;

    float dir = isFacingRight() ? 1.f : -1.f;

    auto proj = std::make_unique<Projectile>(
        *m_body->GetWorld(),
        getPosition().x / PPM,
        getPosition().y / PPM,
        dir,
        textures,
        false
    );

    m_projectiles.push_back(std::move(proj));
    m_shootCooldown = m_fireInterval;
}

void Player::updateProjectiles(float deltaTime) {
    for (auto& p : m_projectiles)
        p->update(deltaTime);

    // Remove dead projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) {
                return !p->isAlive();
            }),
        m_projectiles.end()
    );
}

void Player::renderProjectiles(sf::RenderTarget& target) const {
    for (const auto& p : m_projectiles)
        p->render(target);
}

const std::vector<std::unique_ptr<Projectile>>& Player::getProjectiles() const {
    return m_projectiles;
}