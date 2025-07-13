#pragma once
#include <cstdint>

class Entity;

// Base class for all components
class Component {
public:
    using IdType = std::uint32_t;

    Component();
    virtual ~Component();

    // Set the entity this component is attached to
    void setOwner(Entity* owner);
    Entity* getOwner() const;

    // Called once per frame (if needed)
    virtual void update(float) {}

    // Called when entity is destroyed (optional override)
    virtual void onDestroy() {}

protected:
    Entity* m_owner = nullptr;
};