#pragma once
#include "Entity.h"

// Base class for all collectible entities (e.g., Coin, Gifts)
class CollectibleEntity : public Entity {
public:
    explicit CollectibleEntity(IdType id);
    virtual ~CollectibleEntity() = default;

    // Called when the collectible is collected by a player
    virtual void onCollect(Entity* collector) = 0;
};