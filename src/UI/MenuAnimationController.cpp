#include "../../include/UI/MenuAnimationController.h"
#include <cmath>
#include <iostream>

MenuAnimationController::MenuAnimationController() {
    std::cout << "MenuAnimationController: Initialized for animation control only" << std::endl;
}

void MenuAnimationController::update(float deltaTime) {
    m_animationTime += deltaTime * m_animationSpeed;
}

void MenuAnimationController::resetAnimations() {
    m_animationTime = 0.0f;
}

float MenuAnimationController::calculateGlowIntensity() const {
    return 0.7f + 0.3f * std::sin(m_animationTime * 2.0f);
}

sf::Color MenuAnimationController::calculateGlowColor() const {
    return sf::Color::Yellow; 
}
