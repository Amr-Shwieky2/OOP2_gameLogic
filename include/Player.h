#pragma once

#include "Ball.h"

class Player {
public:
    Player(b2World& world, float startX, float startY, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    // Score and lives
    int getScore() const;
    void addScore(int points);

    int getLives() const;
    void loseLife();
    void reset();

    // Ball interaction
    sf::Vector2f getPosition() const;

private:
    Ball m_ball;
    int m_score = 0;
    int m_lives = 3;
};
