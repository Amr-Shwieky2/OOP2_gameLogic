#pragma once
#include "ComponentTraits.h"

// Forward declarations of component types
class Transform;
class RenderComponent;
class PhysicsComponent;
class CollisionComponent;
class AIComponent;
class InputComponent;
class HealthComponent;

// Specializations for Transform component
template<>
struct ComponentTraits::ComponentCategoryTrait<Transform> {
    static constexpr ComponentCategory value = ComponentCategory::Transform;
};

template<>
struct ComponentTraits::PerformanceImpact<Transform> {
    static constexpr int value = 1; // Very low impact
};

template<>
struct ComponentTraits::IsSerializable<Transform> : std::true_type {};

// Specializations for RenderComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<RenderComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Visual;
};

template<>
struct ComponentTraits::Dependencies<RenderComponent> {
    using type = std::tuple<Transform>; // RenderComponent depends on Transform
};

template<>
struct ComponentTraits::NeedsInitialization<RenderComponent> : std::true_type {};

template<>
struct ComponentTraits::PerformanceImpact<RenderComponent> {
    static constexpr int value = 5; // Medium impact (rendering can be costly)
};

// Specializations for PhysicsComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<PhysicsComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Physics;
};

template<>
struct ComponentTraits::Dependencies<PhysicsComponent> {
    using type = std::tuple<Transform>; // PhysicsComponent depends on Transform
};

template<>
struct ComponentTraits::NeedsInitialization<PhysicsComponent> : std::true_type {};

template<>
struct ComponentTraits::PerformanceImpact<PhysicsComponent> {
    static constexpr int value = 7; // High impact (physics simulation is expensive)
};

// Specializations for CollisionComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<CollisionComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Physics;
};

template<>
struct ComponentTraits::Dependencies<CollisionComponent> {
    using type = std::tuple<Transform, PhysicsComponent>; // Depends on Transform and Physics
};

// Specializations for AIComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<AIComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Logic;
};

template<>
struct ComponentTraits::PerformanceImpact<AIComponent> {
    static constexpr int value = 6; // Fairly high impact (AI calculations)
};

template<>
struct ComponentTraits::NeedsInitialization<AIComponent> : std::true_type {};

// Specializations for InputComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<InputComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Input;
};

// Specializations for HealthComponent
template<>
struct ComponentTraits::ComponentCategoryTrait<HealthComponent> {
    static constexpr ComponentCategory value = ComponentCategory::Logic | ComponentCategory::Data;
};

template<>
struct ComponentTraits::IsSerializable<HealthComponent> : std::true_type {};

// Special compatibility rules
// Example: Define components that cannot work together
template<>
struct ComponentTraits::AreCompatible<InputComponent, AIComponent> : std::false_type {
    // An entity should not have both player input and AI controlling it
};