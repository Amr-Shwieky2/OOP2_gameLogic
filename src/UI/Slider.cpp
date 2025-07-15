#include "Slider.h"
#include <algorithm>

//-------------------------------------------------------------------------------------
Slider::Slider(sf::Vector2f position, sf::Vector2f size, float minValue, float maxValue)
    : m_position(position), m_size(size), m_minValue(minValue), m_maxValue(maxValue), m_value(minValue) {

    m_background.setPosition(position);
    m_background.setSize(size);
    m_background.setFillColor(m_backgroundColor);

    m_fill.setPosition(position);
    m_fill.setSize(sf::Vector2f(0, size.y));
    m_fill.setFillColor(m_fillColor);

    float handleRadius = size.y * 0.6f;
    m_handle.setRadius(handleRadius);
    m_handle.setOrigin(handleRadius, handleRadius);
    m_handle.setFillColor(m_handleColor);

    updateVisuals();
}
//-------------------------------------------------------------------------------------
void Slider::setValue(float value) {
    m_value = std::clamp(value, m_minValue, m_maxValue);
    updateVisuals();
    if (m_onValueChanged) {
        m_onValueChanged(m_value);
    }
}
//-------------------------------------------------------------------------------------
bool Slider::handleMouseMove(sf::Vector2f mousePos) {
    sf::FloatRect bounds(m_position.x, m_position.y - 10, m_size.x, m_size.y + 20);
    m_isHovered = bounds.contains(mousePos);

    if (m_isDragging) {
        float newValue = getValueFromPosition(mousePos.x);
        if (newValue != m_value) {
            setValue(newValue);
            if (m_onValueChanged) {
                m_onValueChanged(m_value);
            }
        }
        return true;
    }
    return m_isHovered;
}
//-------------------------------------------------------------------------------------
bool Slider::handleMousePressed(sf::Vector2f mousePos) {
    sf::FloatRect bounds(m_position.x, m_position.y - 10, m_size.x, m_size.y + 20);
    if (bounds.contains(mousePos)) {
        m_isDragging = true;
        float newValue = getValueFromPosition(mousePos.x);
        setValue(newValue);
        if (m_onValueChanged) {
            m_onValueChanged(m_value);
        }
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------
bool Slider::handleMouseReleased() {
    if (m_isDragging) {
        m_isDragging = false;
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------
void Slider::update(float deltaTime) {
    // Handle hover effect
    sf::Color handleColor = m_handleColor;
    if (m_isHovered || m_isDragging) {
        handleColor.r = std::min(255, (int)(handleColor.r * 1.2f));
        handleColor.g = std::min(255, (int)(handleColor.g * 1.2f));
        handleColor.b = std::min(255, (int)(handleColor.b * 1.2f));
    }
    m_handle.setFillColor(handleColor);
}
//-------------------------------------------------------------------------------------
void Slider::render(sf::RenderWindow& window) {
    window.draw(m_background);
    window.draw(m_fill);
    window.draw(m_handle);
}
//-------------------------------------------------------------------------------------
void Slider::setColors(sf::Color background, sf::Color fill, sf::Color handle) {
    m_backgroundColor = background;
    m_fillColor = fill;
    m_handleColor = handle;

    m_background.setFillColor(m_backgroundColor);
    m_fill.setFillColor(m_fillColor);
    m_handle.setFillColor(m_handleColor);
}
//-------------------------------------------------------------------------------------
void Slider::updateVisuals() {
    float progress = (m_value - m_minValue) / (m_maxValue - m_minValue);

    m_fill.setSize(sf::Vector2f(m_size.x * progress, m_size.y));

    float handleX = m_position.x + (m_size.x * progress);
    float handleY = m_position.y + (m_size.y * 0.5f);
    m_handle.setPosition(handleX, handleY);
}
//-------------------------------------------------------------------------------------
float Slider::getValueFromPosition(float x) const {
    float relativeX = x - m_position.x;
    float progress = std::max(0.0f, std::min(1.0f, relativeX / m_size.x));
    return m_minValue + (progress * (m_maxValue - m_minValue));
}
//-------------------------------------------------------------------------------------
float Slider::getPositionFromValue() const {
    float progress = (m_value - m_minValue) / (m_maxValue - m_minValue);
    return m_position.x + (m_size.x * progress);
}
//-------------------------------------------------------------------------------------