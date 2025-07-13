#pragma once
#include "EventSystem.h"

/**
 * PlayerScoreManager - Single Responsibility: Manage player score
 */
class PlayerScoreManager {
public:
    PlayerScoreManager();

    void addScore(int points);
    int getScore() const { return m_score; }
    void resetScore() { m_score = 0; }
    void setMultiplier(float multiplier) { m_scoreMultiplier = multiplier; }

private:
    int m_score = 0;
    float m_scoreMultiplier = 1.0f;

    void publishScoreEvent(int delta);
};