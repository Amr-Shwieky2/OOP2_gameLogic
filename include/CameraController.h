#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.h"

class CameraController {
private:
    sf::View m_camera;
    sf::Vector2f m_targetPosition;
    float m_smoothing = 0.1f;
    float m_leadDistance = 100.0f;  // Distance to look ahead of player

public:
    CameraController(float width, float height);

    // Follow a target (usually the player)
    void followTarget(const sf::Vector2f& position, const sf::Vector2f& velocity = sf::Vector2f(0, 0));
    void setPosition(const sf::Vector2f& position);

    // Camera settings
    void setSmoothing(float smoothing) { m_smoothing = smoothing; }
    void setLeadDistance(float distance) { m_leadDistance = distance; }
    void setSize(float width, float height);

    // Camera bounds (to prevent going outside level)
    void setBounds(float minX, float maxX, float minY, float maxY);

    // Update camera position (call this every frame)
    void update(float deltaTime);

    // Get the camera view
    const sf::View& getView() const { return m_camera; }
    sf::Vector2f getCenter() const { return m_camera.getCenter(); }
    sf::Vector2f getSize() const { return m_camera.getSize(); }

private:
    bool m_hasBounds = false;
    float m_minX, m_maxX, m_minY, m_maxY;

    sf::Vector2f clampToBounds(const sf::Vector2f& position) const;
};