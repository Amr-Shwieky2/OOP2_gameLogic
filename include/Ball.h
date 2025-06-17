#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "InputService.h"
#include "ResourceManager.h"

static constexpr float PPM = 100.f;
static constexpr float BALL_RADIUS = 0.35f; // Better fit on tile
static constexpr float MOVE_SPEED = 5.f;
static constexpr float JUMP_IMPULSE = 3.2f;

class Ball {
public:
    Ball(b2World& world, float startX, float startY, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float dt);
    void render(sf::RenderTarget& target) const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    void setMagnetActive(bool active);
    void setTransparent(bool active);

    bool isMagnetActive() const;
    bool isTransparent() const;

private:
    void updateVisualState();

    b2Body* m_body = nullptr;
    sf::Sprite m_sprite;
    TextureManager& m_textures;

    bool m_onGround = false;
    bool m_isMagnetic = false;
    bool m_isTransparent = false;
};
