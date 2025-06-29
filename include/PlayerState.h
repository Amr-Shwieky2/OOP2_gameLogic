// PlayerState.h
#pragma once
#include <memory>
#include "InputService.h"

class PlayerEntity;

/**
 * Base class for all player states
 * Implements State Pattern from course material
 */
class PlayerState {
public:
    virtual ~PlayerState() = default;

    // Called when entering this state
    virtual void enter(PlayerEntity& player) = 0;

    // Called when exiting this state
    virtual void exit(PlayerEntity& player) = 0;

    // Update the state
    virtual void update(PlayerEntity& player, float dt) = 0;

    // Handle input in this state
    virtual void handleInput(PlayerEntity& player, const InputService& input) = 0;

    // Get state name for debugging
    virtual const char* getName() const = 0;
};

// Forward declarations for concrete states
class NormalState;
class ShieldedState;
class StunnedState;
class BoostedState;