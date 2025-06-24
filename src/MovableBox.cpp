#include "MovableBox.h"
#include "Constants.h"
#include <cmath>
#include <iostream>

MovableBox::MovableBox(b2World& world, float x, float y, TileType type, TextureManager& textures)
    : m_world(world), m_body(nullptr) {

    sf::Texture& tex = textures.getResource("wooden_box.png");
    m_sprite.setTexture(tex);
    m_sprite.setOrigin(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f);
    createPhysicsBody(x, y);
    updateSpritePosition();
}

MovableBox::~MovableBox() {
    if (m_body) {
        m_world.DestroyBody(m_body);
    }
}

void MovableBox::createPhysicsBody(float x, float y) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;

    bodyDef.position.Set(
        (x + BOX_SIZE / 2.0f) / PPM,    // وسط العرض
        (y - BOX_SIZE / 2.0f) / PPM     // رفع الصندوق ليجلس على الأرض
    );

    // منع الدوران لحل مشكلة الحركة الدائرية
    bodyDef.fixedRotation = true;       // لا دوران

    //  تحسين المقاومة
    bodyDef.linearDamping = 0.2f;       // مقاومة أكثر للحركة الأفقية
    bodyDef.angularDamping = 0.9f;      // مقاومة قوية للدوران

    m_body = m_world.CreateBody(&bodyDef);

    // تعريف الشكل (مربع)
    b2PolygonShape boxShape;
    float halfSize = (BOX_SIZE / 2.0f) / PPM;
    boxShape.SetAsBox(halfSize, halfSize);

    //  تحسين الخصائص الفيزيائية
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = BOX_DENSITY;
    fixtureDef.friction = BOX_FRICTION;
    fixtureDef.restitution = BOX_RESTITUTION;

    m_body->CreateFixture(&fixtureDef);
    m_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
    m_body->SetAwake(true);
}

void MovableBox::update(float deltaTime) {
    updateSpritePosition();

    //  تحديد الحركة - يمين/يسار فقط
    b2Vec2 velocity = m_body->GetLinearVelocity();

    // تحديد السرعة الأفقية القصوى
    const float MAX_HORIZONTAL_SPEED = 5.0f;
    if (std::abs(velocity.x) > MAX_HORIZONTAL_SPEED) {
        velocity.x = (velocity.x > 0) ? MAX_HORIZONTAL_SPEED : -MAX_HORIZONTAL_SPEED;
    }

    // منع الحركة العمودية المفرطة (إلا الجاذبية)
    if (velocity.y > 2.0f) {
        velocity.y = 2.0f;
    }
    // إيقاف الحركة البطيئة
    if (std::abs(velocity.x) < 0.1f && std::abs(velocity.y) < 0.1f) {
        m_body->SetLinearVelocity(b2Vec2(0, 0));
    }
    else {
        m_body->SetLinearVelocity(velocity);
    }
}

void MovableBox::updateSpritePosition() {
    if (m_body) {
        b2Vec2 position = m_body->GetPosition();

        float spriteX = position.x * PPM;
        float spriteY = position.y * PPM;

        m_sprite.setPosition(spriteX, spriteY);
        m_sprite.setRotation(0.0f);
    }
}

void MovableBox::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect MovableBox::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void MovableBox::applyForce(float forceX, float forceY) {
    if (m_body) {
        m_body->SetAwake(true);
        m_body->ApplyForceToCenter(b2Vec2(forceX / PPM, forceY / PPM), true);
    }
}

bool MovableBox::isMoving() const {
    if (m_body) {
        b2Vec2 velocity = m_body->GetLinearVelocity();
        return (std::abs(velocity.x) > 0.1f || std::abs(velocity.y) > 0.1f);
    }
    return false;
}