#include "CameraController.h"
#include <algorithm>
#include <cmath>
#include <iostream> 

CameraController::CameraController(float width, float height)
    : m_minX(0.0f), m_maxX(0.0f), m_minY(0.0f), m_maxY(0.0f) {
    m_camera.setSize(width, height);
    m_camera.setCenter(width / 2.0f, height / 2.0f);
    m_targetPosition = m_camera.getCenter();
}

void CameraController::followTarget(const sf::Vector2f& position, const sf::Vector2f& velocity) {
    sf::Vector2f lookAhead(0, 0);

    float velocityThreshold = 10.0f;
    if (std::abs(velocity.x) > velocityThreshold) {
        float maxLookAhead = 50.0f;
        float velocityFactor = std::min(std::abs(velocity.x) / 200.0f, 1.0f);
        lookAhead.x = (velocity.x > 0 ? maxLookAhead : -maxLookAhead) * velocityFactor;
    }

    m_targetPosition = position + lookAhead;

    float minCameraX = m_camera.getSize().x / 2.0f;
    m_targetPosition.x = std::max(m_targetPosition.x, minCameraX);

    m_targetPosition.y = WINDOW_HEIGHT / 2.0f;
}

void CameraController::setPosition(const sf::Vector2f& position) {
    m_targetPosition = position;
    m_camera.setCenter(position);
}

void CameraController::setSize(float width, float height) {
    m_camera.setSize(width, height);
}

void CameraController::setBounds(float minX, float maxX, float minY, float maxY) {
    m_minX = minX;
    m_maxX = maxX;
    m_minY = minY;
    m_maxY = maxY;
    m_hasBounds = true;

    std::cout << "Camera Bounds set: X[" << minX << ", " << maxX << "], Y[" << minY << ", " << maxY << "]\n";
}

static sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t) {
    return a + (b - a) * t;
}

void CameraController::update(float) {
    sf::Vector2f currentPos = m_camera.getCenter();
    float distance = std::hypot(m_targetPosition.x - currentPos.x, m_targetPosition.y - currentPos.y);

    if (distance < 1.0f) {
        m_camera.setCenter(m_targetPosition);
        return;
    }

    float dynamicSmoothing = std::min(0.1f + (distance / 1000.0f), 0.3f);
    sf::Vector2f newPos = lerp(currentPos, m_targetPosition, dynamicSmoothing);

    if (m_hasBounds)
        newPos = clampToBounds(newPos);

    m_camera.setCenter(newPos);
}

sf::Vector2f CameraController::clampToBounds(const sf::Vector2f& position) const {
    sf::Vector2f clamped = position;
    float halfWidth = m_camera.getSize().x / 2.0f;
    float halfHeight = m_camera.getSize().y / 2.0f;

    clamped.x = std::max(clamped.x, m_minX + halfWidth);
    clamped.x = std::min(clamped.x, m_maxX - halfWidth);
    clamped.y = std::max(clamped.y, m_minY + halfHeight);
    clamped.y = std::min(clamped.y, m_maxY - halfHeight);

    return clamped;
}