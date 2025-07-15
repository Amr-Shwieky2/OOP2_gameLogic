#pragma once
#include "PlayerState.h"

/**
 * Normal player state - default behavior
 */
class NormalState : public PlayerState {
public:
    void enter(PlayerEntity& player) override;
    void exit(PlayerEntity& player) override;
    void update(PlayerEntity& player, float dt) override;
    void handleInput(PlayerEntity& player, const InputService& input) override;
    const char* getName() const override { return "Normal"; }

    // Singleton pattern for states
    static PlayerState* getInstance();

private:
    NormalState() = default;
    static std::unique_ptr<NormalState> s_instance;
};