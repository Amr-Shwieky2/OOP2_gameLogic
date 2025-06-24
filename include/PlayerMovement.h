#pragma once
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "InputService.h"
#include "PlayerEffect.h"
#include "Constants.h" 

class PlayerMovement {
public:
    PlayerMovement(b2World& world, float x, float y);
    ~PlayerMovement();

    void handleInput(const InputService& input, bool hasReverseEffect, bool hasSpeedBoost, bool hasHeadwind);
    void updatePhysics(float deltaTime);

    // Movement methods
    void moveForward(float strength);
    void jump();
    void applyJumpImpulse();

    // Getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    float getRotation() const;
    bool isFacingRight() const;
    bool isOnGround() const;

    // Ground contact management
    void beginContact();
    void endContact();

    b2Body* getBody() { return m_body; }

private:
    b2Body* m_body = nullptr;
    int m_groundContacts = 0;
    bool m_onGround = false;
    bool m_facingRight = true;
};
