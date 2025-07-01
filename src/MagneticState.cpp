#include "MagneticState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "CoinEntity.h"
#include "GameSession.h"
#include "Constants.h"
#include <cmath>
#include <iostream>

std::unique_ptr<MagneticState> MagneticState::s_instance = nullptr;

PlayerState* MagneticState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<MagneticState>(new MagneticState());
    }
    return s_instance.get();
}

void MagneticState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Magnetic state" << std::endl;
    m_duration = 15.0f;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->setTexture(player.getTextures().getResource("MagneticBall.png"));
        render->getSprite().setColor(sf::Color(255, 200, 150));
    }
}

void MagneticState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Magnetic state" << std::endl;

    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void MagneticState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;
    m_attractTimer += dt;

    if (m_attractTimer >= 0.1f) {
        m_attractTimer = 0.0f;

        auto* playerTransform = player.getComponent<Transform>();
        if (!playerTransform) return;

        sf::Vector2f playerPos = playerTransform->getPosition();

        if (g_currentSession) {
            for (auto* entity : g_currentSession->getEntityManager().getAllEntities()) {
                if (auto* coin = dynamic_cast<CoinEntity*>(entity)) {
                    if (coin->isActive()) {
                        auto* coinTransform = coin->getComponent<Transform>();
                        auto* coinPhysics = coin->getComponent<PhysicsComponent>();

                        if (coinTransform && coinPhysics) {
                            sf::Vector2f coinPos = coinTransform->getPosition();
                            sf::Vector2f diff = playerPos - coinPos;
                            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

                            if (distance < 300.0f && distance > 10.0f) {
                                diff /= distance;
                                float force = 200.0f * (1.0f - distance / 300.0f);
                                coinPhysics->applyForce(diff.x * force, diff.y * force);
                            }
                        }
                    }
                }
            }
        }
    }

    if (static_cast<int>(m_duration * 10) % 3 == 0) {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            render->getSprite().setColor(sf::Color(255, 220, 180));
        }
    } else {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            render->getSprite().setColor(sf::Color(255, 200, 150));
        }
    }

    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void MagneticState::handleInput(PlayerEntity& player, const InputService& input) {
    NormalState::getInstance()->handleInput(player, input);
}
