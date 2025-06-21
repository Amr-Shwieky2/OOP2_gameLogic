#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "DynamicGameObject.h"
#include "ResourceManager.h"
#include "GameObjectVisitor.h"
#include <TileType.h>

class MovableBox : public DynamicGameObject {
public:
    MovableBox(b2World& world, float x, float y, TileType type, TextureManager& textures);
    ~MovableBox();

    // من DynamicGameObject
    void update(float deltaTime) override;

    // من GameObject
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    // دوال خاصة بالصندوق
    b2Body* getBody() const { return m_body; }
    void applyForce(float forceX, float forceY);
    bool isMoving() const;

private:
    sf::Sprite m_sprite;
    b2Body* m_body;
    b2World& m_world;

    // إعدادات الفيزياء
    static constexpr float BOX_DENSITY = 1.0f;
    static constexpr float BOX_FRICTION = 0.7f;
    static constexpr float BOX_RESTITUTION = 0.1f; // مقاومة الارتداد
    static constexpr float BOX_SIZE = 32.0f; // حجم الصندوق بالبكسل

    void createPhysicsBody(float x, float y);
    void updateSpritePosition();
};