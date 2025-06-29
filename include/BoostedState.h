// BoostedState.h
#pragma once
#include "PlayerState.h"

/**
 * Boosted state - player moves faster
 */
class BoostedState : public PlayerState {
public:
    void enter(PlayerEntity& player) override;
    void exit(PlayerEntity& player) override;
    void update(PlayerEntity& player, float dt) override;
    void handleInput(PlayerEntity& player, const InputService& input) override;
    const char* getName() const override { return "Boosted"; }

    static PlayerState* getInstance();

private:
    BoostedState() = default;
    static std::unique_ptr<BoostedState> s_instance;
    float m_duration = 0.0f;
};