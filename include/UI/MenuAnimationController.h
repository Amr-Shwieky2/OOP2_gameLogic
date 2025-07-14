#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Responsible for menu animations only
 * Single Responsibility: Animation effects and timing
 */
class MenuAnimationController {
public:
    MenuAnimationController();
    ~MenuAnimationController() = default;

    // Animation updates
    void update(float deltaTime);

    // Animation effects
    void resetAnimations();

    // Animation control
    void setAnimationSpeed(float speed) { m_animationSpeed = speed; }
    void enableGlowEffect(bool enable) { m_glowEnabled = enable; }

private:
    float m_animationTime = 0.0f;
    float m_animationSpeed = 1.0f;
    bool m_glowEnabled = true;

    // Animation calculations
    float calculateGlowIntensity() const;
    sf::Color calculateGlowColor() const;
};