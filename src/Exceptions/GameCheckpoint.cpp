#include "Exceptions/GameCheckpoint.h"
#include "GameSession.h"
#include "PlayerEntity.h"
#include "Entity.h"
#include "HealthComponent.h"
#include "PhysicsComponent.h"
#include "Transform.h"
#include "Exceptions/Logger.h"
#include <format>

namespace GameExceptions {

std::shared_ptr<RecoveryCheckpoint> GameCheckpoint::create(
    const std::string& name, 
    GameSession* gameSession)
{
    if (!gameSession) {
        getLogger().error("Cannot create checkpoint: GameSession is null");
        return nullptr;
    }
    
    // Create checkpoint
    auto checkpoint = ExceptionRecoverySystem::getInstance().createCheckpoint(name);
    
    // Save game state
    checkpoint->saveState("levelName", gameSession->getCurrentLevelName());
    checkpoint->saveState("score", gameSession->getScore());
    
    // Save player state
    auto* player = gameSession->getPlayer();
    if (player) {
        savePlayerState(checkpoint, player);
    } else {
        getLogger().warning("No player to save in checkpoint");
    }
    
    // Save level state
    saveLevelState(checkpoint, gameSession);
    
    // Set restore function
    auto restoreFunc = createRestoreFunction(gameSession);
    checkpoint->saveState("restoreFunction", restoreFunc);
    
    getLogger().info(std::format("Created game checkpoint: {}", name));
    
    return checkpoint;
}

bool GameCheckpoint::savePlayerState(
    std::shared_ptr<RecoveryCheckpoint> checkpoint,
    const PlayerEntity* player)
{
    if (!checkpoint || !player) {
        return false;
    }
    
    try {
        // Save position
        auto* transform = player->getComponent<Transform>();
        if (transform) {
            sf::Vector2f position = transform->getPosition();
            checkpoint->saveState("playerPositionX", position.x);
            checkpoint->saveState("playerPositionY", position.y);
        }
        
        // Save health
        auto* health = player->getComponent<HealthComponent>();
        if (health) {
            checkpoint->saveState("playerHealth", health->getHealth());
            checkpoint->saveState("playerMaxHealth", health->getMaxHealth());
            checkpoint->saveState("playerInvulnerable", health->isInvulnerable());
        }
        
        // Save physics state
        auto* physics = player->getComponent<PhysicsComponent>();
        if (physics) {
            sf::Vector2f velocity = physics->getVelocity();
            checkpoint->saveState("playerVelocityX", velocity.x);
            checkpoint->saveState("playerVelocityY", velocity.y);
        }
        
        // Save score
        int score = player->getScore();
        checkpoint->saveState("playerScore", score);
        
        // Save state name
        if (auto* state = player->getCurrentState()) {
            checkpoint->saveState("playerStateName", std::string(state->getName()));
        }
        
        getLogger().debug("Saved player state to checkpoint");
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error saving player state: {}", ex.what()));
        return false;
    }
}

bool GameCheckpoint::saveEntityState(
    std::shared_ptr<RecoveryCheckpoint> checkpoint,
    const Entity* entity,
    int entityId)
{
    if (!checkpoint || !entity) {
        return false;
    }
    
    try {
        std::string prefix = std::format("entity{}.", entityId);
        
        // Save entity type
        checkpoint->saveState(prefix + "type", std::string(typeid(*entity).name()));
        
        // Save position
        auto* transform = entity->getComponent<Transform>();
        if (transform) {
            sf::Vector2f position = transform->getPosition();
            checkpoint->saveState(prefix + "positionX", position.x);
            checkpoint->saveState(prefix + "positionY", position.y);
        }
        
        // Save physics state if available
        auto* physics = entity->getComponent<PhysicsComponent>();
        if (physics) {
            sf::Vector2f velocity = physics->getVelocity();
            checkpoint->saveState(prefix + "velocityX", velocity.x);
            checkpoint->saveState(prefix + "velocityY", velocity.y);
        }
        
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error saving entity state: {}", ex.what()));
        return false;
    }
}

bool GameCheckpoint::saveLevelState(
    std::shared_ptr<RecoveryCheckpoint> checkpoint,
    const GameSession* gameSession)
{
    if (!checkpoint || !gameSession) {
        return false;
    }
    
    try {
        // Save level name
        checkpoint->saveState("levelName", gameSession->getCurrentLevelName());
        
        // Save current score
        checkpoint->saveState("gameScore", gameSession->getScore());
        
        // The complete implementation would save the state of all entities
        // Here we'll just save a basic snapshot
        checkpoint->saveState("levelSnapshot", true);
        
        getLogger().debug("Saved level state to checkpoint");
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error saving level state: {}", ex.what()));
        return false;
    }
}

bool GameCheckpoint::restorePlayerState(
    const RecoveryCheckpoint& checkpoint,
    PlayerEntity* player)
{
    if (!player) {
        return false;
    }
    
    try {
        // Restore position
        auto* transform = player->getComponent<Transform>();
        if (transform) {
            auto posX = checkpoint.getState<float>("playerPositionX");
            auto posY = checkpoint.getState<float>("playerPositionY");
            
            if (posX && posY) {
                transform->setPosition(*posX, *posY);
            }
        }
        
        // Restore health
        auto* health = player->getComponent<HealthComponent>();
        if (health) {
            auto healthVal = checkpoint.getState<int>("playerHealth");
            auto maxHealth = checkpoint.getState<int>("playerMaxHealth");
            auto invulnerable = checkpoint.getState<bool>("playerInvulnerable");
            
            if (healthVal) health->setHealth(*healthVal);
            if (maxHealth) health->setMaxHealth(*maxHealth);
            if (invulnerable) health->setInvulnerable(*invulnerable);
        }
        
        // Restore physics state
        auto* physics = player->getComponent<PhysicsComponent>();
        if (physics) {
            auto velX = checkpoint.getState<float>("playerVelocityX");
            auto velY = checkpoint.getState<float>("playerVelocityY");
            
            if (velX && velY) {
                physics->setVelocity(*velX, *velY);
            }
        }
        
        // Restore score
        auto score = checkpoint.getState<int>("playerScore");
        if (score) {
            player->setScore(*score);
        }
        
        getLogger().info("Restored player state from checkpoint");
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error restoring player state: {}", ex.what()));
        return false;
    }
}

bool GameCheckpoint::restoreEntityState(
    const RecoveryCheckpoint& checkpoint,
    Entity* entity,
    int entityId)
{
    if (!entity) {
        return false;
    }
    
    try {
        std::string prefix = std::format("entity{}.", entityId);
        
        // Restore position
        auto* transform = entity->getComponent<Transform>();
        if (transform) {
            auto posX = checkpoint.getState<float>(prefix + "positionX");
            auto posY = checkpoint.getState<float>(prefix + "positionY");
            
            if (posX && posY) {
                transform->setPosition(*posX, *posY);
            }
        }
        
        // Restore physics state if available
        auto* physics = entity->getComponent<PhysicsComponent>();
        if (physics) {
            auto velX = checkpoint.getState<float>(prefix + "velocityX");
            auto velY = checkpoint.getState<float>(prefix + "velocityY");
            
            if (velX && velY) {
                physics->setVelocity(*velX, *velY);
            }
        }
        
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error restoring entity state: {}", ex.what()));
        return false;
    }
}

bool GameCheckpoint::restoreLevelState(
    const RecoveryCheckpoint& checkpoint,
    GameSession* gameSession)
{
    if (!gameSession) {
        return false;
    }
    
    try {
        // Restore level - if different from current, reload it
        auto levelName = checkpoint.getState<std::string>("levelName");
        if (levelName && *levelName != gameSession->getCurrentLevelName()) {
            gameSession->loadLevel(*levelName);
        }
        
        // Restore score
        auto score = checkpoint.getState<int>("gameScore");
        if (score) {
            gameSession->setScore(*score);
        }
        
        getLogger().info("Restored level state from checkpoint");
        return true;
    }
    catch (const std::exception& ex) {
        getLogger().error(std::format("Error restoring level state: {}", ex.what()));
        return false;
    }
}

std::function<RecoveryResult()> GameCheckpoint::createRestoreFunction(GameSession* gameSession)
{
    // Capture a weak_ptr to avoid dangling pointers if the game session is destroyed
    std::weak_ptr<GameSession> weakSession(std::shared_ptr<GameSession>(gameSession, [](GameSession*) {}));
    
    return [weakSession]() -> RecoveryResult {
        auto gameSession = weakSession.lock();
        if (!gameSession) {
            getLogger().error("Cannot restore checkpoint: GameSession no longer exists");
            return RecoveryResult::Failure;
        }
        
        try {
            // Reload the current level to reset the game state
            std::string currentLevel = gameSession->getCurrentLevelName();
            if (gameSession->loadLevel(currentLevel)) {
                getLogger().info(std::format("Checkpoint restored by reloading level: {}", currentLevel));
                return RecoveryResult::Success;
            } else {
                getLogger().error(std::format("Failed to reload level during checkpoint restoration: {}", currentLevel));
                return RecoveryResult::Failure;
            }
        }
        catch (const std::exception& ex) {
            getLogger().error(std::format("Exception during checkpoint restoration: {}", ex.what()));
            return RecoveryResult::Failure;
        }
    };
}

//-------------------------------------------------------------------------
// AutoCheckpointManager Implementation
//-------------------------------------------------------------------------

AutoCheckpointManager& AutoCheckpointManager::getInstance()
{
    static AutoCheckpointManager instance;
    return instance;
}

void AutoCheckpointManager::initialize(GameSession* gameSession)
{
    m_gameSession = gameSession;
    m_checkpoints.clear();
    m_timeSinceLastCheckpoint = 0.0f;
    
    getLogger().info("AutoCheckpointManager initialized");
}

void AutoCheckpointManager::createLevelStartCheckpoint()
{
    if (!m_gameSession) {
        getLogger().error("Cannot create level start checkpoint: no game session");
        return;
    }
    
    std::string levelName = m_gameSession->getCurrentLevelName();
    std::string checkpointName = std::format("LevelStart_{}", levelName);
    
    auto checkpoint = createCheckpoint(checkpointName);
    if (checkpoint) {
        getLogger().info(std::format("Created level start checkpoint for {}", levelName));
    }
}

void AutoCheckpointManager::createLevelCheckpointAtPosition(float x, float y, const std::string& name)
{
    if (!m_gameSession) {
        getLogger().error("Cannot create position checkpoint: no game session");
        return;
    }
    
    std::string levelName = m_gameSession->getCurrentLevelName();
    std::string checkpointName = name.empty() ? 
        std::format("Position_{:.0f}_{:.0f}", x, y) : name;
    
    auto checkpoint = createCheckpoint(checkpointName);
    if (checkpoint) {
        // Save position information
        checkpoint->saveState("checkpointX", x);
        checkpoint->saveState("checkpointY", y);
        
        getLogger().info(std::format("Created checkpoint at position ({:.1f}, {:.1f})", x, y));
    }
}

void AutoCheckpointManager::createCheckpointAfterEvent(const std::string& eventName)
{
    if (!m_gameSession) {
        getLogger().error("Cannot create event checkpoint: no game session");
        return;
    }
    
    std::string checkpointName = std::format("Event_{}", eventName);
    auto checkpoint = createCheckpoint(checkpointName);
    
    if (checkpoint) {
        // Save event information
        checkpoint->saveState("triggeringEvent", eventName);
        
        getLogger().info(std::format("Created checkpoint after event: {}", eventName));
    }
}

void AutoCheckpointManager::enableIntervalCheckpoints(float seconds)
{
    m_intervalCheckpointsEnabled = true;
    m_checkpointInterval = seconds;
    m_timeSinceLastCheckpoint = 0.0f;
    
    getLogger().info(std::format("Enabled interval checkpoints every {:.1f} seconds", seconds));
}

void AutoCheckpointManager::disableIntervalCheckpoints()
{
    m_intervalCheckpointsEnabled = false;
    getLogger().info("Disabled interval checkpoints");
}

void AutoCheckpointManager::update(float deltaTime)
{
    if (!m_gameSession || !m_intervalCheckpointsEnabled) {
        return;
    }
    
    m_timeSinceLastCheckpoint += deltaTime;
    
    if (m_timeSinceLastCheckpoint >= m_checkpointInterval) {
        std::string checkpointName = std::format("Interval_{:.0f}", m_gameSession->getGameTime());
        
        auto checkpoint = createCheckpoint(checkpointName);
        if (checkpoint) {
            getLogger().debug(std::format("Created interval checkpoint at game time {:.1f}", 
                m_gameSession->getGameTime()));
        }
        
        m_timeSinceLastCheckpoint = 0.0f;
    }
}

std::shared_ptr<RecoveryCheckpoint> AutoCheckpointManager::getLatestCheckpoint() const
{
    if (m_checkpoints.empty()) {
        return nullptr;
    }
    
    return m_checkpoints.back();
}

const std::vector<std::shared_ptr<RecoveryCheckpoint>>& AutoCheckpointManager::getCheckpoints() const
{
    return m_checkpoints;
}

std::shared_ptr<RecoveryCheckpoint> AutoCheckpointManager::createCheckpoint(const std::string& name)
{
    if (!m_gameSession) {
        getLogger().error("Cannot create checkpoint: no game session");
        return nullptr;
    }
    
    auto checkpoint = GameCheckpoint::create(name, m_gameSession);
    if (checkpoint) {
        m_checkpoints.push_back(checkpoint);
        
        // Keep only a reasonable number of checkpoints to avoid memory issues
        const size_t MAX_CHECKPOINTS = 10;
        if (m_checkpoints.size() > MAX_CHECKPOINTS) {
            m_checkpoints.erase(m_checkpoints.begin());
        }
    }
    
    return checkpoint;
}

} // namespace GameExceptions