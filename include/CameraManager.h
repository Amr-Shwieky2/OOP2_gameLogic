#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Constants.h"

class CameraManager {
public:
    CameraManager();

    void initialize(float windowWidth, float windowHeight);
    void update(const Player& player);
    void setView(sf::RenderWindow& window);

    const sf::View& getCamera() const { return m_camera; }

private:
    sf::View m_camera;

    void updateCameraPosition(sf::Vector2f playerPos);
};
