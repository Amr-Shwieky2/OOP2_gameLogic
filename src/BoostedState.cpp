#include "BoostedState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PlayerStateManager.h"
#include "PlayerVisualEffects.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Constants.h"
#include <iostream>

std::unique_ptr<BoostedState> BoostedState::s_instance = nullptr;

PlayerState* BoostedState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<BoostedState>(new BoostedState());
    }
    return s_instance.get();
}

void BoostedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Boosted state" << std::endl;
    m_duration = 8.0f;

    // Use visual effects system
    if (auto* visualEffects = player.getVisualEffects()) {
        visualEffects->setStateColor(sf::Color(255, 255, 200));
    }
}

void BoostedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Boosted state" << std::endl;

    // Reset visual effects
    if (auto* visualEffects = player.getVisualEffects()) {
        visualEffects->setStateColor(sf::Color::White);
    }
}

void BoostedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Visual pulsing effect
    if (static_cast<int>(m_duration * 10) % 2 == 0) {
        // Could spawn speed particles here
    }

    // Return to normal state when duration expires
    if (m_duration <= 0) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->changeState(NormalState::getInstance());
        }
    }
}

void BoostedState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    // Boosted movement speed
    float moveSpeed = PLAYER_MOVE_SPEED * 1.5f;
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

    // Enhanced jumping
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 1.2f);
    }
}
