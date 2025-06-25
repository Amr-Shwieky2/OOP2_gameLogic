#pragma once
#include "DynamicGameObject.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include "Projectile.h"

class FalconEnemy : public DynamicGameObject {
public:
    FalconEnemy(b2World& world, float x, float y, TextureManager& textures, int direction = -1);

    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    sf::Vector2f getPosition() const;

    void shoot(TextureManager& textures);
    bool isTimeToAppear(float elapsedTime);

    void kill();

private:
    b2Body* m_body;
    sf::Sprite m_sprite1;
    sf::Sprite m_sprite2;
    bool m_useFirstSprite = true;
    bool m_alive = true;
    float m_animationTimer = 0.f;
    float m_shootCooldown = 0.f;
	static constexpr float m_fireInterval = 1.0f; // Time between shots
	int m_direction; // -1 for left, 1 for right

    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    void switchSprite(float deltaTime);
};
