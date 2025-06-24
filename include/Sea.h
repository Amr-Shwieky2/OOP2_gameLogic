#pragma once

#include "StaticGameObject.h"
#include "ResourceManager.h"
#include <SFML/Graphics.hpp>

class Sea : public StaticGameObject {
public:
    Sea(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void update(float deltaTime) override;

    // للتصادم
    bool isConsumed() const { return m_consumed; }
    void consume();
    void setDeathEffect();

    // ✅ للإرجاع
    bool isRespawning() const { return m_respawning; }
    void startRespawn();

private:
    sf::Sprite m_sprite;
    TextureManager& m_textures;

    // المكان الأصلي
    sf::Vector2f m_originalPosition;

    // حالات Sea
    bool m_consumed = false;
    bool m_respawning = false;

    // أوقات التأثيرات
    float m_deathTimer = 0.0f;
    float m_respawnTimer = 0.0f;
    bool m_deathEffect = false;

    // إعدادات التوقيت
    static constexpr float DEATH_DURATION = 0.5f;    // مدة الاختفاء
    static constexpr float RESPAWN_TIME = 5.0f;      // وقت الإرجاع (5 ثوان)

    // الألوان
    sf::Color m_originalColor;
};