#include "DarkLevelSystem.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <PhysicsComponent.h>

// تعريف M_PI إذا لم يكن معرفاً
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DarkLevelSystem::DarkLevelSystem() {
    m_lightCircle.setRadius(m_playerLightRadius);
    m_lightCircle.setOrigin(m_playerLightRadius, m_playerLightRadius);
    m_lightCircle.setFillColor(sf::Color::Transparent);
    m_lightCircle.setOutlineThickness(0);
}

DarkLevelSystem::~DarkLevelSystem() = default;

void DarkLevelSystem::initialize(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    m_lightTexture = std::make_unique<sf::RenderTexture>();
    m_darknessTexture = std::make_unique<sf::RenderTexture>();
    m_flashlightTexture = std::make_unique<sf::RenderTexture>();
    m_maskTexture = std::make_unique<sf::RenderTexture>();

    if (!m_lightTexture->create(windowSize.x, windowSize.y) ||
        !m_darknessTexture->create(windowSize.x, windowSize.y) ||
        !m_flashlightTexture->create(windowSize.x, windowSize.y) ||
        !m_maskTexture->create(windowSize.x, windowSize.y)) {
        std::cerr << "[DarkLevelSystem] Failed to create render textures" << std::endl;
        return;
    }

    m_flashlightRange = 300.0f;
    m_flashlightAngle = 45.0f;
    m_flashlightIntensity = 0.9f;
    m_flashlightOn = true;

    m_batteryLevel = 1.0f;
    m_batteryDrainRate = 0.02f;
    m_lowBatteryWarning = false;

    m_darknessLevel = 0.98f;


    m_darknessOverlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, 250));

    std::cout << "[DarkLevelSystem] Realistic flashlight system initialized" << std::endl;
}

void DarkLevelSystem::update(float dt, PlayerEntity* player) {
    if (!m_enabled) return;

    m_flickerTimer += dt;

    updateBattery(dt);
    updatePlayerLight(player);

    if (player) {
        auto* transform = player->getComponent<Transform>();
        if (transform) {
            auto* physics = player->getComponent<PhysicsComponent>();
            if (physics) {
                sf::Vector2f velocity = physics->getVelocity();
                if (std::abs(velocity.x) > 0.1f) {
                    m_flashlightDirection = sf::Vector2f(velocity.x > 0 ? 1.0f : -1.0f, 0.0f);
                }
            }
        }
    }
}

void DarkLevelSystem::updateBattery(float dt) {
    if (!m_flashlightOn) return;

    m_batteryLevel -= m_batteryDrainRate * dt;
    m_batteryLevel = std::max(0.0f, m_batteryLevel);

    if (m_batteryLevel < 0.2f && !m_lowBatteryWarning) {
        m_lowBatteryWarning = true;
        std::cout << "[Flashlight] LOW BATTERY WARNING!" << std::endl;
    }

    if (m_batteryLevel <= 0.0f) {
        m_flashlightOn = false;
        std::cout << "[Flashlight] Battery depleted - flashlight OFF!" << std::endl;
    }

    if (m_batteryLevel < 0.1f && m_flashlightOn) {
        float flickerChance = (0.1f - m_batteryLevel) * 10.0f;
        if ((rand() % 100) < flickerChance * 30) {
            m_flashlightOn = false;
        }
    }
}

void DarkLevelSystem::updatePlayerLight(PlayerEntity* player) {
    if (!player) return;
    auto* transform = player->getComponent<Transform>();
    if (transform) {
        m_playerLightPos = transform->getPosition();
    }
}

void DarkLevelSystem::render(sf::RenderWindow& window) {
    if (!m_enabled) return;

    sf::View currentView = window.getView();

    window.draw(m_darknessOverlay);

    if (m_flashlightOn && m_batteryLevel > 0.0f) {
        renderFlashlight(window);
    }

    // ✅ استخدام الدالة الصحيحة لرسم مصادر الإضاءة
    renderLightSources(*m_flashlightTexture);
}

