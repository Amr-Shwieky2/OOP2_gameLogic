#include "WellEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include "ResourcePaths.h"
#include <iostream>
#include <cmath>

WellEntity::WellEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, textures);
}

void WellEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Hazard);

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE, TILE_SIZE);
    physics->setPosition(centerX, centerY);

    if (auto* body = physics->getBody()) {
        body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    }

    auto* render = addComponent<RenderComponent>();
    try {
        render->setTexture(textures.getResource("well.png"));
    }
    catch (...) {
        render->setTexture(textures.getResource("ground.png"));
    }

    auto& sprite = render->getSprite();
    sprite.setScale(1.0f, 1.0f);

    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(centerX, centerY);

    std::cout << "[DEBUG] WellEntity fixture created: " << (physics->getBody()->GetFixtureList() != nullptr) << std::endl;
}

void WellEntity::onPlayerEnter() {
    if (m_activated) return;

    std::cout << "[Well] Player entered the well! Requesting level change..." << std::endl;

    m_activated = true;

    std::string targetLevel = getTargetLevel();
    if (targetLevel.empty()) {
        targetLevel = ResourcePaths::DARK_LEVEL;
    }

    requestLevelChange(targetLevel);

    auto* render = getComponent<RenderComponent>();
    if (render) {
        render->getSprite().setColor(sf::Color(120, 120, 200));
    }
}

void WellEntity::updateAnimation(float dt) {
    m_animationTimer += dt;

    if (m_bubbleEffect) {
        auto* render = getComponent<RenderComponent>();
        if (render) {
            float pulse = 0.8f + 0.2f * std::sin(m_animationTimer * 8.0f);
            sf::Color color = render->getSprite().getColor();
            color.a = static_cast<sf::Uint8>(255 * pulse);
            render->getSprite().setColor(color);
        }
    }
}

bool WellEntity::s_levelChangeRequested = false;
std::string WellEntity::s_targetLevelName = "";

void WellEntity::requestLevelChange(const std::string& levelName) {
    s_levelChangeRequested = true;
    s_targetLevelName = levelName;
}

void WellEntity::clearLevelChangeRequest() {
    s_levelChangeRequested = false;
    s_targetLevelName.clear();
}