// AIComponent.cpp
#include "AIComponent.h"
#include "AIStrategy.h"
#include "Entity.h"

AIComponent::AIComponent(std::unique_ptr<AIStrategy> strategy)
    : m_strategy(std::move(strategy)) {
}

void AIComponent::update(float dt) {
    if (m_strategy && m_owner && m_targetPlayer) {
        m_strategy->update(*m_owner, dt, m_targetPlayer);
    }
}

void AIComponent::setStrategy(std::unique_ptr<AIStrategy> strategy) {
    m_strategy = std::move(strategy);
}