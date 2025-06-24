#include "PlayerStats.h"
#include <algorithm>

PlayerStats::PlayerStats(int initialLives) : m_lives(initialLives) {}

void PlayerStats::addLife() {
    if (m_lives < MAX_LIVES) {
        ++m_lives;
    }
}

void PlayerStats::loseLife() {
    if (m_lives > 0) --m_lives;
}

int PlayerStats::getLives() const {
    return m_lives;
}

void PlayerStats::increaseScore(int amount) {
    m_score += amount;
}

int PlayerStats::getScore() const {
    return m_score;
}

void PlayerStats::resetScore() {
    m_score = 0;
}

void PlayerStats::kill() {
    m_lives = 0;
}
