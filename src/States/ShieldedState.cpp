#include "ShieldedState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "Constants.h"
#include <iostream>

std::unique_ptr<ShieldedState> ShieldedState::s_instance = nullptr;

//-------------------------------------------------------------------------------------
PlayerState* ShieldedState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<ShieldedState>(new ShieldedState());
    }
    return s_instance.get();
}
//-------------------------------------------------------------------------------------
void ShieldedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Shielded state" << std::endl;
    m_duration = 7.0f;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->setTexture(player.getTextures().getResource("TransparentBall.png"));
        render->getSprite().setColor(sf::Color(255, 255, 255, 200));
    }

    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(true);
    }
}
//-------------------------------------------------------------------------------------
void ShieldedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Shielded state" << std::endl;

    auto* health = player.getComponent<HealthComponent>();
    if (health) {
        health->setInvulnerable(false);
    }

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}
//-------------------------------------------------------------------------------------
void ShieldedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    if (m_duration <= 0) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->changeState(NormalState::getInstance());
        }
    }
}
//-------------------------------------------------------------------------------------
void ShieldedState::handleInput(PlayerEntity& player, const InputService& input) {
    NormalState::getInstance()->handleInput(player, input);
}
//-------------------------------------------------------------------------------------