#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "DynamicGameObject.h"
#include "ResourceManager.h"
#include "Constants.h"

class SquareEnemy : public DynamicGameObject {
public:
    SquareEnemy(b2World& world, float x, float y, TextureManager& textures);

    void followPlayer(const sf::Vector2f& playerPos);
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    bool isAlive() const;
    void kill();

    bool canDamage() const;
    void startDamageCooldown();
    void blockMovement();


private:
    void updateVisuals();
    void updatePhysics();

    b2Body* m_body = nullptr;
    sf::Sprite m_sprite;
    TextureManager& m_textures;

    bool m_alive = true;
    float m_speed = 1.0f;

    float m_damageCooldown = 0.f;
    float m_damageInterval = 1.0f;
};
