// DarkLevelSystem.cpp
#include "DarkLevelSystem.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "Constants.h"
#include <iostream>
#include <cmath>
#include <algorithm>

DarkLevelSystem::DarkLevelSystem() {
    // Initialize light circle for player
    m_lightCircle.setRadius(m_playerLightRadius);
    m_lightCircle.setOrigin(m_playerLightRadius, m_playerLightRadius);

    // Create gradient effect for smoother light falloff
    m_lightCircle.setFillColor(sf::Color::Transparent);
    m_lightCircle.setOutlineThickness(0);
}

DarkLevelSystem::~DarkLevelSystem() = default;

void DarkLevelSystem::initialize(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    // Create render textures for lighting effects
    m_lightTexture = std::make_unique<sf::RenderTexture>();
    m_darknessTexture = std::make_unique<sf::RenderTexture>();

    if (!m_lightTexture->create(windowSize.x, windowSize.y) ||
        !m_darknessTexture->create(windowSize.x, windowSize.y)) {
        std::cerr << "[DarkLevelSystem] Failed to create render textures" << std::endl;
        return;
    }

    // Setup darkness overlay
    m_darknessOverlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * m_darknessLevel)));

    std::cout << "[DarkLevelSystem] Initialized with darkness level: " << m_darknessLevel << std::endl;
}

void DarkLevelSystem::update(float dt, PlayerEntity* player) {
    if (!m_enabled) return;

    m_flickerTimer += dt;
    m_ambientTimer += dt;

    // Update player light position
    updatePlayerLight(player);

    // Add subtle flickering effect to player light
    float flicker = 1.0f + 0.1f * std::sin(m_flickerTimer * 12.0f);
    m_playerLightRadius = 150.0f * flicker;
    m_lightCircle.setRadius(m_playerLightRadius);
    m_lightCircle.setOrigin(m_playerLightRadius, m_playerLightRadius);

    // Update light sources (if any have animations)
    for (auto& light : m_lightSources) {
        // Add flickering to light sources
        light.intensity = 0.8f + 0.2f * std::sin(m_ambientTimer * 8.0f + light.position.x * 0.01f);
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
    if (!m_enabled || !m_lightTexture || !m_darknessTexture) return;

    sf::View currentView = window.getView();

    // Clear light texture with black (no light)
    m_lightTexture->clear(sf::Color::Black);
    m_lightTexture->setView(currentView);

    // Render player light
    sf::CircleShape playerLight(m_playerLightRadius);
    playerLight.setOrigin(m_playerLightRadius, m_playerLightRadius);
    playerLight.setPosition(m_playerLightPos);

    // Create radial gradient effect for smoother lighting
    sf::Color lightColor = sf::Color::White;
    lightColor.a = 180; // Semi-transparent for blending
    playerLight.setFillColor(lightColor);

    // Use additive blending for light accumulation
    sf::BlendMode additiveBlend(sf::BlendMode::One, sf::BlendMode::One);
    m_lightTexture->draw(playerLight, additiveBlend);

    // Render additional light sources
    renderLightSources(*m_lightTexture);

    m_lightTexture->display();

    // Create darkness texture
    m_darknessTexture->clear(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * m_darknessLevel)));
    m_darknessTexture->setView(currentView);

    // Subtract light from darkness (multiply blend mode)
    sf::Sprite lightSprite(m_lightTexture->getTexture());
    sf::BlendMode subtractBlend(sf::BlendMode::Zero, sf::BlendMode::SrcColor);
    m_darknessTexture->draw(lightSprite, subtractBlend);

    m_darknessTexture->display();

    // Draw final darkness overlay
    sf::Sprite darknessSprite(m_darknessTexture->getTexture());
    window.draw(darknessSprite);
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

    // Update darkness overlay
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