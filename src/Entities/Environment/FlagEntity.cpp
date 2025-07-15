#include "FlagEntity.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "Constants.h"
//-------------------------------------------------------------------------------------
FlagEntity::FlagEntity(IdType id, b2World& world, float x, float y, TextureManager& textures)
    : Entity(id), m_completed(false) {  
    setupComponents(world, x, y, textures);
}
//-------------------------------------------------------------------------------------
void FlagEntity::setupComponents(b2World& world, float x, float y, TextureManager& textures) {
    float centerX = x + TILE_SIZE / 2.f;
    float centerY = y + TILE_SIZE / 2.f;

    addComponent<Transform>(sf::Vector2f(centerX, centerY));

    auto* physics = addComponent<PhysicsComponent>(world, b2_staticBody);
    physics->createBoxShape(TILE_SIZE / 2.f, TILE_SIZE);
    physics->setPosition(centerX, centerY);

    auto* render = addComponent<RenderComponent>();
    render->setTexture(textures.getResource("redflag.png"));
    auto& sprite = render->getSprite();
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    sprite.setPosition(centerX, centerY);

    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Collectible);
}
//-------------------------------------------------------------------------------------
void FlagEntity::onPlayerReach() {
}
//-------------------------------------------------------------------------------------
bool FlagEntity::isCompleted() const {
    return m_completed;
}
//-------------------------------------------------------------------------------------
void FlagEntity::setCompleted(bool completed) {
    m_completed = completed;
}
//-------------------------------------------------------------------------------------