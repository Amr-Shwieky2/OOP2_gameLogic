#include "CollisionComponent.h"
#include "Entity.h"
#include "Transform.h"

CollisionComponent::CollisionComponent(CollisionType type)
    : m_type(type) {
}