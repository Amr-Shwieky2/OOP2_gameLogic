#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "ButtonModel.h"

class ButtonInteraction {
public:
    ButtonInteraction(ButtonModel& model);

    void handleMouseMove(const sf::Vector2f& mousePos);
    bool handleClick(const sf::Vector2f& mousePos);
    void update(float deltaTime);

    void setCallback(std::function<void()> callback);
    bool isHovered() const;
    float getHoverScale() const;

private:
    ButtonModel& m_model;
    std::function<void()> m_callback;

    bool m_isHovered = false;
    float m_hoverScale = 1.0f;
    float m_targetScale = 1.0f;

    void updateHoverEffects(float deltaTime);
};
