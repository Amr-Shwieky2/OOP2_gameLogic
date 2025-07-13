// DarkLevelSystem.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <cmath>

class PlayerEntity;
class Entity;

/**
 * DarkLevelSystem - Handles darkness effects and lighting with shadow casting
 */
class DarkLevelSystem {
public:
    DarkLevelSystem();
    ~DarkLevelSystem();

    void initialize(sf::RenderWindow& window);
    void update(float dt, PlayerEntity* player);
    void render(sf::RenderWindow& window);

    // Darkness control
    void setDarknessLevel(float level); // 0.0 = bright, 1.0 = complete darkness
    float getDarknessLevel() const { return m_darknessLevel; }

    // Lighting effects
    void addLightSource(const sf::Vector2f& position, float radius, sf::Color color = sf::Color::White);
    void clearLightSources();

    // Shadow casting
    void registerObstacle(const sf::FloatRect& bounds);
    void clearObstacles();
    void setObstacles(const std::vector<sf::FloatRect>& obstacles);

    // Enable/disable the system
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void updateFlashlightDirection(const sf::Vector2f& playerPos, const sf::Vector2f& mousePos);

    // Flashlight settings
    void setFlashlightRange(float range) { m_flashlightRange = range; }
    void setFlashlightIntensity(float intensity) { m_flashlightIntensity = intensity; }
    void setFlashlightAngle(float angle) { m_flashlightAngle = angle; }

    void updatePlayerLight(PlayerEntity* player);

private:
    // Light source structure
    struct LightSource {
        sf::Vector2f position;
        float radius;
        sf::Color color;
        float intensity;
    };

    // Ray structure for shadow casting
    struct Ray {
        sf::Vector2f origin;
        sf::Vector2f direction;
        float distance;
        sf::Vector2f endPoint;
    };

    // Obstacle structure
    struct Obstacle {
        sf::FloatRect bounds;
        sf::ConvexShape shape;
    };

    // Intersection point data
    struct Intersection {
        sf::Vector2f point;
        float distance;
        bool valid;
    };

    // Shadow map rendering
    void renderShadowMap(const sf::Vector2f& lightPos, float radius, sf::RenderTexture& target);
    void castRays(const sf::Vector2f& origin, std::vector<Ray>& rays, float maxDistance);

    // Light rendering
    void renderFlashlight(sf::RenderWindow& window);
    void drawFlashlightCone(float intensity);
    void renderLightSources(sf::RenderTexture& target);

    // Shadow casting helpers
    Intersection rayObstacleIntersection(const Ray& ray, const Obstacle& obstacle);
    sf::Vector2f calculateIntersection(const sf::Vector2f& rayStart, const sf::Vector2f& rayEnd,
                                        const sf::Vector2f& segStart, const sf::Vector2f& segEnd);

    // Main system state
    bool m_enabled = false;
    float m_darknessLevel = 0.5f;

    // Lighting system
    std::vector<LightSource> m_lightSources;
    std::vector<Obstacle> m_obstacles;
    sf::Vector2f m_playerLightPos;
    float m_playerLightRadius = 150.0f;

    // Render targets
    std::unique_ptr<sf::RenderTexture> m_lightTexture;
    std::unique_ptr<sf::RenderTexture> m_darknessTexture;
    std::unique_ptr<sf::RenderTexture> m_shadowMapTexture;
    std::unique_ptr<sf::RenderTexture> m_flashlightTexture;
    std::unique_ptr<sf::RenderTexture> m_maskTexture;

    // Darkness overlay
    sf::RectangleShape m_darknessOverlay;
    sf::CircleShape m_lightCircle;

    // Animation timers
    float m_flickerTimer = 0.0f;
    float m_ambientTimer = 0.0f;

    // Flashlight properties
    sf::Vector2f m_flashlightDirection = {1.0f, 0.0f}; // Default right direction
    float m_flashlightAngle = 45.0f;                  // Beam angle in degrees
    float m_flashlightRange = 800.0f;                 // How far the light reaches
    float m_flashlightIntensity = 2.0f;               // Brightness of light
    bool m_flashlightOn = true;                       // Is flashlight on

    // Shader for advanced effects
    sf::Shader m_shadowShader;
    bool m_useShaders = false;

    // Shadow quality settings
    int m_rayCount = 360;                             // Number of rays for shadow casting
    float m_rayStep = 1.0f;                           // Angle step for ray casting
};