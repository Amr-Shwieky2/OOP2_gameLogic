// GameEvents.h
#pragma once
#include "EventSystem.h"
#include "Entity.h"
#include <string>

/**
 * Score changed event
 */
class ScoreChangedEvent : public Event {
public:
    ScoreChangedEvent(int newScore, int delta)
        : newScore(newScore), delta(delta) {
    }

    const char* getName() const override { return "ScoreChanged"; }

    int newScore;  // New total score
    int delta;     // Change amount
};

/**
 * Player died event
 */
class PlayerDiedEvent : public Event {
public:
    PlayerDiedEvent(Entity::IdType playerId)
        : playerId(playerId) {
    }

    const char* getName() const override { return "PlayerDied"; }

    Entity::IdType playerId;
};

/**
 * Item collected event
 */
class ItemCollectedEvent : public Event {
public:
    enum class ItemType {
        Coin,
        Gift,
        PowerUp
    };

    ItemCollectedEvent(Entity::IdType collectorId, Entity::IdType itemId, ItemType type)
        : collectorId(collectorId), itemId(itemId), type(type) {
    }

    const char* getName() const override { return "ItemCollected"; }

    Entity::IdType collectorId;
    Entity::IdType itemId;
    ItemType type;
};

/**
 * Enemy killed event
 */
class EnemyKilledEvent : public Event {
public:
    EnemyKilledEvent(Entity::IdType enemyId, Entity::IdType killerId)
        : enemyId(enemyId), killerId(killerId) {
    }

    const char* getName() const override { return "EnemyKilled"; }

    Entity::IdType enemyId;
    Entity::IdType killerId;
};

/**
 * Level completed event
 */
class LevelCompletedEvent : public Event {
public:
    LevelCompletedEvent(const std::string& levelName, int score, float time)
        : levelName(levelName), finalScore(score), completionTime(time) {
    }

    const char* getName() const override { return "LevelCompleted"; }

    std::string levelName;
    int finalScore;
    float completionTime;
};

/**
 * Player state changed event
 */
class PlayerStateChangedEvent : public Event {
public:
    PlayerStateChangedEvent(const std::string& oldState, const std::string& newState)
        : oldStateName(oldState), newStateName(newState) {
    }

    const char* getName() const override { return "PlayerStateChanged"; }

    std::string oldStateName;
    std::string newStateName;
};

// Add to GameEvents.h:
class CoinCollectedEvent : public Event {
public:
    CoinCollectedEvent(Entity::IdType playerId, int totalCoins)
        : playerId(playerId), totalCoins(totalCoins) {
    }

    const char* getName() const override { return "CoinCollected"; }

    Entity::IdType playerId;
    int totalCoins;
};

/**
 * Flag reached event 
 */
class FlagReachedEvent : public Event {
public:
    FlagReachedEvent(Entity::IdType playerId, Entity::IdType flagId, const std::string& currentLevel)
        : playerId(playerId), flagId(flagId), currentLevel(currentLevel) {
    }

    const char* getName() const override { return "FlagReached"; }

    Entity::IdType playerId;
    Entity::IdType flagId;
    std::string currentLevel;
};

/**
 * Level transition event -
 */
class LevelTransitionEvent : public Event {
public:
    LevelTransitionEvent(const std::string& fromLevel, const std::string& toLevel, bool isGameComplete = false)
        : fromLevel(fromLevel), toLevel(toLevel), isGameComplete(isGameComplete) {
    }

    const char* getName() const override { return "LevelTransition"; }

    std::string fromLevel;
    std::string toLevel;
    bool isGameComplete;
};