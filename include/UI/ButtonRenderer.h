#pragma once
#include <SFML/Graphics.hpp>
#include "ButtonModel.h"
#include "ButtonInteraction.h"

class ButtonRenderer {
public:
    ButtonRenderer(ButtonModel& model, ButtonInteraction& interaction);

    void render(sf::RenderWindow& window);

private:
    ButtonModel& m_model;
    ButtonInteraction& m_interaction;

    sf::RectangleShape m_background;
    sf::Sprite m_sprite;
    sf::Text m_text;

    void updateGraphics();
    void updateTextPosition();
};
