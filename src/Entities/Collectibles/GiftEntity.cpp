#include "GiftEntity.h"
#include "Transform.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "ResourceManager.h"

GiftEntity::GiftEntity(IdType id, GiftType type, float x, float y, TextureManager& textures)
    : Entity(id)
    , m_giftType(type) {
    setupComponents(x, y, textures);
}

void GiftEntity::setupComponents(float x, float y, TextureManager& textures) {
    // Add transform
    addComponent<Transform>(sf::Vector2f(x, y));

    // Add rendering
    auto* render = addComponent<RenderComponent>();
    std::string textureName = getTextureNameForType(m_giftType);
    render->setTexture(textures.getResource(textureName));
    auto& sprite = render->getSprite();
    sprite.setScale(0.2f, 0.2f);
    auto bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    // Add collision
    addComponent<CollisionComponent>(CollisionComponent::CollisionType::Collectible);
}

std::string GiftEntity::getTextureNameForType(GiftType type) const {
    switch (type) {
    case GiftType::LifeHeart: return "LifeHeartGift.png";
    case GiftType::SpeedBoost: return "SpeedGift.png";
    case GiftType::Shield: return "ProtectiveShieldGift.png";
    case GiftType::RareCoin: return "RareCoinGift.png";
    case GiftType::ReverseMovement: return "ReverseMovementGift.png";
    case GiftType::HeadwindStorm: return "HeadwindStormGift.png";
    case GiftType::Magnetic: return "MagneticGift.png";
    default: return "Coin.png";
    }
}

void GiftEntity::collect() {
    m_collected = true;
    setActive(false);
}