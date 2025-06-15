#include "Player.h"

Player::Player(b2World& world, float startX, float startY, TextureManager& textures)
    : m_ball(world, startX, startY, textures) {
}

void Player::handleInput(const InputService& input) {
    m_ball.handleInput(input);
}

void Player::update(float deltaTime) {
    m_ball.update(deltaTime);
}

void Player::render(sf::RenderTarget& target) const {
    m_ball.render(target);
}

sf::Vector2f Player::getPosition() const {
    return m_ball.getPosition();
}

sf::FloatRect Player::getBounds() const
{
    return m_ball.getBounds();
}

int Player::getScore() const {
    return m_score;
}

void Player::addScore(int points) {
    m_score += points;
}

void Player::addLife()
{
    if (m_lives < 3)
        ++m_lives;
}

int Player::getLives() const {
    return m_lives;
}

void Player::loseLife() {
    if (m_lives > 0)
        --m_lives;
}

void Player::reset() {
    m_score = 0;
    m_lives = 3;
}

void Player::applyEffect(PlayerEffect effect, float duration) {
    m_effects.applyEffect(effect, duration);
}

bool Player::hasEffect(PlayerEffect effect) const {
    return m_effects.hasEffect(effect);
}

void Player::updateEffects(float deltaTime) {
    m_effects.update(deltaTime);

    // Example: Apply speed boost multiplier
    if (hasEffect(PlayerEffect::SpeedBoost)) {
        m_speedMultiplier = 1.8f;
    }
    else if (hasEffect(PlayerEffect::Headwind)) {
        m_speedMultiplier = 0.5f;
    }
    else {
        m_speedMultiplier = 1.0f;
    }

    // You can later add logic here for Shield, ReverseControl, etc.
}
