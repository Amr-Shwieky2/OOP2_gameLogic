#pragma once

#include "StaticGameObject.h"
#include "ResourceManager.h"
#include <SFML/Graphics.hpp>

class Player; // Forward declaration

class Sea : public StaticGameObject {
public:
    Sea(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void update(float deltaTime) override;

    // الوظيفة الوحيدة المطلوبة
    void onPlayerContact(Player& player);

private:
    sf::Sprite m_sprite;
    TextureManager& m_textures;
    sf::Vector2f m_originalPosition;
    sf::Color m_originalColor;

    // متغير بسيط لمنع التصادم المتكرر
    bool m_canHarm = true;
    float m_cooldownTimer = 0.0f;
    static constexpr float HARM_COOLDOWN = 1.0f; // ثانية واحدة cooldown
};