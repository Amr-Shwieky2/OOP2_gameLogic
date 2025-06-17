// Player.h

#pragma once

#include "Ball.h"
#include "PlayerEffectManager.h"
#include "ResourceManager.h"

class Player {
public:
    Player(b2World& world, float startX, float startY, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float deltaTime);
    void updateEffects(float deltaTime);
    void render(sf::RenderTarget& target) const;

    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    int getScore() const;
    void addScore(int points);
    void reset();

    int getLives() const;
    void addLife();
    void loseLife();

    void applyEffect(PlayerEffect effect, float duration);
    bool hasEffect(PlayerEffect effect) const;

    TextureManager& getTextureManager();  // ✅ Add this

private:
    Ball m_ball;
    PlayerEffectManager m_effects;
    int m_score = 0;
    int m_lives = 3;
    TextureManager& m_textures;  
    float m_speedMultiplier;

};
