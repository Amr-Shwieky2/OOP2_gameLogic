#include "PlayerVisualEffects.h"
#include "PlayerEntity.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"
#include "Transform.h"
#include "Constants.h"
#include <cmath>

PlayerVisualEffects::PlayerVisualEffects(PlayerEntity& player)
    : m_player(player), m_damageTimer(0.0f), m_damageCooldown(1.0f) {
}

void PlayerVisualEffects::update(float dt) {
    updateDamageEffect(dt);
    updateVisualSync();
    applyRollRotation(dt);
}

void PlayerVisualEffects::updateDamageEffect(float dt) {
    if (m_damageTimer > 0.0f) {
        m_damageTimer -= dt;

        auto* render = m_player.getComponent<RenderComponent>();
        if (render) {
            // Flicker effect during damage immunity
            int flicker = static_cast<int>(m_damageTimer * 10) % 2;
            sf::Uint8 alpha = flicker ? 150 : 255;
            render->getSprite().setColor(sf::Color(255, 255, 255, alpha));
        }
    }
    else {
        // Reset color when damage effect ends
        auto* render = m_player.getComponent<RenderComponent>();
        if (render) {
            sf::Color currentColor = render->getSprite().getColor();
            if (currentColor.a != 255) {
                render->getSprite().setColor(sf::Color::White);
            }
        }
    }
}

void PlayerVisualEffects::updateVisualSync() {
    // Sync sprite position with physics body
    auto* render = m_player.getComponent<RenderComponent>();
    auto* transform = m_player.getComponent<Transform>();

    if (render && transform) {
        render->getSprite().setPosition(transform->getPosition());
    }
}

void PlayerVisualEffects::applyRollRotation(float dt) {
    auto* render = m_player.getComponent<RenderComponent>();
    auto* physics = m_player.getComponent<PhysicsComponent>();

    if (render && physics) {
        float vx = physics->getVelocity().x;
        float deltaAngle = vx * dt * PLAYER_SPIN_RATE;
        render->getSprite().rotate(deltaAngle);
    }
}

void PlayerVisualEffects::startDamageEffect() {
    m_damageTimer = m_damageCooldown;

    auto* render = m_player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(255, 150, 150));
    }
}

void PlayerVisualEffects::setStateColor(const sf::Color& color) {
    auto* render = m_player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(color);
    }
}

void PlayerVisualEffects::resetVisuals() {
    auto* render = m_player.getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color::White);
        render->getSprite().setRotation(0.0f);
    }
    m_damageTimer = 0.0f;
}

void PlayerVisualEffects::startDamageCooldown() {
    m_damageTimer = m_damageCooldown;
}
