#include "CoinEntity.h"
#include "Transform.h"
#include "MovementComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include <random>

//-------------------------------------------------------------------------------------
CoinEntity::CoinEntity(IdType id)
    : CollectibleEntity(id)
{
    addComponent<Transform>();

    auto* movement = addComponent<MovementComponent>(MovementComponent::MovementType::Circular);

    static float s_radius = []() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> radiusDist(30.0f, 40.0f);
        return radiusDist(gen);
        }();

    static float s_speed = []() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> speedDist(1.0f, 3.0f);
        return speedDist(gen);
        }();

    m_circleRadius = s_radius;
    m_rotationSpeed = s_speed;
}
//-------------------------------------------------------------------------------------
void CoinEntity::setupCircularMotion(const sf::Vector2f& centerPosition) {
    auto* movement = getComponent<MovementComponent>();
    if (movement) {
        movement->setCircularMotion(centerPosition, m_circleRadius, m_rotationSpeed);
    }
}
//-------------------------------------------------------------------------------------
void CoinEntity::onCollect(Entity*) {
    setActive(false);
}
//-------------------------------------------------------------------------------------