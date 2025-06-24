#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <memory>
#include <random>
#include "ResourceManager.h"

// Forward declarations
class GameObject;
using TextureManagerType = ResourceManager<sf::Texture>;

// تعريف SurpriseGiftType هنا بدلاً من الاستيراد
enum class SurpriseGiftType {
    LifeHeart,
    SpeedBoost,
    Shield,
    RareCoin,
    ReverseMovement,
    HeadwindStorm,
    Magnetic
};

// جسيم الانفجار
struct ExplosionParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float size;
    float life;
    float maxLife;
};

class SurpriseBoxScreen {
public:
    SurpriseBoxScreen(sf::RenderWindow& window, TextureManagerType& textures);
    ~SurpriseBoxScreen() = default;

    // عرض شاشة الصندوق
    SurpriseGiftType showSurpriseBox();

    // callback لإنشاء الهدايا
    void setGiftCallback(std::function<void(std::unique_ptr<GameObject>)> callback);

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    // رسوم متحركة
    void updateBoxAnimation(float deltaTime);
    void updateParticles(float deltaTime);
    void createParticles();

    // اختيار هدية عشوائية
    SurpriseGiftType getRandomGiftType();

    // الأعضاء
    sf::RenderWindow& m_window;
    TextureManagerType& m_textures;

    // حالة الشاشة
    bool m_isRunning = false;
    bool m_boxOpened = false;
    SurpriseGiftType m_selectedGift;

    // الرسوم المتحركة
    sf::Sprite m_boxSprite;
    sf::RectangleShape m_fallbackBox;
    sf::Sprite m_giftSprite;
    bool m_giftImageLoaded = false;
    bool m_useSprite = true;

    float m_animationTimer = 0.0f;
    float m_boxScale = 0.0f;
    sf::Vector2f m_boxPosition;

    // الجسيمات
    std::vector<ExplosionParticle> m_particles;

    // خلفية
    sf::RectangleShape m_background;

    // مولد عشوائي
    std::mt19937 m_gen;

    // callback
    std::function<void(std::unique_ptr<GameObject>)> m_giftCallback;
};