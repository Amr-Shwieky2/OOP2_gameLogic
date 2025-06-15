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

int Player::getScore() const {
    return m_score;
}

void Player::addScore(int points) {
    m_score += points;
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
