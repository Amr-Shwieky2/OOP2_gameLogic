#pragma once
#include "PlayerState.h"
#include <set>

class CoinEntity;

class MagneticState : public PlayerState {
public:
    void enter(PlayerEntity& player) override;
    void exit(PlayerEntity& player) override;
    void update(PlayerEntity& player, float dt) override;
    void handleInput(PlayerEntity& player, const InputService& input) override;
    const char* getName() const override { return "Magnetic"; }

    static PlayerState* getInstance();

private:
    MagneticState() = default;
    static std::unique_ptr<MagneticState> s_instance;
    float m_duration = 0.0f;
    float m_attractTimer = 0.0f;

    std::set<CoinEntity*> m_attractedCoins;
};