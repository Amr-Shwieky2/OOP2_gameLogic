﻿#include "SurpriseBoxScreen.h"
#include "ResourceManager.h"
#include <iostream>
#include <cmath>

SurpriseBoxScreen::SurpriseBoxScreen(sf::RenderWindow& window, TextureManagerType& textures)
    : m_window(window)
    , m_textures(textures)
    , m_gen(std::random_device{}())
{
    // إعداد الموقع وسط الشاشة
    m_boxPosition = sf::Vector2f(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);

    // محاولة تحميل النسيج
    bool textureLoaded = false;
    try {
        m_boxSprite.setTexture(m_textures.getResource("OpenBox.png"));
        sf::Vector2u textureSize = m_boxSprite.getTexture()->getSize();
        if (textureSize.x > 0 && textureSize.y > 0) {
            m_boxSprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
            m_useSprite = true;
            textureLoaded = true;
        }
    }
    catch (...) {
        std::cout << "Failed to load CloseBox" << std::endl;
    }

    // إعداد الخلفية
    m_background.setSize(sf::Vector2f(m_window.getSize()));
    try {
        m_background.setTexture(&m_textures.getResource("BoxBackground.png"));
    }
    catch (...) {
        m_background.setFillColor(sf::Color(50, 50, 100, 200)); // لون افتراضي
    }

    // إعداد fallback box
    m_fallbackBox.setSize(sf::Vector2f(100.0f, 100.0f));
    m_fallbackBox.setOrigin(50.0f, 50.0f);
    m_fallbackBox.setFillColor(sf::Color(139, 69, 19)); // بني
}

SurpriseGiftType SurpriseBoxScreen::showSurpriseBox() {
    m_isRunning = true;
    m_boxOpened = false;
    m_animationTimer = 0.0f;
    m_boxScale = 0.0f;
    m_particles.clear();
    m_giftImageLoaded = false;

    sf::Clock clock;

    while (m_isRunning && m_window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();
    }
    return m_selectedGift;
}

void SurpriseBoxScreen::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            m_isRunning = false;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space && !m_boxOpened) {
                m_boxOpened = true;
                m_animationTimer = 0.0f;
                createParticles();
                m_selectedGift = getRandomGiftType();
            }
            else if (event.key.code == sf::Keyboard::Enter && m_boxOpened) {
                m_isRunning = false;
            }
        }
    }
}

void SurpriseBoxScreen::update(float deltaTime) {
    updateBoxAnimation(deltaTime);
    updateParticles(deltaTime);

    // إنهاء تلقائي بعد 5 ثوان من فتح الصندوق
    if (m_boxOpened && m_animationTimer > 5.0f) {
        m_isRunning = false;
    }
}

void SurpriseBoxScreen::updateBoxAnimation(float deltaTime) {
    if (!m_boxOpened) {
        // رسوم متحركة للتنفس
        float breathe = 0.9f + 0.1f * sin(m_animationTimer * 3.0f);
        m_boxScale = breathe;
    }
    else {
        // رسوم متحركة للانفجار
        if (m_animationTimer < 1.0f) {
            float progress = m_animationTimer / 1.0f;
            m_boxScale = 1.0f + progress * 2.0f; // يكبر إلى 3x

            // اهتزاز
            float shake = sin(m_animationTimer * 50.0f) * 10.0f * (1.0f - progress);
            sf::Vector2f shakePos = m_boxPosition + sf::Vector2f(shake, 0);

            if (m_useSprite) {
                m_boxSprite.setPosition(shakePos);
                m_boxSprite.setScale(m_boxScale, m_boxScale);
            }
            else {
                m_fallbackBox.setPosition(shakePos);
                m_fallbackBox.setScale(m_boxScale, m_boxScale);
            }
        }
        else if (m_animationTimer >= 1.3f && !m_giftImageLoaded) {
            // تحميل صورة الهدية - الكود الأصلي
            std::string giftImages[] = {
                "LifeHeartGift.png",         // 0
                "SpeedGift.png",             // 1
                "ProtectiveShieldGift.png",  // 2
                "RareCoinGift.png",          // 3
                "ReverseMovementGift.png",   // 4
                "HeadwindStormGift.png",     // 5
                "MagneticGift.png"           // 6
            };

            try {
                m_giftSprite.setTexture(m_textures.getResource(giftImages[static_cast<int>(m_selectedGift)]));
                sf::Vector2u giftSize = m_giftSprite.getTexture()->getSize();
                m_giftSprite.setOrigin(giftSize.x / 2.0f, giftSize.y / 2.0f);
                m_giftSprite.setPosition(m_boxPosition.x, m_boxPosition.y - 200.0f);
                m_giftSprite.setScale(0.5f, 0.5f);
                m_giftImageLoaded = true;
            }
            catch (...) {
                std::cout << "Failed to load gift image" << std::endl;
                m_giftImageLoaded = true; // تجنب المحاولة مرة أخرى
            }
        }
        else {
            // اختفاء الصندوق
            float fade = std::max(0.0f, 2.0f - m_animationTimer);
            m_boxScale = 3.0f * fade;

            if (m_useSprite) {
                m_boxSprite.setPosition(m_boxPosition);
                m_boxSprite.setScale(m_boxScale, m_boxScale);
            }
            else {
                m_fallbackBox.setPosition(m_boxPosition);
                m_fallbackBox.setScale(m_boxScale, m_boxScale);
            }
        }
    }

    if (!m_useSprite) {
        m_fallbackBox.setPosition(m_boxPosition);
        m_fallbackBox.setScale(m_boxScale, m_boxScale);
    }
    else {
        m_boxSprite.setPosition(m_boxPosition);
        m_boxSprite.setScale(m_boxScale, m_boxScale);
    }
    m_animationTimer += deltaTime;
}