void DarkLevelSystem::renderFlashlight(sf::RenderWindow& window) {
    m_flashlightTexture->clear(sf::Color::Transparent);
    m_flashlightTexture->setView(window.getView());

    float currentIntensity = m_flashlightIntensity * m_batteryLevel;
    float flicker = 1.0f;
    if (m_batteryLevel < 0.3f) {
        flicker = 0.7f + 0.3f * std::sin(m_flickerTimer * 20.0f);
    }
    currentIntensity *= flicker;

    drawFlashlightCone(currentIntensity);
    m_flashlightTexture->display();

    sf::Sprite flashlightSprite(m_flashlightTexture->getTexture());

    // ✅ تصحيح استخدام Blend Mode
    window.draw(flashlightSprite, sf::RenderStates(sf::BlendAdd));
}

void DarkLevelSystem::drawFlashlightCone(float intensity) {
    if (m_playerLightPos.x == 0 && m_playerLightPos.y == 0) return;

    sf::VertexArray flashlightCone(sf::TriangleFan);
    sf::Vector2f center = m_playerLightPos;
    flashlightCone.append(sf::Vertex(center, sf::Color(255, 255, 200, static_cast<sf::Uint8>(255 * intensity))));

    float angleRad = m_flashlightAngle * static_cast<float>(M_PI) / 180.0f;
    float directionAngle = std::atan2(m_flashlightDirection.y, m_flashlightDirection.x);

    int segments = 20;
    for (int i = 0; i <= segments; ++i) {
        float segmentAngle = directionAngle - angleRad / 2 + (angleRad * i / segments);
        sf::Vector2f point = center + sf::Vector2f(
            std::cos(segmentAngle) * m_flashlightRange,
            std::sin(segmentAngle) * m_flashlightRange
        );
        sf::Uint8 alpha = static_cast<sf::Uint8>(100 * intensity * (1.0f - static_cast<float>(i) / segments));
        flashlightCone.append(sf::Vertex(point, sf::Color(255, 255, 180, alpha)));
    }

    m_flashlightTexture->draw(flashlightCone);

    sf::CircleShape halo(50.0f);
    halo.setOrigin(50.0f, 50.0f);
    halo.setPosition(center);
    halo.setFillColor(sf::Color(255, 255, 200, static_cast<sf::Uint8>(50 * intensity)));
    m_flashlightTexture->draw(halo);
}

void DarkLevelSystem::toggleFlashlight() {
    if (m_batteryLevel > 0.0f) {
        m_flashlightOn = !m_flashlightOn;
        std::cout << "[Flashlight] " << (m_flashlightOn ? "ON" : "OFF") << std::endl;
    }
    else {
        std::cout << "[Flashlight] Cannot turn on - battery depleted!" << std::endl;
    }
}

void DarkLevelSystem::updateFlashlightDirection(const sf::Vector2f& playerPos, const sf::Vector2f& targetPos) {
    sf::Vector2f direction = targetPos - playerPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        m_flashlightDirection = direction / length;
    }
}

void DarkLevelSystem::renderLightSources(sf::RenderTexture& target) {
    sf::BlendMode additiveBlend(sf::BlendMode::One, sf::BlendMode::One);
    for (const auto& light : m_lightSources) {
        sf::CircleShape lightShape(light.radius);
        lightShape.setOrigin(light.radius, light.radius);
        lightShape.setPosition(light.position);

        sf::Color lightColor = light.color;
        lightColor.a = static_cast<sf::Uint8>(255 * light.intensity * 0.7f);
        lightShape.setFillColor(lightColor);

        target.draw(lightShape, additiveBlend);
    }
}

void DarkLevelSystem::setDarknessLevel(float level) {
    m_darknessLevel = std::clamp(level, 0.0f, 1.0f);
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * m_darknessLevel);
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, alpha));
    std::cout << "[DarkLevelSystem] Darkness level set to: " << m_darknessLevel << std::endl;
}

void DarkLevelSystem::addLightSource(const sf::Vector2f& position, float radius, sf::Color color) {
    LightSource light;
    light.position = position;
    light.radius = radius;
    light.color = color;
    light.intensity = 1.0f;
    m_lightSources.push_back(light);

    std::cout << "[DarkLevelSystem] Added light source at (" << position.x << ", " << position.y
        << ") with radius " << radius << std::endl;
}

void DarkLevelSystem::clearLightSources() {
    m_lightSources.clear();
    std::cout << "[DarkLevelSystem] Cleared all light sources" << std::endl;
}
