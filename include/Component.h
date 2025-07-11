#pragma once
#include <cstdint>
#include "Property/PropertyContainer.h"

class Entity;

// Base class for all components
class Component : public PropertyContainer {
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
    
    // PropertyContainer interface
    virtual void registerProperties() override;
    
    // Handle property changes
    virtual void onPropertyChanged(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue) override;

protected:
    Entity* m_owner = nullptr;
};