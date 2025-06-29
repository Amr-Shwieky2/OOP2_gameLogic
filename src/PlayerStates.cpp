// PlayerStates.cpp - All state implementations
#include "NormalState.h"
#include "ShieldedState.h"
#include "BoostedState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "Constants.h"
#include <iostream>

// ========== NormalState Implementation ==========
std::unique_ptr<NormalState> NormalState::s_instance = nullptr;

PlayerState* NormalState::getInstance() {
    if (!s_instance) {
        s_instance = std::make_unique<NormalState>();
    }
    return s_instance.get();
}

void NormalState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Normal state" << std::endl;

    // Reset to normal texture
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->setTexture(player.getTextures().getResource("NormalBall.png"));
    }

    // Ensure normal health settings
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(false);
    }
}

void NormalState::exit(PlayerEntity& player) {
    // Nothing special to do when leaving normal state
}

void NormalState::update(PlayerEntity& player, float dt) {
    // Normal state has no special update logic
}

void NormalState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED;
    auto vel = physics->getVelocity();

    // Normal movement
    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    // Jump
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }

    // Shoot
    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}

// ========== ShieldedState Implementation ==========
std::unique_ptr<ShieldedState> ShieldedState::s_instance = nullptr;

PlayerState* ShieldedState::getInstance() {
    if (!s_instance) {
        s_instance = std::make_unique<ShieldedState>();
    }
    return s_instance.get();
}

void ShieldedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Shielded state" << std::endl;
    m_duration = 7.0f; // Shield lasts 7 seconds

    // Change to shield texture
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->setTexture(player.getTextures().getResource("TransparentBall.png"));
        render->getSprite().setColor(sf::Color(255, 255, 255, 200)); // Slightly transparent
    }

    // Make invulnerable
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(true);
    }
}

void ShieldedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Shielded state" << std::endl;

    // Remove invulnerability
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(false);
    }

    // Reset sprite color
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void ShieldedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Flash effect when shield is about to expire
    if (m_duration < 2.0f) {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            int alpha = (static_cast<int>(m_duration * 10) % 2 == 0) ? 255 : 150;
            render->getSprite().setColor(sf::Color(255, 255, 255, alpha));
        }
    }

    // Return to normal state when shield expires
    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void ShieldedState::handleInput(PlayerEntity& player, const InputService& input) {
    // Same input handling as normal state
    NormalState::getInstance()->handleInput(player, input);
}

// ========== BoostedState Implementation ==========
std::unique_ptr<BoostedState> BoostedState::s_instance = nullptr;

PlayerState* BoostedState::getInstance() {
    if (!s_instance) {
        s_instance = std::make_unique<BoostedState>();
    }
    return s_instance.get();
}

void BoostedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Boosted state" << std::endl;
    m_duration = 8.0f; // Speed boost lasts 8 seconds

    // Add visual effect - slight yellow tint
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(255, 255, 200)); // Yellowish tint
    }
}

void BoostedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Boosted state" << std::endl;

    // Reset color
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void BoostedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Add particle trail effect (visual feedback)
    if (static_cast<int>(m_duration * 10) % 2 == 0) {
        // Could spawn speed particles here
    }

    // Return to normal state when boost expires
    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void BoostedState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED * 1.5f; // 50% faster
    auto vel = physics->getVelocity();

    // Boosted movement
    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    // Higher jump when boosted
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 1.2f);
    }

    // Shoot
    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}