#pragma once
#include "PlayerState.h"

class ReversedState : public PlayerState {
public:
    void enter(PlayerEntity& player) override;
    void exit(PlayerEntity& player) override;
    void update(PlayerEntity& player, float dt) override;
    void handleInput(PlayerEntity& player, const InputService& input) override;
    const char* getName() const override { return "Reversed"; }

    static PlayerState* getInstance();

private:
    ReversedState() = default;
    static std::unique_ptr<ReversedState> s_instance;
    float m_duration = 0.0f;
};