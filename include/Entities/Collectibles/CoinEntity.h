#pragma once
#include "CollectibleEntity.h"
#include "Transform.h"
#include <SFML/System/Vector2.hpp>

class CoinEntity : public CollectibleEntity {
public:
    explicit CoinEntity(IdType id);

    void onCollect(Entity* collector) override;

    // Setup circular motion after position is set
    void setupCircularMotion(const sf::Vector2f& centerPosition);

private:
    float m_circleRadius = 50.0f;    
    float m_rotationSpeed = 2.0f;    
};