#include "AIComponent.h"
#include "AIStrategy.h"
#include "Entity.h"
//-------------------------------------------------------------------------------------
AIComponent::AIComponent(std::unique_ptr<AIStrategy> strategy)
    : m_strategy(std::move(strategy)) {
}
//-------------------------------------------------------------------------------------
void AIComponent::update(float dt) {
    if (!m_owner || !m_strategy) {
        return;
    }

    if (m_strategy->requiresPlayer()) {
        if (!m_targetPlayer) {
            return;
        }
        m_strategy->update(*m_owner, dt, m_targetPlayer);
    }
    else {
        m_strategy->update(*m_owner, dt, nullptr);
    }
}
//-------------------------------------------------------------------------------------
void AIComponent::setStrategy(std::unique_ptr<AIStrategy> strategy) {
    m_strategy = std::move(strategy);
}
//-------------------------------------------------------------------------------------