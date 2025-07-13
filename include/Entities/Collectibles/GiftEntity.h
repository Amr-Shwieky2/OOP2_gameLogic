#pragma once
#include "Entity.h"
#include <string>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <type_traits>
#include <functional>
#include "ResourceManager.h"


/**
 * GiftEntity - Single class for all gift types
 * Replaces SpeedGift, LifeHeartGift, etc.
 */
class GiftEntity : public Entity {
public:
    enum class GiftType {
        LifeHeart,
        SpeedBoost,
        Shield,
        RareCoin,
        ReverseMovement,
        HeadwindStorm,
        Magnetic
    };
    
    GiftEntity(IdType id, GiftType type, float x, float y, TextureManager& textures);
    
    GiftType getGiftType() const { return m_giftType; }
    void collect();
    bool isCollected() const { return m_collected; }
    
private:
    void setupComponents(float x, float y, TextureManager& textures);
    std::string getTextureNameForType(GiftType type) const;
    
    GiftType m_giftType;
    bool m_collected = false;
};