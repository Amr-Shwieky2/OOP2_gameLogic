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
#include <MagneticState.h>
#include <Transform.h>
#include <CoinEntity.h>
#include <GameSession.h>
#include <HeadwindState.h>
#include <ReversedState.h>

// ========== NormalState Implementation ==========
std::unique_ptr<NormalState> NormalState::s_instance = nullptr;

PlayerState* NormalState::getInstance() {
    if (!s_instance) {
        // Can't use make_unique with private constructor
        s_instance = std::unique_ptr<NormalState>(new NormalState());
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
        // Can't use make_unique with private constructor
        s_instance = std::unique_ptr<ShieldedState>(new ShieldedState());
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
        // Can't use make_unique with private constructor
        s_instance = std::unique_ptr<BoostedState>(new BoostedState());
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

// ========== MagneticState Implementation ==========
std::unique_ptr<MagneticState> MagneticState::s_instance = nullptr;

PlayerState* MagneticState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<MagneticState>(new MagneticState());
    }
    return s_instance.get();
}

void MagneticState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Magnetic state" << std::endl;
    m_duration = 15.0f; // Magnetic effect lasts 15 seconds

    // Add visual effect - purple/orange glow
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(255, 200, 150)); // Orange tint
    }
}

void MagneticState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Magnetic state" << std::endl;

    // Reset color
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void MagneticState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;
    m_attractTimer += dt;

    // Attract nearby coins every 0.1 seconds
    if (m_attractTimer >= 0.1f) {
        m_attractTimer = 0.0f;

        // Get player position
        auto* playerTransform = player.getComponent<Transform>();
        if (!playerTransform) return;

        sf::Vector2f playerPos = playerTransform->getPosition();

        // Check all entities for coins
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

                            // Attract coins within 300 pixel radius
                            if (distance < 300.0f && distance > 10.0f) {
                                // Normalize direction
                                diff /= distance;

                                // Apply force towards player
                                float force = 200.0f * (1.0f - distance / 300.0f); // Stronger when closer
                                coinPhysics->applyForce(diff.x * force, diff.y * force);
                            }
                        }
                    }
                }
            }
        }
    }

    // Add sparkle effect
    if (static_cast<int>(m_duration * 10) % 3 == 0) {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            render->getSprite().setColor(sf::Color(255, 220, 180)); // Brighter orange
        }
    }
    else {
        auto* render = player.getComponent<RenderComponent>();
        if (render) {
            render->getSprite().setColor(sf::Color(255, 200, 150)); // Normal orange
        }
    }

    // Return to normal state when effect expires
    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void MagneticState::handleInput(PlayerEntity& player, const InputService& input) {
    // Same input handling as normal state
    NormalState::getInstance()->handleInput(player, input);
}

// ========== ReversedState Implementation ==========
std::unique_ptr<ReversedState> ReversedState::s_instance = nullptr;

PlayerState* ReversedState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<ReversedState>(new ReversedState());
    }
    return s_instance.get();
}

void ReversedState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Reversed state - Controls inverted!" << std::endl;
    m_duration = 10.0f; // Reversed controls for 10 seconds

    // Add visual effect - dizzy/confused appearance
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(200, 150, 255)); // Purple tint for confusion
    }
}

void ReversedState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Reversed state - Controls normal" << std::endl;

    // Reset color
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }
}

void ReversedState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Add dizzy visual effect
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        // Rotate slightly to show confusion
        float wobble = std::sin(m_duration * 5.0f) * 5.0f;
        render->getSprite().setRotation(wobble);
    }

    // Flash warning when about to expire
    if (m_duration < 2.0f) {
        int flash = static_cast<int>(m_duration * 10) % 2;
        if (render) {
            render->getSprite().setColor(flash ? sf::Color::White : sf::Color(200, 150, 255));
        }
    }

    // Return to normal state when effect expires
    if (m_duration <= 0) {
        if (render) {
            render->getSprite().setRotation(0); // Reset rotation
        }
        player.changeState(NormalState::getInstance());
    }
}

void ReversedState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    float moveSpeed = PLAYER_MOVE_SPEED;
    auto vel = physics->getVelocity();

    // REVERSED CONTROLS!
    if (input.isKeyDown(sf::Keyboard::Left)) {
        physics->setVelocity(moveSpeed, vel.y);  // Go RIGHT when LEFT is pressed
    }
    else if (input.isKeyDown(sf::Keyboard::Right)) {
        physics->setVelocity(-moveSpeed, vel.y); // Go LEFT when RIGHT is pressed
    }
    else {
        physics->setVelocity(0, vel.y);
    }

    // Jump is still normal
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE);
    }

    // Shoot is still normal
    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}

// ========== HeadwindState Implementation ==========
std::unique_ptr<HeadwindState> HeadwindState::s_instance = nullptr;

PlayerState* HeadwindState::getInstance() {
    if (!s_instance) {
        s_instance = std::unique_ptr<HeadwindState>(new HeadwindState());
    }
    return s_instance.get();
}

void HeadwindState::enter(PlayerEntity& player) {
    std::cout << "[State] Entering Headwind state - Movement slowed!" << std::endl;
    m_duration = 12.0f; // Headwind lasts 12 seconds

    // Add visual effect - bluish tint for wind
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(150, 150, 255)); // Light blue for wind
    }

    // Slow down physics
    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(2.0f); // Add drag
    }
}

void HeadwindState::exit(PlayerEntity& player) {
    std::cout << "[State] Exiting Headwind state - Movement normal" << std::endl;

    // Reset color
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
    }

    // Reset physics
    auto* physics = player.getComponent<PhysicsComponent>();
    if (physics && physics->getBody()) {
        physics->getBody()->SetLinearDamping(0.0f); // Remove drag
    }
}

void HeadwindState::update(PlayerEntity& player, float dt) {
    m_duration -= dt;

    // Add wind particle effect (could spawn actual particles here)
    auto* render = player.getComponent<RenderComponent>();
    if (render) {
        // Flicker effect to simulate wind
        int flicker = static_cast<int>(m_duration * 20) % 3;
        sf::Uint8 alpha = 200 + flicker * 20;
        render->getSprite().setColor(sf::Color(150, 150, 255, alpha));
    }

    // Return to normal state when effect expires
    if (m_duration <= 0) {
        player.changeState(NormalState::getInstance());
    }
}

void HeadwindState::handleInput(PlayerEntity& player, const InputService& input) {
    auto* physics = player.getComponent<PhysicsComponent>();
    if (!physics) return;

    // Movement is MUCH slower due to headwind
    float moveSpeed = PLAYER_MOVE_SPEED * 0.3f; // 70% slower!
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

    // Jump is also affected by headwind
    if (input.isKeyPressed(sf::Keyboard::Up) && player.isOnGround()) {
        physics->applyImpulse(0, -PLAYER_JUMP_IMPULSE * 0.7f); // Weaker jump
    }

    // Shoot is normal
    if (input.isKeyPressed(sf::Keyboard::C)) {
        player.shoot();
    }
}