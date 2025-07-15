#pragma once
#include "Entity.h"
#include "ResourceManager.h"
#include"PlayerStateManager.h"
#include"PlayerInputHandler.h"
#include"PlayerScoreManager.h"
#include"PlayerVisualEffects.h"
#include"PlayerWeaponSystem.h"

/**
 * PlayerEntity -  only responsible for:
 * - Being a player entity
 * - Coordinating its subsystems
 * - Basic entity lifecycle
 */
class PlayerEntity : public Entity {
public:
    PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void update(float dt) override;

    // System accessors
    PlayerStateManager* getStateManager() const { return m_stateManager.get(); }
    PlayerInputHandler* getInputHandler() const { return m_inputHandler.get(); }
    PlayerScoreManager* getScoreManager() const { return m_scoreManager.get(); }
    PlayerVisualEffects* getVisualEffects() const { return m_visualEffects.get(); }
    PlayerWeaponSystem* getWeaponSystem() const { return m_weaponSystem.get(); }

    // Convenience methods that delegate to appropriate systems
    void handleInput(const InputService& input);
    void addScore(int points);
    int getScore() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    bool isOnGround() const;

    sf::Keyboard::Key getJumpKey() const;

    PlayerState* getCurrentState() const;

    // Texture access for states that need to change player appearance
    TextureManager& getTextures() { return m_textures; }

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);

    // Specialized subsystems - each with single responsibility
    std::unique_ptr<PlayerStateManager> m_stateManager;
    std::unique_ptr<PlayerInputHandler> m_inputHandler;
    std::unique_ptr<PlayerScoreManager> m_scoreManager;
    std::unique_ptr<PlayerVisualEffects> m_visualEffects;
    std::unique_ptr<PlayerWeaponSystem> m_weaponSystem;

    TextureManager& m_textures;
    b2World& m_world;
};