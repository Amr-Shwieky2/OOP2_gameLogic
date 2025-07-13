#include "AIComponent.h"
#include "AIStrategy.h"
#include "Entity.h"
#include <iostream>

AIComponent::AIComponent(std::unique_ptr<AIStrategy> strategy)
    : m_strategy(std::move(strategy)) {
    std::cout << "[AI COMPONENT] Created with strategy: "
        << (m_strategy ? m_strategy->getName() : "null") << std::endl;
}

void AIComponent::update(float dt) {
    if (!m_owner) {
        std::cout << "[AI COMPONENT] No owner!" << std::endl;
        return;
    }

    if (!m_strategy) {
        std::cout << "[AI COMPONENT] No strategy!" << std::endl;
        return;
    }

    if (m_strategy->requiresPlayer()) {
        if (!m_targetPlayer) {
            std::cout << "[AI COMPONENT] No target player!" << std::endl;
            return;
        }
        m_strategy->update(*m_owner, dt, m_targetPlayer);
    }
    else {
        m_strategy->update(*m_owner, dt, nullptr);
    }
}

void AIComponent::setStrategy(std::unique_ptr<AIStrategy> strategy) {
    std::cout << "[AI COMPONENT] Strategy changed from "
        << (m_strategy ? m_strategy->getName() : "null")
        << " to "
        << (strategy ? strategy->getName() : "null") << std::endl;
    m_strategy = std::move(strategy);
}
