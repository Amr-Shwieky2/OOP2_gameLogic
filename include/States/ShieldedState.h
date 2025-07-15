#pragma once
#include "PlayerState.h"

/**
 * Shielded state - player is invulnerable
 */
class ShieldedState : public PlayerState {
public:
    void enter(PlayerEntity& player) override;
    void exit(PlayerEntity& player) override;
    void update(PlayerEntity& player, float dt) override;
    void handleInput(PlayerEntity& player, const InputService& input) override;
    const char* getName() const override { return "Shielded"; }

    static PlayerState* getInstance();

private:
    ShieldedState() = default;
    static std::unique_ptr<ShieldedState> s_instance;
    float m_duration = 0.0f;
};