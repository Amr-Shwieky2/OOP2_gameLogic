#include "BoostedState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
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

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(255, 255, 200));
    }
}

void BoostedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Boosted state" << std::endl;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void BoostedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    if (static_cast<int>(m_duration * 10) % 2 == 0) {
        // Could spawn speed particles here
    }

    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void BoostedState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

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

    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 1.2f);
    }

    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}
