#include "FalconEnemy.h"
#include "Projectile.h"
#include "Constants.h"

FalconEnemy::FalconEnemy(b2World& world, float x, float y, TextureManager& textures, int direction)
    : m_direction(direction)
{
    // Body setup
    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position.Set(x / PPM, y / PPM);
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(1.0f, 0.5f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;
    m_body->CreateFixture(&fixtureDef);

    // Velocity: always moving left
    m_body->SetLinearVelocity(b2Vec2(direction * 1.5f, 0.f));

    // Sprite setup
    sf::Texture& tex1 = textures.getResource("FalconEnemy.png");
    sf::Texture& tex2 = textures.getResource("FalconEnemy2.png");

    m_sprite1.setTexture(tex1);
    m_sprite2.setTexture(tex2);

    m_sprite1.setOrigin(tex1.getSize().x / 2, tex1.getSize().y / 2);
    m_sprite2.setOrigin(tex2.getSize().x / 2, tex2.getSize().y / 2);

    float scale = 0.2f;
    m_sprite1.setScale(scale * direction, scale);  // Flip for left-facing
    m_sprite2.setScale(scale * direction, scale);
}

void FalconEnemy::update(float deltaTime) {
    m_animationTimer += deltaTime;
    m_shootCooldown -= deltaTime;

    // Move horizontally in the sky
    b2Vec2 velocity(-1.5f, 0.f); // horizontal speed
    m_body->SetLinearVelocity(velocity);

    // Sprite switching every 0.2s
    switchSprite(deltaTime);

    // Update projectiles
    for (auto& proj : m_projectiles)
        proj->update(deltaTime);

    m_projectiles.erase(std::remove_if(m_projectiles.begin(), m_projectiles.end(),
        [](const std::unique_ptr<Projectile>& p) { return !p->isAlive(); }),
        m_projectiles.end()
    );
}

void FalconEnemy::switchSprite(float deltaTime) {
    if (m_animationTimer > 0.2f) {
        m_useFirstSprite = !m_useFirstSprite;
        m_animationTimer = 0.f;
    }
}

void FalconEnemy::render(sf::RenderTarget& target) const {
    b2Vec2 pos = m_body->GetPosition();
    sf::Vector2f drawPos(pos.x * PPM, pos.y * PPM);

    sf::Sprite spriteToDraw = m_useFirstSprite ? m_sprite1 : m_sprite2;
    spriteToDraw.setPosition(drawPos); // ✅ no problem with copy
    target.draw(spriteToDraw);

    for (const auto& p : m_projectiles)
        p->render(target);
}

sf::FloatRect FalconEnemy::getBounds() const {
    return m_useFirstSprite ? m_sprite1.getGlobalBounds() : m_sprite2.getGlobalBounds();
}

sf::Vector2f FalconEnemy::getPosition() const
{
	b2Vec2 pos = m_body->GetPosition();
	return sf::Vector2f(pos.x * PPM, pos.y * PPM);
}

void FalconEnemy::shoot(TextureManager& textures) {
    if (m_shootCooldown > 0.f) return;

    b2Vec2 pos = m_body->GetPosition();

    auto proj = std::make_unique<Projectile>(
        *m_body->GetWorld(),
        pos.x,
        pos.y,
        0.f, // direction X
        textures,
        true // from enemy
    );

    proj->getBody()->SetLinearVelocity(b2Vec2(0.f, 6.f)); // downward shot
    m_projectiles.push_back(std::move(proj));
    m_shootCooldown = 2.f; // shoot every 2 seconds
}

bool FalconEnemy::isTimeToAppear(float elapsedTime) {
    return static_cast<int>(elapsedTime) % 30 == 0;
}

void FalconEnemy::kill()
{
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
    m_sprite1.setColor(sf::Color(255, 255, 255, 50));
    m_sprite2.setColor(sf::Color(255, 255, 255, 50));
}
