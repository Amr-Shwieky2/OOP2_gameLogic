#include "ProjectileEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"

ProjectileEntity::ProjectileEntity(IdType id, b2World& world, float x, float y,
                                   float dirX, float dirY, TextureManager& textures)
    : Entity(id) {
    setupComponents(world, x, y, dirX, dirY, textures);
}

void ProjectileEntity::setupComponents(b2World& world, float x, float y,
                                       float dirX, float dirY, TextureManager& textures) {
    addComponent<Transform>(sf::Vector2f(x, y));

    auto* physics = addComponent<PhysicsComponent>(world, b2_dynamicBody);
    physics->createCircleShape(PROJECTILE_RADIUS * PPM);
    physics->setPosition(x, y);
    physics->setVelocity(dirX * PROJECTILE_SPEED, dirY * PROJECTILE_SPEED);
    if (auto* body = physics->getBody()) {
        body->SetBullet(true);
        body->SetGravityScale(0.f);
    }

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("Bullet.png"));
    auto& sprite = render->getSprite();
    sprite.setScale(0.05f, 0.05f);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Projectile);
}

void ProjectileEntity::update(float dt) {
    Entity::update(dt);
    m_lifetime -= dt;
    if (m_lifetime <= 0.f) {
        setActive(false);
    }
}
