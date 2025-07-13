#include "CollectibleEntity.h"

CollectibleEntity::CollectibleEntity(IdType id)
    : Entity(id)
{
    // Typically, a collectible will always have a Transform
    // This is where you might add it automatically if desired
}