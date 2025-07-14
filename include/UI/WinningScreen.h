#pragma once
#include <SFML/Graphics.hpp>
#include <string>

/**
 * @brief Dedicated screen for displaying the winning image.
 *        This class handles only the winning screen logic
 *        to keep GameSession free from UI responsibilities.
 */
class WinningScreen {
public:
    explicit WinningScreen(const std::string& textureFile = "winning.png");
    void show(sf::RenderWindow& window);

private:
    std::string m_textureFile;
};
