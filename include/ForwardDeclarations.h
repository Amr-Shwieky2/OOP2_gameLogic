#pragma once

// Forward declarations for components and entities
class Entity;
class Component;
class Transform;
class PhysicsComponent;
class RenderComponent;
class CollisionComponent;
class AIComponent;
class InputComponent;
class HealthComponent;

// Forward declarations for Box2D
class b2World;
class b2Body;

// Forward declarations for SFML
namespace sf {
    class RenderTarget;
    class Sprite;
    class Texture;
    class Color;
    class View;
    
    template<typename T>
    class Vector2;
    
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
}