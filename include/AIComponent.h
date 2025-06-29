// AIComponent.h
#pragma once
#include "Component.h"
#include <memory>

class AIStrategy;
class PlayerEntity;

/**
 * AIComponent - Manages AI behavior using Strategy Pattern
 */
class AIComponent : public Component {
public:
    AIComponent(std::unique_ptr<AIStrategy> strategy);

    void update(float dt) override;

    // Strategy management
    void setStrategy(std::unique_ptr<AIStrategy> strategy);
    AIStrategy* getStrategy() const { return m_strategy.get(); }

    // Set target player
    void setTarget(PlayerEntity* player) { m_targetPlayer = player; }

private:
    std::unique_ptr<AIStrategy> m_strategy;
    PlayerEntity* m_targetPlayer = nullptr;
};