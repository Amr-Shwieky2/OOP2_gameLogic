#include "Player.h"

Player::Player(b2World& world, float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    // Initialize components
    m_movement = std::make_unique<PlayerMovement>(world, x, y);
    m_renderer = std::make_unique<PlayerRenderer>(textures);
    m_weapon = std::make_unique<PlayerWeapon>(world);
    m_stats = std::make_unique<PlayerStats>(3);
}

void Player::update(float deltaTime) {
    m_effects.update(deltaTime);
    m_movement->updatePhysics(deltaTime);
    m_weapon->updateProjectiles(deltaTime);

    // Update renderer with current state
    m_renderer->updateVisuals(
        getPosition(),
        m_movement->getRotation(),
        hasEffect(PlayerEffect::Transparent),
        hasEffect(PlayerEffect::Magnetic)
    );
}

void Player::render(sf::RenderTarget& target) const {
    m_renderer->render(target);
    m_weapon->renderProjectiles(target);
}

sf::FloatRect Player::getBounds() const {
    return m_renderer->getBounds();
}

void Player::handleInput(const InputService& input) {
    m_movement->handleInput(
        input,
        hasEffect(PlayerEffect::ReverseControl),
        hasEffect(PlayerEffect::SpeedBoost),
        hasEffect(PlayerEffect::Headwind)
    );
}

sf::Vector2f Player::getPosition() const {
    return m_movement->getPosition();
}

sf::Vector2f Player::getVelocity() const {
    return m_movement->getVelocity();
}

void Player::moveForward(float strength) {
    m_movement->moveForward(strength);
}

void Player::jump() {
    m_movement->jump();
}

bool Player::isOnGround() const {
    return m_movement->isOnGround();
}

bool Player::isFacingRight() const {
    return m_movement->isFacingRight();
}

void Player::shoot(TextureManager& textures) {
    m_weapon->shoot(getPosition(), isFacingRight(), textures);
}

const std::vector<std::unique_ptr<Projectile>>& Player::getProjectiles() const {
    return m_weapon->getProjectiles();
}

void Player::addLife() {
    m_stats->addLife();
}

void Player::loseLife() {
    m_stats->loseLife();
}

void Player::increaseScore(int amount) {
    m_stats->increaseScore(amount);
}

int Player::getScore() const {
    return m_stats->getScore();
}

int Player::getLives() const {
    return m_stats->getLives();
}

void Player::applyEffect(PlayerEffect effect, float duration) {
    m_effects.applyEffect(effect, duration);
}

bool Player::hasEffect(PlayerEffect effect) const {
    return m_effects.hasEffect(effect);
}

void Player::beginContact() {
    m_movement->beginContact();
}

void Player::endContact() {
    m_movement->endContact();
}

void Player::applyJumpImpulse() {
    m_movement->applyJumpImpulse();
}

TextureManager& Player::getTextureManager() {
    return m_textures;
}

sf::Vector2f Player::getSpriteCenter() const {
    return m_renderer->getSpriteCenter();
}
void Player::kill() {
    m_stats->kill();
}