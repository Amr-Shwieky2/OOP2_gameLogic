// CameraController.cpp (مُصحح)
#include "CameraController.h"
#include <algorithm>
#include <cmath>

CameraController::CameraController(float width, float height) {
    m_camera.setSize(width, height);
    m_camera.setCenter(width / 2.0f, height / 2.0f);
    m_targetPosition = m_camera.getCenter();
}

void CameraController::followTarget(const sf::Vector2f& position, const sf::Vector2f& velocity) {
    // ✅ إزالة look-ahead المفرط الذي يسبب الاهتزاز
    sf::Vector2f lookAhead(0, 0);

    // ✅ look-ahead أقل وأكثر سلاسة
    float velocityThreshold = 50.0f; // الحد الأدنى للسرعة
    if (std::abs(velocity.x) > velocityThreshold) {
        float maxLookAhead = 50.0f; // مسافة أقل
        float velocityFactor = std::min(std::abs(velocity.x) / 200.0f, 1.0f);
        lookAhead.x = (velocity.x > 0 ? maxLookAhead : -maxLookAhead) * velocityFactor;
    }

    // Set target position
    m_targetPosition = position + lookAhead;

    // Ensure minimum X position (don't go behind start of level)
    float minCameraX = m_camera.getSize().x / 2.0f;
    m_targetPosition.x = std::max(m_targetPosition.x, minCameraX);

    // Keep Y position centered on screen height
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
}

void CameraController::update(float deltaTime) {
    sf::Vector2f currentPos = m_camera.getCenter();

    // ✅ تحسين smoothing - أقل اهتزاز
    float distance = std::sqrt(std::pow(m_targetPosition.x - currentPos.x, 2) +
        std::pow(m_targetPosition.y - currentPos.y, 2));

    // ✅ إيقاف الحركة إذا كانت المسافة صغيرة جداً
    if (distance < 1.0f) {
        return; // لا تحرك الكاميرا
    }

    // ✅ smoothing متدرج حسب المسافة
    float dynamicSmoothing = std::min(0.1f + (distance / 1000.0f), 0.3f);

    // Smooth interpolation towards target
    sf::Vector2f diff = m_targetPosition - currentPos;
    sf::Vector2f newPos = currentPos + diff * dynamicSmoothing;

    // Apply bounds if set
    if (m_hasBounds) {
        newPos = clampToBounds(newPos);
    }

    m_camera.setCenter(newPos);
}

sf::Vector2f CameraController::clampToBounds(const sf::Vector2f& position) const {
    sf::Vector2f clamped = position;

    float halfWidth = m_camera.getSize().x / 2.0f;
    float halfHeight = m_camera.getSize().y / 2.0f;

    // Clamp X
    clamped.x = std::max(clamped.x, m_minX + halfWidth);
    clamped.x = std::min(clamped.x, m_maxX - halfWidth);

    // Clamp Y
    clamped.y = std::max(clamped.y, m_minY + halfHeight);
    clamped.y = std::min(clamped.y, m_maxY - halfHeight);

    return clamped;
}