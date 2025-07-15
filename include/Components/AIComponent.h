#pragma once
#include "Component.h"
#include <memory>

class AIStrategy;
class PlayerEntity;

/**
 * @brief Component responsible for AI behavior via Strategy Pattern.
 *
 * This component delegates its behavior to a pluggable AIStrategy instance.
 * It is intended to be attached to an Entity (e.g., an enemy), and updates
 * are forwarded to the strategy implementation.
 *
 * If the strategy requires a PlayerEntity as a target (e.g., Follow or Attack),
 * a pointer to the player must be set beforehand.
 */
class AIComponent : public Component {
public:
    /**
     * @brief Constructs the component with a given strategy.
     * @param strategy A unique_ptr to an AI strategy object.
     */
    AIComponent(std::unique_ptr<AIStrategy> strategy);

    /**
     * @brief Updates the strategy logic for this frame.
     * @param dt Delta time in seconds.
     */
    void update(float dt) override;

    /**
     * @brief Replaces the current AI strategy.
     * @param strategy New strategy to adopt.
     */
    void setStrategy(std::unique_ptr<AIStrategy> strategy);

    /**
     * @brief Returns a raw pointer to the current strategy.
     */
    AIStrategy* getStrategy() const { return m_strategy.get(); }

    /**
     * @brief Sets the target player for strategies that require it.
     */
    void setTarget(PlayerEntity* player) { m_targetPlayer = player; }

private:
    std::unique_ptr<AIStrategy> m_strategy;   ///< The active AI behavior.
    PlayerEntity* m_targetPlayer = nullptr;   ///< Player target if required.
};
