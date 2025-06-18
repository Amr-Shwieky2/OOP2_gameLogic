#pragma once
#include "GameObject.h"

// Forward declarations for the Visitor
class Player;
class Coin;
class LifeHeartGift;
class SpeedGift;
class ReverseMovementGift;
class ProtectiveShieldGift;  // Assuming this is defined elsewhere
class HeadwindStormGift;  // Assuming this is defined elsewhere
class CloseBox;  // Assuming CloseBox is defined elsewhere
class RareCoinGift;  // Assuming RareCoinGift is defined elsewhere
class Flag;
class GroundTile;

// Add others as needed

class GameObjectVisitor {
public:
    virtual ~GameObjectVisitor() = default;

    virtual void visit(Player&) = 0;
    virtual void visit(Coin&) = 0;
    virtual void visit(LifeHeartGift&) = 0;
    virtual void visit(SpeedGift&) = 0;
    virtual void visit(ReverseMovementGift&) = 0;
    virtual void visit(ProtectiveShieldGift&) = 0;
    virtual void visit(HeadwindStormGift&) = 0;
    virtual void visit(RareCoinGift&) = 0;
    virtual void visit(CloseBox&) = 0;

    // ✅ Add these:
    virtual void visit(Flag&) = 0;
    virtual void visit(GroundTile&) = 0;
};