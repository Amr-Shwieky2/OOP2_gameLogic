// HeadwindState.cpp - Fixed for refactored PlayerEntity
#include "HeadwindState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PlayerStateManager.h"
#include "PlayerVisualEffects.h"
#include "PlayerWeaponSystem.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Constants.h"
#include <iostream>

std::unique_ptr<HeadwindState> HeadwindState::s_instance = nullptr;

PlayerState* HeadwindState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<HeadwindState>(new HeadwindState());
    }
    return s_instance.get();
}

void HeadwindState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Headwind state - Movement slowed!" << std::endl;
    m_duration = 8.0f;

    // Use visual effects system for color change
    if (auto* visualEffects = player.getVisualEffects()) {
        visualEffects->setStateColor(sf::Color(150, 150, 255));
    }

    // Apply physics dampening for headwind effect
    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(2.0f);
    }
}

void HeadwindState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Headwind state - Movement normal" << std::endl;

    // Reset visual effects
    if (auto* visualEffects = player.getVisualEffects()) {
        visualEffects->setStateColor(sf::Color::White);
    }

    // Reset physics dampening
    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(0.0f);
    }
}

void HeadwindState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Visual flickering effect during headwind
    if (auto* visualEffects = player.getVisualEffects()) {
        int flicker = static_cast<int>(m_duration * 20) % 3;
        sf::Uint8 alpha = static_cast<sf::Uint8>(200 + flicker * 20);
        visualEffects->setStateColor(sf::Color(150, 150, 255, alpha));
    }

    // Return to normal state when duration expires
    if (m_duration <= 0) {
        // FIXED: Use StateManager instead of direct changeState()
        if (auto* stateManager = player.getStateManager()) {
            stateManager->changeState(NormalState::getInstance());
        }
    }
}

void HeadwindState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    // Reduced movement speed for headwind effect
    float moveSpeed = PLAYER_MOVE_SPEED * 0.3f;
    auto vel = physics->getVelocity();

    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    // Reduced jumping power for headwind effect
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 0.5f);
    }

    if (input.isKeyPressed(sf::Keyboard::C)) {
        if (auto* weaponSystem = player.getWeaponSystem()) {
            weaponSystem->shoot();
        }
    }
}