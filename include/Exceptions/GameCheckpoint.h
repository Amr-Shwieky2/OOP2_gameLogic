#pragma once

#include "Exceptions/ExceptionRecoverySystem.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include <optional>
#include <vector>

// Forward declarations
class GameSession;
class PlayerEntity;
class Entity;

namespace GameExceptions {

/**
 * @brief Game-specific checkpoint implementation
 * 
 * Extends RecoveryCheckpoint with game-specific state data and restoration logic
 */
class GameCheckpoint {
public:
    /**
     * @brief Create a new game checkpoint
     * 
     * @param name Checkpoint name
     * @param gameSession Pointer to the current game session
     * @return std::shared_ptr<RecoveryCheckpoint> The created checkpoint
     */
    static std::shared_ptr<RecoveryCheckpoint> create(
        const std::string& name, 
        GameSession* gameSession);
    
    /**
     * @brief Save player state to a checkpoint
     * 
     * @param checkpoint The checkpoint to save to
     * @param player The player entity
     * @return true if successful
     */
    static bool savePlayerState(
        std::shared_ptr<RecoveryCheckpoint> checkpoint,
        const PlayerEntity* player);
    
    /**
     * @brief Save entity state to a checkpoint
     * 
     * @param checkpoint The checkpoint to save to
     * @param entity The entity
     * @param entityId The entity ID
     * @return true if successful
     */
    static bool saveEntityState(
        std::shared_ptr<RecoveryCheckpoint> checkpoint,
        const Entity* entity,
        int entityId);
    
    /**
     * @brief Save level state to a checkpoint
     * 
     * @param checkpoint The checkpoint to save to
     * @param gameSession The game session
     * @return true if successful
     */
    static bool saveLevelState(
        std::shared_ptr<RecoveryCheckpoint> checkpoint,
        const GameSession* gameSession);
    
    /**
     * @brief Restore player state from a checkpoint
     * 
     * @param checkpoint The checkpoint to restore from
     * @param player The player entity
     * @return true if successful
     */
    static bool restorePlayerState(
        const RecoveryCheckpoint& checkpoint,
        PlayerEntity* player);
    
    /**
     * @brief Restore entity state from a checkpoint
     * 
     * @param checkpoint The checkpoint to restore from
     * @param entity The entity
     * @param entityId The entity ID
     * @return true if successful
     */
    static bool restoreEntityState(
        const RecoveryCheckpoint& checkpoint,
        Entity* entity,
        int entityId);
    
    /**
     * @brief Restore level state from a checkpoint
     * 
     * @param checkpoint The checkpoint to restore from
     * @param gameSession The game session
     * @return true if successful
     */
    static bool restoreLevelState(
        const RecoveryCheckpoint& checkpoint,
        GameSession* gameSession);
    
    /**
     * @brief Create a checkpoint restoration function
     * 
     * @param gameSession The game session to restore
     * @return std::function<RecoveryResult()> The restoration function
     */
    static std::function<RecoveryResult()> createRestoreFunction(GameSession* gameSession);
};

/**
 * @brief Automatic checkpoint manager
 * 
 * Creates checkpoints at appropriate times during gameplay
 */
class AutoCheckpointManager {
public:
    static AutoCheckpointManager& getInstance();
    
    // Initialize the checkpoint manager
    void initialize(GameSession* gameSession);
    
    // Create checkpoints at key moments
    void createLevelStartCheckpoint();
    void createLevelCheckpointAtPosition(float x, float y, const std::string& name = "");
    void createCheckpointAfterEvent(const std::string& eventName);
    
    // Create a checkpoint on a fixed interval
    void enableIntervalCheckpoints(float seconds);
    void disableIntervalCheckpoints();
    
    // Update should be called every frame
    void update(float deltaTime);
    
    // Get the latest checkpoint
    std::shared_ptr<RecoveryCheckpoint> getLatestCheckpoint() const;
    
    // Get all checkpoints
    const std::vector<std::shared_ptr<RecoveryCheckpoint>>& getCheckpoints() const;
    
    // Set the current game session
    void setGameSession(GameSession* gameSession) { m_gameSession = gameSession; }
    
private:
    AutoCheckpointManager() = default;
    
    GameSession* m_gameSession = nullptr;
    bool m_intervalCheckpointsEnabled = false;
    float m_checkpointInterval = 60.0f;  // Default: 60 seconds
    float m_timeSinceLastCheckpoint = 0.0f;
    
    std::vector<std::shared_ptr<RecoveryCheckpoint>> m_checkpoints;
    
    // Helper to create a named checkpoint
    std::shared_ptr<RecoveryCheckpoint> createCheckpoint(const std::string& name);
};

} // namespace GameExceptions