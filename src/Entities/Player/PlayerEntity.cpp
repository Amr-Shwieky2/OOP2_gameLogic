#include "PlayerEntity.h"
#include "PlayerStateManager.h"
#include "PlayerInputHandler.h"
#include "PlayerScoreManager.h"
#include "PlayerVisualEffects.h"
#include "PlayerWeaponSystem.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
#include "NormalState.h"

PlayerEntity::PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id), m_textures(textures), m_world(world) {

    setupComponents(world, x, y, textures);

    // Create subsystems after components are set up
    m_stateManager = std::make_unique<PlayerStateManager>(*this);
    m_inputHandler = std::make_unique<PlayerInputHandler>(*this);
    m_scoreManager = std::make_unique<PlayerScoreManager>();
    m_visualEffects = std::make_unique<PlayerVisualEffects>(*this);
    m_weaponSystem = std::make_unique<PlayerWeaponSystem>(*this, world, textures);

    // Initialize to normal state
    m_stateManager->changeState(NormalState::getInstance());
}

void PlayerEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    // Add transform
    addComponent<Transform>(sf::Vector2f(x, y));

    // Add physics
    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createCircleShape(PLAYER_RADIUS * PPM);
    physics->setPosition(x, y);

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("NormalBall.png"));
    auto& sprite = render->getSprite();

    // Scale to desired size
    float desiredDiameter = PLAYER_RADIUS * 2 * PPM;
    sf::Vector2u textureSize = sprite.getTexture()->getSize();
    float scaleX = desiredDiameter / textureSize.x;
    float scaleY = desiredDiameter / textureSize.y;
    sprite.setScale(scaleX, scaleY);
    sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);

    // Add health
    addComponent<HealthComponent>(3); // 3 lives

    // Add collision
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Player);
}

void PlayerEntity::update(float dt) {
    // Update base entity components
    Entity::update(dt);

    // Update all subsystems
    if (m_stateManager) {
        m_stateManager->update(dt);
    }

    if (m_visualEffects) {
        m_visualEffects->update(dt);
    }

    if (m_weaponSystem) {
        m_weaponSystem->update(dt);
    }
}

void PlayerEntity::handleInput(const InputService& input) {
    if (m_inputHandler) {
        m_inputHandler->handleInput(input);
    }
}

void PlayerEntity::addScore(int points) {
    if (m_scoreManager) {
        m_scoreManager->addScore(points);
    }
}

int PlayerEntity::getScore() const {
    return m_scoreManager ? m_scoreManager->getScore() : 0;
}

sf::Vector2f PlayerEntity::getPosition() const {
    auto* transform = getComponent<Transform>();
    return transform ? transform->getPosition() : sf::Vector2f(0, 0);
}

sf::Vector2f PlayerEntity::getVelocity() const {
    auto* physics = getComponent<PhysicsComponent>();
    return physics ? physics->getVelocity() : sf::Vector2f(0, 0);
}

bool PlayerEntity::isOnGround() const {
    auto* physics = getComponent<PhysicsComponent>();
    if (!physics) return false;

    // Simple ground detection - check if vertical velocity is near zero
    sf::Vector2f vel = physics->getVelocity();
    return std::abs(vel.y) < 0.1f;
}

sf::Keyboard::Key PlayerEntity::getJumpKey() const {
    // Check if we're in reversed state to return appropriate jump key
    if (m_stateManager && m_stateManager->getCurrentState()) {
        const char* stateName = m_stateManager->getCurrentState()->getName();
        if (std::string(stateName) == "Reversed") {
            return sf::Keyboard::Down;  // In reversed state, down key jumps
        }
    }
    return sf::Keyboard::Up;  // Normal jump key
}

PlayerState* PlayerEntity::getCurrentState() const {
    return m_stateManager ? m_stateManager->getCurrentState() : nullptr;
}