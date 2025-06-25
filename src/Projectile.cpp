// Projectile.cpp
#include "Projectile.h"
#include "Constants.h"

Projectile::Projectile(b2World& world, float x, float y, float direction, TextureManager& textures, bool isEnemyShot, bool isCurved)
    : m_textures(textures), m_fromEnemy(isEnemyShot), m_isCurved(isCurved)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.bullet = true; // Precise collisions

    // Allow gravity for curved projectiles
    bodyDef.gravityScale = isCurved ? 1.0f : 0.0f;

    m_body = world.CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = 0.1f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.f;
    fixtureDef.friction = 0.f;
    fixtureDef.restitution = 0.f;
    fixtureDef.isSensor = true;
    m_body->CreateFixture(&fixtureDef);

    if (isCurved) {
        // Apply initial upward and forward velocity
        m_body->SetLinearVelocity(b2Vec2(direction * 6.f, -10.f)); // adjust for arc
    }
    else {
        // Straight shot
        m_body->SetLinearVelocity(b2Vec2(direction * 10.f, 0.f));
    }

    // Sprite setup
    m_sprite.setTexture(m_textures.getResource("Bullet.png"));
    auto size = m_sprite.getTexture()->getSize();
    m_sprite.setOrigin(size.x / 2.f, size.y / 2.f);
    m_sprite.setScale(0.05f, 0.05f);
}

void Projectile::update(float) {
    if (!m_alive || !m_body) return;
    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(pos.x * PPM, pos.y * PPM);
}

void Projectile::render(sf::RenderTarget& target) const {
    if (m_alive)
        target.draw(m_sprite);
}

sf::FloatRect Projectile::getBounds() const {
    return m_sprite.getGlobalBounds();
}
