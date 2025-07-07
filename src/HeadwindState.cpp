#include "HeadwindState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
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
    m_duration = 12.0f;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(150, 150, 255));
    }

    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(2.0f);
    }
}

void HeadwindState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Headwind state - Movement normal" << std::endl;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }

    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(0.0f);
    }
}

void HeadwindState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        int flicker = static_cast<int>(m_duration * 20) % 3;
        sf::Uint8 alpha = 200 + flicker * 20;
        render->getSprite().setColor(sf::Color(150, 150, 255, alpha));
    }

    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void HeadwindState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED * 0.1f;
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
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 0.5f);
    }

    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}
