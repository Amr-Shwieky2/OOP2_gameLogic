#pragma once
#include <memory>
#include "InputService.h"

class PlayerEntity;

/**
 * PlayerState - Updated to work with new PlayerEntity structure
 */
class PlayerState {
public:
    virtual ~PlayerState() = default;

    virtual void enter(PlayerEntity& player) = 0;
    virtual void exit(PlayerEntity& player) = 0;
    virtual void update(PlayerEntity& player, float dt) = 0;
    virtual void handleInput(PlayerEntity& player, const InputService& input) = 0;
    virtual const char* getName() const = 0;
};