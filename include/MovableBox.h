#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "DynamicGameObject.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <TileType.h>

class MovableBox : public DynamicGameObject {
public:
    MovableBox(b2World& world, float x, float y, TileType type, TextureManager& textures);
    ~MovableBox();
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    b2Body* getBody() const { return m_body; }
    void applyForce(float forceX, float forceY);
    bool isMoving() const;

private:
    sf::Sprite m_sprite;
    b2Body* m_body;
    b2World& m_world;

    

    void createPhysicsBody(float x, float y);
    void updateSpritePosition();
};