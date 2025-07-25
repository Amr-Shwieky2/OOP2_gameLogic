#include "NormalState.h"
#include "PlayerEntity.h"
#include "PlayerVisualEffects.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "Constants.h"
#include <iostream>

std::unique_ptr<NormalState> NormalState::s_instance = nullptr;
//-------------------------------------------------------------------------------------
PlayerState* NormalState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<NormalState>(new NormalState());
    }
    return s_instance.get();
}
//-------------------------------------------------------------------------------------
void NormalState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Normal state" << std::endl;

    // Use visual effects system to set texture
    if (auto* visualEffects = player.getVisualEffects()) {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            render->setTexture(player.getTextures().getResource("NormalBall.png"));
            visualEffects->setStateColor(sf::Color::White);
        }
    }

    // Set health component to not invulnerable
    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(false);
    }
}
//-------------------------------------------------------------------------------------
void NormalState::exit(PlayerEntity&) {
}
//-------------------------------------------------------------------------------------
void NormalState::update(PlayerEntity& , float ) {
}
//-------------------------------------------------------------------------------------
void NormalState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED;
    auto vel = physics->getVelocity();

    // Handle movement
    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    // Handle jumping
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }
}
//-------------------------------------------------------------------------------------