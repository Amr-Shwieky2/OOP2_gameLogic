#pragma once
#include "Entity.h"
#include <Box2D/Box2D.h>
#include "ResourceManager.h"
#include "ResourcePaths.h"

/**
 * WellEntity - Portal to dark underground levels
 */
class WellEntity : public Entity {
public:
    WellEntity(IdType id, b2World& world, float x, float y, TextureManager& textures);

    void onPlayerEnter();
    bool isActivated() const { return m_activated; }
    void setActivated(bool activated) { m_activated = activated; }

    // Get the target level for this well
    const std::string& getTargetLevel() const { return m_targetLevel; }
    void setTargetLevel(const std::string& level) { m_targetLevel = level; }

    static bool isLevelChangeRequested() { return s_levelChangeRequested; }
    static std::string getTargetLevelName() { return s_targetLevelName; }
    static void requestLevelChange(const std::string& levelName);
    static void clearLevelChangeRequest();

private:
    void setupComponents(b2World& world, float x, float y, TextureManager& textures);
    void updateAnimation(float dt);

    bool m_activated = false;
    std::string m_targetLevel = ResourcePaths::DARK_LEVEL; // Default underground level

    // Animation variables
    float m_animationTimer = 0.0f;
    float m_waterLevel = 0.0f;
    bool m_bubbleEffect = false;

    static bool s_levelChangeRequested;
    static std::string s_targetLevelName;
};