#pragma once
#include <SFML/Graphics.hpp>
#include "PlayerEntity.h"  
#include "Constants.h"

class CameraManager {
public:
    CameraManager();

    void initialize(float windowWidth, float windowHeight);
    void update(const PlayerEntity& player);  
    void setView(sf::RenderWindow& window);
    void setCenterPosition(const sf::Vector2f& center); 

    const sf::View& getCamera() const { return m_camera; }

private:
    sf::View m_camera;

    void updateCameraPosition(sf::Vector2f playerPos);
};