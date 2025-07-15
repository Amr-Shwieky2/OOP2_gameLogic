#include "PlayerInputHandler.h"
#include "PlayerEntity.h"
#include "PlayerStateManager.h"
#include "PlayerWeaponSystem.h"
#include "PhysicsComponent.h"
#include "Constants.h"
#include <AudioManager.h>

//-------------------------------------------------------------------------------------
PlayerInputHandler::PlayerInputHandler(PlayerEntity& player)
    : m_player(player), m_enabled(true) {
}
//-------------------------------------------------------------------------------------
void PlayerInputHandler::handleInput(const InputService& input) {
    if (!m_enabled) return;

    // Delegate movement input to current state
    if (auto* stateManager = m_player.getStateManager()) {
        if (auto* currentState = stateManager->getCurrentState()) {
            currentState->handleInput(m_player, input);
        }
    }

    // Handle action input directly
    processActionInput(input);
}
//-------------------------------------------------------------------------------------
void PlayerInputHandler::processActionInput(const InputService& input) {
    // Shooting
    if (input.isKeyPressed(sf::Keyboard::C)) {
        AudioManager::instance().playSound("shoot");
        if (auto* weaponSystem = m_player.getWeaponSystem()) {
            weaponSystem->shoot();
        }
    }

    // Backward shooting with B
    if (input.isKeyPressed(sf::Keyboard::B)) {
        AudioManager::instance().playSound("shoot");
        if (auto* weaponSystem = m_player.getWeaponSystem()) {
            weaponSystem->shootBackward();
        }
    }

    // Special gravity shot with F (left-upward)
    if (input.isKeyPressed(sf::Keyboard::F)) {
        AudioManager::instance().playSound("shoot");
        if (auto* weaponSystem = m_player.getWeaponSystem()) {
            weaponSystem->shootSpecialGravity();
        }
    }
    
    // Forward gravity shot with V
    if (input.isKeyPressed(sf::Keyboard::V)) {
        AudioManager::instance().playSound("shoot");
        if (auto* weaponSystem = m_player.getWeaponSystem()) {
            weaponSystem->shootForwardGravity();
        }
    }
}
//-------------------------------------------------------------------------------------
void PlayerInputHandler::processMovementInput(const InputService& input) {
    // Movement input is delegated to the current player state
    // Each state handles movement differently (normal, reversed, boosted, etc.)
    if (auto* stateManager = m_player.getStateManager()) {
        if (auto* currentState = stateManager->getCurrentState()) {
            currentState->handleInput(m_player, input);
        }
    }
}
//-------------------------------------------------------------------------------------