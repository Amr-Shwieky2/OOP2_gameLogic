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

// Add others as needed

class GameObjectVisitor {
public:
    virtual ~GameObjectVisitor() = default;

    virtual void visit(Player& player) = 0;
    virtual void visit(Coin& coin) = 0;
    virtual void visit(LifeHeartGift& gift) = 0;
    virtual void visit(SpeedGift& gift) = 0;
    virtual void visit(ReverseMovementGift& gift) = 0;
	virtual void visit(ProtectiveShieldGift& gift) = 0;
	virtual void visit(HeadwindStormGift& gift) = 0; // Assuming this is defined elsewhere
	virtual void visit(CloseBox& box) = 0; // Assuming CloseBox is defined elsewhere
	virtual void visit(RareCoinGift& gift) = 0; // Assuming RareCoinGift is defined elsewhere


    // Add visit(...) for other game objects
};