void SurpriseBoxScreen::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        particle.position += particle.velocity * deltaTime;
        particle.life -= deltaTime;

        // تلاشي الجسيم
        float alpha = std::max(0.0f, particle.life / particle.maxLife);
        particle.color.a = static_cast<sf::Uint8>(255 * alpha);

        // جاذبية للجسيمات
        particle.velocity.y += 200.0f * deltaTime;
    }

    // إزالة الجسيمات الميتة
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const ExplosionParticle& p) { return p.life <= 0.0f; }),
        m_particles.end()
    );
}

void SurpriseBoxScreen::createParticles() {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(100.0f, 400.0f);
    std::uniform_real_distribution<float> sizeDist(3.0f, 8.0f);
    std::uniform_int_distribution<int> colorDist(0, 6);

    sf::Color colors[] = {
        sf::Color(194, 178, 128), // رملي
        sf::Color(210, 180, 140), // بيج
        sf::Color(205, 133, 63),  // بني فاتح
        sf::Color(244, 164, 96),  // برتقالي رملي
        sf::Color(184, 134, 11),  // ذهبي غامق
        sf::Color(139, 69, 19),   // بني داكن
        sf::Color(160, 82, 45)    // أحمر طيني
    };

    for (int i = 0; i < 50; ++i) {
        ExplosionParticle particle;
        particle.position = m_boxPosition;

        float angle = angleDist(m_gen);
        float speed = speedDist(m_gen);
        particle.velocity = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);

        particle.color = colors[colorDist(m_gen)];
        particle.size = sizeDist(m_gen);
        particle.life = particle.maxLife = 10.0f;

        m_particles.push_back(particle);
    }
}

void SurpriseBoxScreen::render() {
    m_window.clear();

    // رسم الخلفية
    m_window.draw(m_background);

    // رسم الجسيمات
    for (const auto& particle : m_particles) {
        sf::CircleShape circle(particle.size);
        circle.setOrigin(particle.size, particle.size);
        circle.setPosition(particle.position);
        circle.setFillColor(particle.color);
        m_window.draw(circle);

        // رسم صورة الهدية إذا تم تحميلها (بعد الانفجار)
        if (m_giftImageLoaded) {
            m_window.draw(m_giftSprite);
        }
    }
    // رسم الصندوق
    if (m_boxScale > 0.1f) {
        if (m_useSprite) {
            m_window.draw(m_boxSprite);
        }
        else {
            m_window.draw(m_fallbackBox);
        }
    }
    
    m_window.display();
}

SurpriseGiftType SurpriseBoxScreen::getRandomGiftType() {
    std::uniform_int_distribution<int> dist(0, 6);
    return static_cast<SurpriseGiftType>(dist(m_gen));
}

void SurpriseBoxScreen::setGiftCallback(std::function<void(std::unique_ptr<GameObject>)> callback) {
    m_giftCallback = callback;
}