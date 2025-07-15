#include "PlayerScoreManager.h"
#include "EventSystem.h"
#include "GameEvents.h"
#include <iostream>

//-------------------------------------------------------------------------------------
PlayerScoreManager::PlayerScoreManager()
    : m_score(0), m_scoreMultiplier(1.0f) {
}
//-------------------------------------------------------------------------------------
void PlayerScoreManager::addScore(int points) {
    if (points <= 0) return;

    int adjustedPoints = static_cast<int>(points * m_scoreMultiplier);
    int oldScore = m_score;
    m_score += adjustedPoints;

    publishScoreEvent(adjustedPoints);
}
//-------------------------------------------------------------------------------------
void PlayerScoreManager::publishScoreEvent(int delta) {
    EventSystem::getInstance().publish(
        ScoreChangedEvent(m_score, delta)
    );
}
//-------------------------------------------------------------------------------------