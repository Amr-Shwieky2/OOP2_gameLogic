#include "SquareEnemy.h"
#include <iostream>

SquareEnemy::SquareEnemy(b2World& world, float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    // Box2D body setup
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y + ENEMY_HEIGHT / 2.f);
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(ENEMY_WIDTH / 2.f, ENEMY_HEIGHT / 2.f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    m_body->CreateFixture(&fixtureDef);

    // SFML sprite setup
    m_sprite.setTexture(m_textures.getResource("SquareEnemy.png"));
    sf::Vector2u texSize = m_sprite.getTexture()->getSize();
    m_sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);

    float desiredWidth = ENEMY_WIDTH * PPM;
    float desiredHeight = ENEMY_HEIGHT * PPM;
    m_sprite.setScale(desiredWidth / texSize.x, desiredHeight / texSize.y);

    updateVisuals();
}

void SquareEnemy::followPlayer(const sf::Vector2f& playerPos)
{
    if (!m_alive || !m_body) return;

    float playerX = playerPos.x / PPM;
    float myX = m_body->GetPosition().x;

    float direction = (playerX > myX) ? 1.f : -1.f;

    b2Vec2 velocity = m_body->GetLinearVelocity();
    velocity.x = direction * m_speed;
    m_body->SetLinearVelocity(velocity);
}

void SquareEnemy::update(float deltaTime)
{
    if (!m_alive || !m_body) return;

    if (m_damageCooldown > 0.f)
        m_damageCooldown -= deltaTime;

    updatePhysics();
    updateVisuals();
}

void SquareEnemy::updatePhysics() {
    if (!m_body) return;

    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(pos.x * PPM, pos.y * PPM);

    b2Vec2 velocity = m_body->GetLinearVelocity();
    float angularVel = -velocity.x / (ENEMY_WIDTH / 2.f); // negative for leftward roll
    m_body->SetAngularVelocity(angularVel);

    // Then apply rotation visually
    float angle = m_body->GetAngle();
    m_sprite.setRotation(angle * 180.f / b2_pi);
}

void SquareEnemy::updateVisuals()
{
    if (!m_alive)
        m_sprite.setColor(sf::Color(255, 255, 255, 100)); // faded color when dead
}

void SquareEnemy::render(sf::RenderTarget& target) const
{
    if (m_alive)
        target.draw(m_sprite);
}

sf::FloatRect SquareEnemy::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

bool SquareEnemy::isAlive() const
{
    return m_alive;
}

void SquareEnemy::kill() {
    m_alive = false;

    if (m_body) {
        // Disable collisions by setting to static and removing all fixtures
        m_body->SetType(b2_staticBody);

        // Destroy all fixtures (to remove collision)
        for (b2Fixture* f = m_body->GetFixtureList(); f; ) {
            b2Fixture* next = f->GetNext();
            m_body->DestroyFixture(f);
            f = next;
        }
    }

    // Visually fade
    m_sprite.setColor(sf::Color(255, 255, 255, 50));
}

bool SquareEnemy::canDamage() const
{
    return m_damageCooldown <= 0.f;
}

void SquareEnemy::startDamageCooldown()
{
    m_damageCooldown = m_damageInterval;
}