// PlayerEntity.h - Enhanced with State Pattern
#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "ResourceManager.h"
#include "InputService.h"

class PlayerState;
class GameSession;

/**
 * PlayerEntity - Enhanced with State Pattern
 */
class PlayerEntity : public Entity {
public:
    PlayerEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    // Override update to use state
    void update(float dt) override;

    // State management
    void changeState(PlayerState* newState);
    PlayerState* getCurrentState() const { return m_currentState; }

    // Input handling through state
    void handleInput(const InputService& input);

    // Movement methods (called by states)
    void jump();
    void moveLeft();
    void moveRight();
    void shoot();

    // Score/Lives management
    void addScore(int points);
    int getScore() const { return m_score; }

    // Effects (now trigger state changes)
    void applySpeedBoost(float duration);
    void applyShield(float duration);

    // Getters
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    bool isOnGround() const;
    TextureManager& getTextures() { return m_textures; }

    static void setGameSession(GameSession* session) { s_gameSession = session; }
    static GameSession* getGameSession() { return s_gameSession; }

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
    void updateVisuals();
    void updatePhysics();
    void applyRollRotation(float dt);

    int m_score = 0;
    TextureManager& m_textures;

    // State Pattern
    PlayerState* m_currentState = nullptr;

    // Ground detection
    int m_groundContacts = 0;

    static GameSession* s_gameSession;
};