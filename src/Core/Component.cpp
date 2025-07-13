#include "Component.h"
#include "Entity.h"

Component::Component() : m_owner(nullptr) {}

Component::~Component() {}

void Component::setOwner(Entity* owner) {
    m_owner = owner;
}

Entity* Component::getOwner() const {
    return m_owner;
}