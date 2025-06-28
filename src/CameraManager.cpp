// ==========================================
// CameraManager.cpp - Updated Implementation  
// ==========================================

#include "CameraManager.h"
#include "Transform.h"
#include <algorithm>

CameraManager::CameraManager() = default;

void CameraManager::initialize(float windowWidth, float windowHeight) {
    m_camera.setSize(windowWidth, windowHeight);
    m_camera.setCenter(windowWidth / 2.f, windowHeight / 2.f);
}

void CameraManager::update(const PlayerEntity& player) {
    // Get player position from Transform component
    auto* transform = player.getComponent<Transform>();
    if (transform) {
        updateCameraPosition(transform->getPosition());
    }
}

void CameraManager::setCenterPosition(const sf::Vector2f& center) {
    m_camera.setCenter(center);
}

void CameraManager::updateCameraPosition(sf::Vector2f playerPos) {
    float newX = std::max(playerPos.x, WINDOW_WIDTH / 2.f);
    m_camera.setCenter(newX, WINDOW_HEIGHT / 2.f);
}

void CameraManager::setView(sf::RenderWindow& window) {
    window.setView(m_camera);
}