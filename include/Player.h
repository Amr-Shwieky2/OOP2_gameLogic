#pragma once

#include "Ball.h"
#include "InputService.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "PlayerEffectManager.h"

class Player {
public:
    Player(b2World& world, float startX, float startY, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float deltaTime);
    void render(sf::RenderTarget& target) const;

    // Score and lives
    int getScore() const;
    void addScore(int points);

    void addLife();
    int getLives() const;
    void loseLife();
    void reset();

    // Ball interaction
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    void applyEffect(PlayerEffect effect, float duration);
    bool hasEffect(PlayerEffect effect) const;
    void updateEffects(float deltaTime);

private:
    PlayerEffectManager m_effects;
    Ball m_ball;
    int m_score = 0;
    int m_lives = 3;
    float m_speedMultiplier = 1.0f;
};
