#pragma once

class GameState {
private:
    int m_score = 0;
    int m_lives = 3;
    float m_timeElapsed = 0.0f;
    int m_currentLevel = 0;
    bool m_gameOver = false;
    bool m_levelComplete = false;

public:
    // Score management
    void addScore(int amount) { m_score += amount; }
    int getScore() const { return m_score; }
    void resetScore() { m_score = 0; }

    // Lives management
    void loseLife() {
        if (m_lives > 0) {
            --m_lives;
            if (m_lives <= 0) {
                m_gameOver = true;
            }
        }
    }
    void addLife() {
        if (m_lives < 5) {  // Maximum 5 lives
            ++m_lives;
        }
    }
    int getLives() const { return m_lives; }
    void resetLives() { m_lives = 3; }

    // Time management
    void updateTime(float deltaTime) { m_timeElapsed += deltaTime; }
    float getTime() const { return m_timeElapsed; }
    void resetTime() { m_timeElapsed = 0.0f; }

    // Level management
    void setCurrentLevel(int level) { m_currentLevel = level; }
    int getCurrentLevel() const { return m_currentLevel; }
    void nextLevel() { ++m_currentLevel; }

    // Game state
    bool isGameOver() const { return m_gameOver; }
    bool isLevelComplete() const { return m_levelComplete; }
    void setLevelComplete(bool complete) { m_levelComplete = complete; }
    void setGameOver(bool gameOver) { m_gameOver = gameOver; }

    // Reset for new game
    void resetGame() {
        m_score = 0;
        m_lives = 3;
        m_timeElapsed = 0.0f;
        m_currentLevel = 0;
        m_gameOver = false;
        m_levelComplete = false;
    }

    // Reset for new level (keep score)
    void resetLevel() {
        m_timeElapsed = 0.0f;
        m_levelComplete = false;
    }
};