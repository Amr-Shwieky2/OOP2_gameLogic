#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <memory>
#include <random>
#include "ResourceManager.h"

// Forward declarations
using TextureManagerType = ResourceManager<sf::Texture>;

// Gift types that can be obtained from surprise box
enum class SurpriseGiftType {
    LifeHeart,
    SpeedBoost,
    Shield,
    RareCoin,
    ReverseMovement,
    HeadwindStorm,
    Magnetic
};

// Particle for explosion effect
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

    // Show the surprise box screen and return the selected gift
    SurpriseGiftType showSurpriseBox();

private:
    // Event handling
    void handleEvents();
    void update(float deltaTime);
    void render();

    // Animation updates
    void updateBoxAnimation(float deltaTime);
    void updateParticles(float deltaTime);
    void createParticles();

    // Gift selection
    SurpriseGiftType getRandomGiftType();
    std::string getGiftName(SurpriseGiftType type) const;
    sf::Color getGiftColor(SurpriseGiftType type) const;

    // Drawing helpers
    void drawInstructions();
    void drawGiftInfo();

    // Member variables
    sf::RenderWindow& m_window;
    TextureManagerType& m_textures;

    // Screen state
    bool m_isRunning = false;
    bool m_boxOpened = false;
    bool m_showingGift = false;
    bool m_canContinue = false;
    SurpriseGiftType m_selectedGift;

    // Animation state
    sf::Sprite m_boxSprite;
    sf::RectangleShape m_fallbackBox;
    sf::Sprite m_giftSprite;
    bool m_giftImageLoaded = false;
    bool m_useSprite = true;

    float m_animationTimer = 0.0f;
    float m_boxScale = 1.0f;
    float m_giftScale = 0.0f;
    sf::Vector2f m_boxPosition;
    sf::Vector2f m_giftPosition;

    // Particles
    std::vector<ExplosionParticle> m_particles;

    // UI elements
    sf::RectangleShape m_background;
    sf::RectangleShape m_overlay;
    sf::Font m_font;
    sf::Text m_instructionText;
    sf::Text m_giftNameText;
    sf::Text m_giftDescriptionText;

    // Random generator
    std::mt19937 m_gen;

    // Animation phases
    enum class AnimationPhase {
        WaitingToOpen,
        Opening,
        Exploding,
        ShowingGift,
        WaitingToContinue
    };
    AnimationPhase m_phase = AnimationPhase::WaitingToOpen;
};