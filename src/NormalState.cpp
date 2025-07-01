#include "NormalState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "Constants.h"
#include <iostream>

std::unique_ptr<NormalState> NormalState::s_instance = nullptr;

PlayerState* NormalState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<NormalState>(new NormalState());
    }
    return s_instance.get();
}

void NormalState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Normal state" << std::endl;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->setTexture(player.getTextures().getResource("NormalBall.png"));
    }

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

    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(-moveSpeed, vel.y);
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(moveSpeed, vel.y);
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }

    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}
