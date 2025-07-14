#include "ButtonInteraction.h"

ButtonInteraction::ButtonInteraction(ButtonModel& model)
    : m_model(model) {
}

void ButtonInteraction::handleMouseMove(const sf::Vector2f& mousePos) {
    bool wasHovered = m_isHovered;
    m_isHovered = m_model.getBounds().contains(mousePos);

    if (m_isHovered && !wasHovered) {
        m_targetScale = 1.1f;
    }
    else if (!m_isHovered && wasHovered) {
        m_targetScale = 1.0f;
    }
}

bool ButtonInteraction::handleClick(const sf::Vector2f& mousePos) {
    if (m_model.getBounds().contains(mousePos)) {
        if (m_callback) {
            m_callback();
        }
        return true;
    }
    return false;
}

void ButtonInteraction::setCallback(std::function<void()> callback) {
    m_callback = callback;
}

bool ButtonInteraction::isHovered() const {
    return m_isHovered;
}

float ButtonInteraction::getHoverScale() const {
    return m_hoverScale;
}

void ButtonInteraction::update(float deltaTime) {
    updateHoverEffects(deltaTime);
}

void ButtonInteraction::updateHoverEffects(float deltaTime) {
    float speed = 5.0f;

    if (m_hoverScale < m_targetScale) {
        m_hoverScale += speed * deltaTime;
        if (m_hoverScale > m_targetScale) m_hoverScale = m_targetScale;
    }
    else if (m_hoverScale > m_targetScale) {
        m_hoverScale -= speed * deltaTime;
        if (m_hoverScale < m_targetScale) m_hoverScale = m_targetScale;
    }
}
