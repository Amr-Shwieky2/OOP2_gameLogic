// MagneticState.cpp - Complete fix for refactored PlayerEntity
#include "MagneticState.h"
#include "NormalState.h"
#include "PlayerEntity.h"
#include "PlayerStateManager.h"
#include "PlayerVisualEffects.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "MovementComponent.h"
#include "CoinEntity.h"
#include "GameSession.h"
#include "Constants.h"
#include <cmath>
#include <iostream>
#include <set>

extern GameSession* g_currentSession;

std::unique_ptr<MagneticState> MagneticState::s_instance = nullptr;

PlayerState* MagneticState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<MagneticState>(new MagneticState());
    }
    return s_instance.get();
}

void MagneticState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Magnetic state" << std::endl;
    m_duration = 6.0f;
    m_attractedCoins.clear();

    // Use the visual effects system to change appearance
    if (auto* visualEffects = player.getVisualEffects()) {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            // Now we can access textures through PlayerEntity
            render->setTexture(player.getTextures().getResource("MagneticBall.png"));
        }

        // Set magnetic visual effect color
        visualEffects->setStateColor(sf::Color(255, 200, 150));
    }
}

void MagneticState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Magnetic state" << std::endl;

    // Reset visual effects
    if (auto* visualEffects = player.getVisualEffects()) {
        visualEffects->setStateColor(sf::Color::White);
    }

    // Reset all attracted coins
    if (g_currentSession) {
        for (auto* entity : g_currentSession->getEntityManager().getAllEntities()) {
            if (auto* coin = dynamic_cast<CoinEntity*>(entity)) {
                auto* coinRender = coin->getComponent<RenderComponent>();
                if (coinRender) {
                    coinRender->getSprite().setColor(sf::Color::White);
                }

                if (m_attractedCoins.find(coin) != m_attractedCoins.end()) {
                    auto* transform = coin->getComponent<Transform>();
                    if (transform) {
                        sf::Vector2f currentPos = transform->getPosition();
                        coin->setupCircularMotion(currentPos);
                    }
                }
            }
        }
    }
    m_attractedCoins.clear();
}

void MagneticState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;
    m_attractTimer += dt;

    // Update attraction every 0.1 seconds
    if (m_attractTimer >= 0.1f) {
        m_attractTimer = 0.0f;

        auto* playerTransform = player.getComponent<Transform>();
        if (!playerTransform) return;

        sf::Vector2f playerPos = playerTransform->getPosition();
        std::set<CoinEntity*> currentlyAttracted;

        if (g_currentSession) {
            for (auto* entity : g_currentSession->getEntityManager().getAllEntities()) {
                if (auto* coin = dynamic_cast<CoinEntity*>(entity)) {
                    if (coin->isActive()) {
                        auto* coinTransform = coin->getComponent<Transform>();
                        auto* coinPhysics = coin->getComponent<PhysicsComponent>();
                        auto* coinMovement = coin->getComponent<MovementComponent>();

                        if (coinTransform && coinPhysics) {
                            sf::Vector2f coinPos = coinTransform->getPosition();
                            sf::Vector2f diff = playerPos - coinPos;
                            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

                            // Attract coins within range
                            if (distance < 300.0f && distance > 25.0f) {
                                currentlyAttracted.insert(coin);

                                // Disable circular motion for attracted coins
                                if (m_attractedCoins.find(coin) == m_attractedCoins.end() && coinMovement) {
                                    *coinMovement = MovementComponent(MovementComponent::MovementType::Static);
                                    std::cout << "[Magnetic] Disabling circular motion for coin" << std::endl;
                                }

                                // Apply attraction force
                                sf::Vector2f direction = diff / distance;
                                float speed = 120.0f * (1.0f - distance / 300.0f);

                                coinPhysics->setVelocity(
                                    direction.x * speed,
                                    direction.y * speed
                                );

                                // Visual feedback for attracted coins
                                auto* coinRender = coin->getComponent<RenderComponent>();
                                if (coinRender) {
                                    if (static_cast<int>(m_duration * 12) % 2 == 0) {
                                        coinRender->getSprite().setColor(sf::Color(255, 215, 0)); // Gold
                                    }
                                    else {
                                        coinRender->getSprite().setColor(sf::Color(255, 255, 150)); // Light yellow
                                    }
                                }
                            }
                            else {
                                // Restore normal behavior for coins out of range
                                if (m_attractedCoins.find(coin) != m_attractedCoins.end()) {
                                    auto* transform = coin->getComponent<Transform>();
                                    if (transform) {
                                        sf::Vector2f currentPos = transform->getPosition();
                                        coin->setupCircularMotion(currentPos);
                                        std::cout << "[Magnetic] Re-enabling circular motion for coin" << std::endl;
                                    }

                                    auto* coinRender = coin->getComponent<RenderComponent>();
                                    if (coinRender) {
                                        coinRender->getSprite().setColor(sf::Color::White);
                                    }
                                }

                                // Stop far coins
                                if (distance > 350.0f) {
                                    coinPhysics->setVelocity(0, 0);
                                }
                            }
                        }
                    }
                }
            }
        }
        m_attractedCoins = currentlyAttracted;
    }

    // Update player visual effect
    if (auto* visualEffects = player.getVisualEffects()) {
        float intensity = 0.8f + 0.2f * std::sin(m_duration * 8.0f);
        sf::Uint8 blue = static_cast<sf::Uint8>(150 * intensity);
        sf::Uint8 green = static_cast<sf::Uint8>(200 * intensity);
        visualEffects->setStateColor(sf::Color(255, green, blue));
    }

    // FIXED: Return to normal state when duration expires
    if (m_duration <= 0) {
        if (auto* stateManager = player.getStateManager()) {
            stateManager->changeState(NormalState::getInstance());
        }
    }
}

void MagneticState::handleInput(PlayerEntity& player, const InputService& input) {
    // Delegate movement to normal state behavior
    NormalState::getInstance()->handleInput(player, input);
}