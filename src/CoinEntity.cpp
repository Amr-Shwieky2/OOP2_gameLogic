#include "CoinEntity.h"

CoinEntity::CoinEntity(IdType id)
    : CollectibleEntity(id)
{
    // Attach a Transform component to this coin
    addComponent<Transform>();
}

void CoinEntity::onCollect(Entity* collector) {
    // Implement your coin collection logic here
    // For example: increment score, play sound, mark as inactive, etc.
    setActive(false);
}