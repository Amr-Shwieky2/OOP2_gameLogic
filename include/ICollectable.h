#pragma once

#include "PlayerEffect.h"

// Forward declaration
class GameState;

class ICollectable {
public:
    virtual ~ICollectable() = default;

    // Core collectable interface
    virtual void onCollect(GameState& gameState) = 0;
    virtual bool isCollected() const = 0;
    virtual int getValue() const = 0;

    // Optional effect on player
    virtual PlayerEffect getEffect() const { return PlayerEffect::None; }
    virtual float getEffectDuration() const { return 0.0f; }

    // Type identification (for sound effects, UI feedback, etc.)
    enum class CollectableType {
        Coin,
        RareCoin,
        LifeHeart,
        SpeedBoost,
        Shield,
        ReverseControl,
        HeadwindStorm,
        Magnetic
    };

    virtual CollectableType getType() const = 0;
};