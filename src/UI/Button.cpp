#include "Button.h"
#include <iostream>

Button::Button()
    : m_interaction(m_model), m_renderer(m_model, m_interaction) {

    // Initialize with safe defaults
    m_model.position = sf::Vector2f(0, 0);
    m_model.size = sf::Vector2f(100, 50);
    m_model.text = "Button";
    m_model.backgroundColor = sf::Color(100, 100, 100, 255);
    m_model.textColor = sf::Color::White;
    m_model.font = nullptr;
    m_model.texture = nullptr;

    std::cout << "Button: Initialized with safe defaults" << std::endl;
}

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text)
    : m_interaction(m_model), m_renderer(m_model, m_interaction) {

    // Validate and set parameters
    m_model.position = position;
    m_model.size = (size.x > 0 && size.y > 0) ? size : sf::Vector2f(100, 50);
    m_model.text = text.empty() ? "Button" : text;
    m_model.backgroundColor = sf::Color(100, 100, 100, 255);
    m_model.textColor = sf::Color::White;
    m_model.font = nullptr;
    m_model.texture = nullptr;

    std::cout << "Button: Initialized with parameters - " << text << std::endl;
}

void Button::setPosition(const sf::Vector2f& pos) {
    m_model.position = pos;
}

void Button::setSize(const sf::Vector2f& size) {
    // Validate size
    if (size.x > 0 && size.y > 0) {
        m_model.size = size;
    }
    else {
        std::cout << "Button: Warning - Invalid size provided, keeping current size" << std::endl;
    }
}

void Button::setText(const std::string& text) {
    m_model.text = text.empty() ? "Button" : text;
    std::cout << "Button: Text set to '" << m_model.text << "'" << std::endl;
}

void Button::setFont(const sf::Font& font) {
    m_model.font = &font;
    std::cout << "Button: Font set successfully" << std::endl;
}

void Button::setTexture(const sf::Texture* texture) {
    m_model.texture = texture;
    if (texture) {
        std::cout << "Button: Texture set successfully" << std::endl;
    }
}

void Button::setColors(const sf::Color& bg, const sf::Color& text) {
    m_model.backgroundColor = bg;
    m_model.textColor = text;
}

void Button::setCallback(std::function<void()> callback) {
    m_interaction.setCallback(callback);
}

void Button::handleMouseMove(const sf::Vector2f& mousePos) {
    m_interaction.handleMouseMove(mousePos);
}

bool Button::handleClick(const sf::Vector2f& mousePos) {
    return m_interaction.handleClick(mousePos); 
}

void Button::update(float deltaTime) {
    m_interaction.update(deltaTime);
}

void Button::render(sf::RenderWindow& window) {
    try {
        m_renderer.render(window);
    }
    catch (const std::exception& e) {
        std::cout << "Button: Render error - " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Button: Unknown render error" << std::endl;
    }
}

void Button::setButtonImage(const sf::Texture* texture) {
    m_model.texture = texture;
    if (texture) {
        std::cout << "Button: Button image texture set" << std::endl;
    }
}

void Button::setBackgroundColor(const sf::Color& color) {
    m_model.backgroundColor = color;
    std::cout << "Button: Background color set" << std::endl;
}

void Button::setTextColor(const sf::Color& color) {
    m_model.textColor = color;
    std::cout << "Button: Text color set" << std::endl;
}