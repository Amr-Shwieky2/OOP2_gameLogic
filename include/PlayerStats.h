#pragma once

class PlayerStats {
public:
    PlayerStats(int initialLives = 3);
    
    // Life management
    void addLife();
    void loseLife();
    int getLives() const;

    // Score management
    void increaseScore(int amount);
    int getScore() const;
    void resetScore();

    void kill();

private:
    int m_lives;
    int m_score = 0;
    static constexpr int MAX_LIVES = 5;
};
