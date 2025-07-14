#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Slider {
public:
    Slider(sf::Vector2f position, sf::Vector2f size, float minValue = 0.0f, float maxValue = 100.0f);

    void setValue(float value);
    float getValue() const { return m_value; }

    void setOnValueChanged(std::function<void(float)> callback) { m_onValueChanged = callback; }

    bool handleMouseMove(sf::Vector2f mousePos);
    bool handleMousePressed(sf::Vector2f mousePos);
    bool handleMouseReleased();

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    void setColors(sf::Color background, sf::Color fill, sf::Color handle);

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    float m_minValue;
    float m_maxValue;
    float m_value;

    sf::RectangleShape m_background;
    sf::RectangleShape m_fill;
    sf::CircleShape m_handle;

    bool m_isDragging = false;
    bool m_isHovered = false;

    sf::Color m_backgroundColor = sf::Color(139, 117, 83);         // Medium Brown 
    sf::Color m_fillColor = sf::Color(139, 69, 19);                // Dark Brown 
    sf::Color m_handleColor = sf::Color(210, 180, 140);            // Tan 

    std::function<void(float)> m_onValueChanged;

    void updateVisuals();
    float getValueFromPosition(float x) const;
    float getPositionFromValue() const;
};