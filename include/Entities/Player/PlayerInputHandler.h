#pragma once
#include "InputService.h"

class PlayerEntity;

/**
 * PlayerInputHandler - Single Responsibility: Process player input
 */
class PlayerInputHandler {
public:
    PlayerInputHandler(PlayerEntity& player);

    void handleInput(const InputService& input);
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    PlayerEntity& m_player;
    bool m_enabled = true;

    void processMovementInput(const InputService& input);
    void processActionInput(const InputService& input);
};
