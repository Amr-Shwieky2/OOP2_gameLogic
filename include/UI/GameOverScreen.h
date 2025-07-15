#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Dedicated screen for displaying the game over image.
 *        Mirrors WinningScreen behaviour for losing scenario.
 */
class GameOverScreen {
public:
    explicit GameOverScreen(const std::string& textureFile = "GameOver.png");
    void show(sf::RenderWindow& window);

private:
    std::string m_textureFile;
};
