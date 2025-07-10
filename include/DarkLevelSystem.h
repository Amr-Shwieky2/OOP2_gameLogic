// DarkLevelSystem.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class PlayerEntity;

/**
 * DarkLevelSystem - Handles darkness effects and lighting
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

    // Enable/disable the system
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void updateFlashlightDirection(const sf::Vector2f& playerPos, const sf::Vector2f& mousePos);
    void toggleFlashlight();
    void renderFlashlight(sf::RenderWindow& window);
    void updateBattery(float dt);

    // إعدادات الكشاف
    void setFlashlightRange(float range) { m_flashlightRange = range; }
    void setFlashlightIntensity(float intensity) { m_flashlightIntensity = intensity; }
    float getBatteryLevel() const { return m_batteryLevel; }

private:
    void updatePlayerLight(PlayerEntity* player);
    void drawFlashlightCone(float intensity);
    void renderLightSources(sf::RenderTexture& target);

    struct LightSource {
        sf::Vector2f position;
        float radius;
        sf::Color color;
        float intensity;
    };

    bool m_enabled = false;
    float m_darknessLevel = 0.8f; // Default darkness level

    // Lighting system
    std::vector<LightSource> m_lightSources;
    sf::Vector2f m_playerLightPos;
    float m_playerLightRadius = 150.0f;

    // Render targets for lighting effects
    std::unique_ptr<sf::RenderTexture> m_lightTexture;
    std::unique_ptr<sf::RenderTexture> m_darknessTexture;

    // Darkness overlay
    sf::RectangleShape m_darknessOverlay;
    sf::CircleShape m_lightCircle;

    // Animation
    float m_flickerTimer = 0.0f;
    float m_ambientTimer = 0.0f;

    // متغيرات الكشاف
    sf::Vector2f m_flashlightDirection;    // اتجاه الكشاف
    float m_flashlightAngle;               // زاوية الكشاف
    float m_flashlightRange;               // مدى الكشاف
    float m_flashlightIntensity;           // قوة الكشاف
    bool m_flashlightOn;                   // حالة الكشاف (مشتعل/مطفأ)

    // بطارية الكشاف
    float m_batteryLevel;                  // مستوى البطارية (0-1)
    float m_batteryDrainRate;              // معدل استنزاف البطارية

    // تأثيرات إضافية
    bool m_lowBatteryWarning;              // تحذير البطارية المنخفضة

    // الرندر تكستشرز
    std::unique_ptr<sf::RenderTexture> m_flashlightTexture;
    std::unique_ptr<sf::RenderTexture> m_maskTexture;

    // الشيدرز (اختياري للتأثيرات المتقدمة)
    sf::Shader m_flashlightShader;
};