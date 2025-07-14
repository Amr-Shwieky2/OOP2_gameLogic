#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "ButtonModel.h"
#include "ButtonInteraction.h"
#include "ButtonRenderer.h"

class Button {
public:
    Button();
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text);

    void setPosition(const sf::Vector2f& pos);
    void setSize(const sf::Vector2f& size);
    void setText(const std::string& text);
    void setFont(const sf::Font& font);
    void setTexture(const sf::Texture* texture);
    void setColors(const sf::Color& bg, const sf::Color& text);
    void setCallback(std::function<void()> callback);

    void handleMouseMove(const sf::Vector2f& mousePos);
    bool handleClick(const sf::Vector2f& mousePos);  
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    bool isMouseOver(const sf::Vector2f& mousePos) const {
        return m_model.getBounds().contains(mousePos);
    }

    void setButtonImage(const sf::Texture* texture);
    void setBackgroundColor(const sf::Color& color);  
    void setTextColor(const sf::Color& color);

private:
    ButtonModel m_model;
    ButtonInteraction m_interaction;
    ButtonRenderer m_renderer;
};