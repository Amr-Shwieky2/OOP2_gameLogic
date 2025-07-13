#include "ReversedState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Constants.h"
#include <cmath>
#include <iostream>

std::unique_ptr<ReversedState> ReversedState::s_instance = nullptr;

PlayerState* ReversedState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<ReversedState>(new ReversedState());
    }
    return s_instance.get();
}

void ReversedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Reversed state - Controls inverted!" << std::endl;
    m_duration = 10.0f;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(200, 150, 255));
    }
}

void ReversedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Reversed state - Controls normal" << std::endl;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void ReversedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    if (m_duration <= 0) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->changeState(NormalState::getInstance());
        }
    }
}

void ReversedState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED;
    auto vel = physics->getVelocity();

    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    if (input.isKeyPressed(player.getJumpKey()) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }

    // Forward shooting with C key
    if (input.isKeyPressed(sf::Keyboard::C)) {
        if (auto* weaponSystem = player.getWeaponSystem()) {
            weaponSystem->shoot();
        }
    }
    
    // Backward shooting with B key
    if (input.isKeyPressed(sf::Keyboard::B)) {
        if (auto* weaponSystem = player.getWeaponSystem()) {
            weaponSystem->shootBackward();
        }
    }
}
