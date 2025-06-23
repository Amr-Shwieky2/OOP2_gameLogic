#include "MovableBox.h"
#include "Constants.h"
#include <cmath>
#include <iostream>

MovableBox::MovableBox(b2World& world, float x, float y, TileType type, TextureManager& textures)
    : m_world(world), m_body(nullptr) {

    // تحميل texture الصندوق الخشبي
    sf::Texture& tex = textures.getResource("wooden_box.jpeg");
    m_sprite.setTexture(tex);
    m_sprite.setOrigin(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f);

    // إنشاء الجسم الفيزيائي
    createPhysicsBody(x, y);

    // تحديث موقع الـ sprite
    updateSpritePosition();
}

MovableBox::~MovableBox() {
    if (m_body) {
        m_world.DestroyBody(m_body);
    }
}

void MovableBox::createPhysicsBody(float x, float y) {
    // تعريف الجسم
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody; // جسم متحرك
    bodyDef.position.Set(x / PPM, (y - BOX_SIZE / 4) / PPM); // رفع الصندوق شوي عن الأرض
    bodyDef.fixedRotation = false; // يقدر يدور
    bodyDef.linearDamping = 0.1f;  // كانت 0.3f - قلل المقاومة
    bodyDef.angularDamping = 0.1f; // إضافة مقاومة دوران قليلة

    m_body = m_world.CreateBody(&bodyDef);

    // تعريف الشكل (مربع)
    b2PolygonShape boxShape;
    float halfSize = (BOX_SIZE / 2.0f) / PPM;
    boxShape.SetAsBox(halfSize, halfSize);

    // تعريف الخصائص الفيزيائية
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = BOX_DENSITY;
    fixtureDef.friction = BOX_FRICTION;
    fixtureDef.restitution = BOX_RESTITUTION;

    // إنشاء الـ fixture
    m_body->CreateFixture(&fixtureDef);

    // ربط المؤشر للكائن الحالي (للـ collision detection)
    m_body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);

    // تأكد إن الجسم مستيقظ
    m_body->SetAwake(true);
}

void MovableBox::update(float deltaTime) {
    // Debug: تأكد إن الدالة بتتنادى
    static int updateCount = 0;
    if (updateCount % 60 == 0) { // كل 60 frame
       // std::cout << "MovableBox::update() called - count: " << updateCount << std::endl;
    }
    updateCount++;

    // تحديث موقع الـ sprite بناءً على موقع الجسم الفيزيائي
    updateSpritePosition();

    // إيقاف الحركة إذا كانت بطيئة جداً (عشان نوفر أداء)
    b2Vec2 velocity = m_body->GetLinearVelocity();
    if (std::abs(velocity.x) < 0.1f && std::abs(velocity.y) < 0.1f) {
        m_body->SetLinearVelocity(b2Vec2(0, 0));
    }
}

void MovableBox::updateSpritePosition() {
    if (m_body) {
        b2Vec2 position = m_body->GetPosition();
        float angle = m_body->GetAngle();

        float spriteX = position.x * PPM;
        float spriteY = position.y * PPM;

        m_sprite.setPosition(spriteX, spriteY);
        m_sprite.setRotation(angle * 180.0f / b2_pi);

        // Debug: طباعة موقع الـ sprite
        //std::cout << "Sprite position updated: " << spriteX << ", " << spriteY << std::endl;
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
        // إيقاظ الجسم إذا كان نايم
        m_body->SetAwake(true);

        // تطبيق القوة
        m_body->ApplyForceToCenter(b2Vec2(forceX / PPM, forceY / PPM), true);

        // إضافة معلومات debug
        std::cout << "Force applied: " << forceX << ", Body position: "
            << m_body->GetPosition().x * PPM << ", "
            << m_body->GetPosition().y * PPM << std::endl;
    }
}

bool MovableBox::isMoving() const {
    if (m_body) {
        b2Vec2 velocity = m_body->GetLinearVelocity();
        return (std::abs(velocity.x) > 0.1f || std::abs(velocity.y) > 0.1f);
    }
    return false;
}