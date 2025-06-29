// SmartEnemyEntity.h
#pragma once
#include "EnemyEntity.h"

/**
 * SmartEnemyEntity - Enemy that changes strategy based on player distance
 */
class SmartEnemyEntity : public EnemyEntity {
public:
    SmartEnemyEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void update(float dt) override;

protected:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures) override;

private:
    void updateStrategy();

    float m_strategyTimer = 0.0f;
    enum class CurrentStrategy { Patrol, Follow, Guard };
    CurrentStrategy m_currentStrategy = CurrentStrategy::Patrol;
};