#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "InputService.h"
#include "ResourceManager.h"

static constexpr float PPM = 100.f;              // Pixels Per Meter
static constexpr float BALL_RADIUS = 0.35f;      // In meters
static constexpr float MOVE_SPEED = 5.f;         // m/s
static constexpr float JUMP_IMPULSE = 2.8f;       // Force to jump

class Ball {
public:
    Ball(b2World& world, float startX, float startY, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float dt);
    void render(sf::RenderTarget& target) const;

    sf::Vector2f getPosition() const;

private:
    b2Body* m_body = nullptr;
    sf::Sprite m_sprite;
    bool m_onGround = false;


};
