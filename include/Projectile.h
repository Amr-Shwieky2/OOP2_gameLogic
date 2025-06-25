// Projectile.h
#pragma once
#include "DynamicGameObject.h"
#include <Box2D/Box2D.h>
#include <ResourceManager.h>

class Projectile : public DynamicGameObject {
public:
    Projectile(b2World& world, float x, float y, float direction, TextureManager& textures, bool isEnemyShot, bool isCurved = false);

    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    bool isAlive() const { return m_alive; }
    void destroy() { m_alive = false; }

    bool isEnemyShot() const { return m_fromEnemy; }
    b2Body* getBody() const { return m_body; }


private:
    b2Body* m_body;
    sf::Sprite m_sprite;
    TextureManager& m_textures;
    bool m_alive = true;
    bool m_fromEnemy;
    bool m_isCurved = false;

};
