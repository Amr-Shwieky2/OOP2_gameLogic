#include "DarkLevelSystem.h"
#include "PlayerEntity.h"
#include "Transform.h"
#include "Constants.h"
#include "Entity.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <PhysicsComponent.h>
#include <numeric>

// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DarkLevelSystem::DarkLevelSystem() {
    m_lightCircle.setRadius(m_playerLightRadius);
    m_lightCircle.setOrigin(m_playerLightRadius, m_playerLightRadius);
    m_lightCircle.setFillColor(sf::Color::Transparent);
    m_lightCircle.setOutlineThickness(0);
}

DarkLevelSystem::~DarkLevelSystem() = default;

void DarkLevelSystem::initialize(sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();

    // Create all necessary render textures
    m_lightTexture = std::make_unique<sf::RenderTexture>();
    m_darknessTexture = std::make_unique<sf::RenderTexture>();
    m_flashlightTexture = std::make_unique<sf::RenderTexture>();
    m_maskTexture = std::make_unique<sf::RenderTexture>();
    m_shadowMapTexture = std::make_unique<sf::RenderTexture>();

    if (!m_lightTexture->create(windowSize.x, windowSize.y) ||
        !m_darknessTexture->create(windowSize.x, windowSize.y) ||
        !m_flashlightTexture->create(windowSize.x, windowSize.y) ||
        !m_shadowMapTexture->create(windowSize.x, windowSize.y) ||
        !m_maskTexture->create(windowSize.x, windowSize.y)) {
        std::cerr << "[DarkLevelSystem] Failed to create render textures" << std::endl;
        return;
    }

    // Initialize flashlight settings
    m_flashlightRange = 400.0f;
    m_flashlightAngle = 45.0f;
    m_flashlightIntensity = 0.9f;
    m_flashlightOn = true;

    // Initialize battery system
    m_batteryLevel = 1.0f;
    m_batteryDrainRate = 0.02f;
    m_lowBatteryWarning = false;

    // Set default darkness level
    m_darknessLevel = 0.92f;

    // Initialize darkness overlay
    m_darknessOverlay.setSize(sf::Vector2f(windowSize.x, windowSize.y));
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * m_darknessLevel)));

    // Try to load shadow shader
    try {
        if (sf::Shader::isAvailable()) {
            m_useShaders = m_shadowShader.loadFromFile("resources/shaders/shadow.frag", sf::Shader::Fragment);
            if (m_useShaders) {
                std::cout << "[DarkLevelSystem] Successfully loaded shadow shader" << std::endl;
                m_shadowShader.setUniform("u_resolution", sf::Glsl::Vec2(windowSize.x, windowSize.y));
            } else {
                std::cout << "[DarkLevelSystem] Failed to load shadow shader, using fallback" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "[DarkLevelSystem] Shader error: " << e.what() << std::endl;
        m_useShaders = false;
    }

    std::cout << "[DarkLevelSystem] Advanced shadow casting system initialized" << std::endl;
}

void DarkLevelSystem::update(float dt, PlayerEntity* player) {
    if (!m_enabled) return;

    // Update animation timers
    m_flickerTimer += dt;
    m_ambientTimer += dt;

    // Update battery level
    updateBattery(dt);
    
    // Update player light position
    updatePlayerLight(player);

    // Update flashlight direction based on player movement
    if (player) {
        auto* transform = player->getComponent<Transform>();
        if (transform) {
            auto* physics = player->getComponent<PhysicsComponent>();
            if (physics) {
                sf::Vector2f velocity = physics->getVelocity();
                if (std::abs(velocity.x) > 0.1f) {
                    m_flashlightDirection = sf::Vector2f(velocity.x > 0 ? 1.0f : -1.0f, 0.0f);
                }
            }
        }
    }
}

void DarkLevelSystem::updateBattery(float dt) {
    if (!m_flashlightOn) return;

    // Drain battery when flashlight is on
    m_batteryLevel -= m_batteryDrainRate * dt;
    m_batteryLevel = std::max(0.0f, m_batteryLevel);

    // Low battery warning
    if (m_batteryLevel < 0.2f && !m_lowBatteryWarning) {
        m_lowBatteryWarning = true;
        std::cout << "[Flashlight] LOW BATTERY WARNING!" << std::endl;
    }

    // Battery depleted
    if (m_batteryLevel <= 0.0f) {
        m_flashlightOn = false;
        std::cout << "[Flashlight] Battery depleted - flashlight OFF!" << std::endl;
    }

    // Flashlight flickers when battery is low
    if (m_batteryLevel < 0.1f && m_flashlightOn) {
        float flickerChance = (0.1f - m_batteryLevel) * 10.0f;
        if ((rand() % 100) < flickerChance * 30) {
            m_flashlightOn = false;
        }
    }
}

void DarkLevelSystem::updatePlayerLight(PlayerEntity* player) {
    if (!player) return;
    auto* transform = player->getComponent<Transform>();
    if (transform) {
        m_playerLightPos = transform->getPosition();
    }
}

void DarkLevelSystem::render(sf::RenderWindow& window) {
    if (!m_enabled) return;

    // Store current view
    sf::View currentView = window.getView();

    // Draw darkness overlay
    window.draw(m_darknessOverlay);

    // Render shadow map for all light sources
    m_shadowMapTexture->clear(sf::Color::Transparent);
    m_shadowMapTexture->setView(currentView);
    
    // Render light sources with shadows
    for (const auto& light : m_lightSources) {
        renderShadowMap(light.position, light.radius, *m_shadowMapTexture);
    }

    // Render player light with shadows
    if (!m_lightSources.empty()) {
        sf::Sprite shadowSprite(m_shadowMapTexture->getTexture());
        window.draw(shadowSprite, sf::RenderStates(sf::BlendAdd));
    }

    // Render flashlight if it's on and has battery
    if (m_flashlightOn && m_batteryLevel > 0.0f) {
        renderFlashlight(window);
    }
}

void DarkLevelSystem::renderShadowMap(const sf::Vector2f& lightPos, float radius, sf::RenderTexture& target) {
    // Cast rays in all directions from the light source
    std::vector<Ray> rays;
    castRays(lightPos, rays, radius);
    
    // Create a vertex array to draw the light with shadows
    sf::VertexArray shadowMesh(sf::TriangleFan);
    
    // Start at the light source position
    shadowMesh.append(sf::Vertex(lightPos, sf::Color(255, 255, 200, 255)));
    
    // Add vertices for each ray's endpoint
    for (const auto& ray : rays) {
        sf::Color rayColor = sf::Color(255, 255, 200, 50); // Light color fading at edges
        shadowMesh.append(sf::Vertex(ray.endPoint, rayColor));
    }
    
    // Close the shape by adding the first ray endpoint again
    if (!rays.empty()) {
        shadowMesh.append(sf::Vertex(rays[0].endPoint, sf::Color(255, 255, 200, 50)));
    }
    
    // Draw the shadow mesh
    target.draw(shadowMesh, sf::BlendAdd);
}

void DarkLevelSystem::castRays(const sf::Vector2f& origin, std::vector<Ray>& rays, float maxDistance) {
    // Generate rays in all directions
    rays.clear();
    
    // Number of rays to cast (more = smoother shadows but lower performance)
    const int rayCount = m_rayCount;
    
    // Cast rays in a full circle
    for (int i = 0; i < rayCount; i++) {
        float angle = static_cast<float>(i) * 2.0f * static_cast<float>(M_PI) / rayCount;
        Ray ray;
        ray.origin = origin;
        ray.direction = sf::Vector2f(std::cos(angle), std::sin(angle));
        ray.distance = maxDistance; // Default max distance
        ray.endPoint = origin + ray.direction * maxDistance;
        
        // Check for intersections with obstacles
        float minDist = maxDistance;
        sf::Vector2f closestPoint;
        bool hitObstacle = false;
        
        for (const auto& obstacle : m_obstacles) {
            Intersection intersection = rayObstacleIntersection(ray, obstacle);
            if (intersection.valid && intersection.distance < minDist) {
                minDist = intersection.distance;
                closestPoint = intersection.point;
                hitObstacle = true;
            }
        }
        
        // Update ray if it hit an obstacle
        if (hitObstacle) {
            ray.distance = minDist;
            ray.endPoint = closestPoint;
        }
        
        rays.push_back(ray);
    }
}

DarkLevelSystem::Intersection DarkLevelSystem::rayObstacleIntersection(const Ray& ray, const Obstacle& obstacle) {
    Intersection result = { {0, 0}, 0.0f, false };
    
    // Convert rectangle to four line segments
    sf::FloatRect bounds = obstacle.bounds;
    sf::Vector2f topLeft = {bounds.left, bounds.top};
    sf::Vector2f topRight = {bounds.left + bounds.width, bounds.top};
    sf::Vector2f bottomRight = {bounds.left + bounds.width, bounds.top + bounds.height};
    sf::Vector2f bottomLeft = {bounds.left, bounds.top + bounds.height};
    
    // Ray endpoints
    sf::Vector2f rayEnd = ray.origin + ray.direction * ray.distance;
    
    // Check intersection with each edge
    sf::Vector2f edges[4][2] = {
        {topLeft, topRight},
        {topRight, bottomRight},
        {bottomRight, bottomLeft},
        {bottomLeft, topLeft}
    };
    
    float minDist = ray.distance;
    bool found = false;
    sf::Vector2f closestIntersection;
    
    for (const auto& edge : edges) {
        sf::Vector2f intersection = calculateIntersection(ray.origin, rayEnd, edge[0], edge[1]);
        
        // Check if intersection is valid and within the ray's range
        sf::Vector2f dir = intersection - ray.origin;
        float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        
        bool onSegment = true;
        if (std::isnan(intersection.x) || std::isnan(intersection.y)) {
            onSegment = false;
        }
        
        // Calculate dot product to make sure the intersection is in front of the ray
        float dotProduct = dir.x * ray.direction.x + dir.y * ray.direction.y;
        
        if (onSegment && dotProduct > 0 && dist < minDist) {
            minDist = dist;
            closestIntersection = intersection;
            found = true;
        }
    }
    
    if (found) {
        result.point = closestIntersection;
        result.distance = minDist;
        result.valid = true;
    }
    
    return result;
}

sf::Vector2f DarkLevelSystem::calculateIntersection(
    const sf::Vector2f& rayStart, const sf::Vector2f& rayEnd, 
    const sf::Vector2f& segStart, const sf::Vector2f& segEnd) {
    
    // Calculate direction vectors
    sf::Vector2f r = rayEnd - rayStart;
    sf::Vector2f s = segEnd - segStart;
    
    // Calculate cross product of r and s
    float rxs = r.x * s.y - r.y * s.x;
    
    // Check if lines are parallel (or collinear)
    if (std::abs(rxs) < 0.0001f) {
        return sf::Vector2f(std::numeric_limits<float>::quiet_NaN(), 
                          std::numeric_limits<float>::quiet_NaN());
    }
    
    // Calculate parameters t and u
    sf::Vector2f qp = segStart - rayStart;
    float t = (qp.x * s.y - qp.y * s.x) / rxs;
    float u = (qp.x * r.y - qp.y * r.x) / rxs;
    
    // Check if intersection is within segments
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return rayStart + t * r;
    }
    
    return sf::Vector2f(std::numeric_limits<float>::quiet_NaN(), 
                      std::numeric_limits<float>::quiet_NaN());
}

void DarkLevelSystem::renderFlashlight(sf::RenderWindow& window) {
    // Prepare flashlight texture
    m_flashlightTexture->clear(sf::Color::Transparent);
    m_flashlightTexture->setView(window.getView());

    // Calculate flashlight intensity with battery and flicker effects
    float currentIntensity = m_flashlightIntensity * m_batteryLevel;
    float flicker = 1.0f;
    
    // Add flicker effect for low battery
    if (m_batteryLevel < 0.3f) {
        flicker = 0.7f + 0.3f * std::sin(m_flickerTimer * 20.0f);
    }
    currentIntensity *= flicker;

    // Draw flashlight cone with shadows
    drawFlashlightCone(currentIntensity);
    m_flashlightTexture->display();

    // Create sprite from flashlight texture
    sf::Sprite flashlightSprite(m_flashlightTexture->getTexture());

    // Draw with additive blending
    window.draw(flashlightSprite, sf::RenderStates(sf::BlendAdd));
}

void DarkLevelSystem::drawFlashlightCone(float intensity) {
    if (m_playerLightPos.x == 0 && m_playerLightPos.y == 0) return;

    // Create a vertex array for the flashlight cone
    sf::VertexArray flashlightCone(sf::TriangleFan);
    
    // Start at the player position
    sf::Vector2f center = m_playerLightPos;
    flashlightCone.append(sf::Vertex(center, sf::Color(255, 255, 200, static_cast<sf::Uint8>(255 * intensity))));

    // Calculate the cone direction and angle
    float angleRad = m_flashlightAngle * static_cast<float>(M_PI) / 180.0f;
    float directionAngle = std::atan2(m_flashlightDirection.y, m_flashlightDirection.x);
    
    // Create the cone segments
    int segments = 20;
    std::vector<Ray> flashlightRays;
    
    for (int i = 0; i <= segments; ++i) {
        float segmentAngle = directionAngle - angleRad / 2 + (angleRad * i / segments);
        
        // Create ray for shadow casting
        Ray ray;
        ray.origin = center;
        ray.direction = sf::Vector2f(std::cos(segmentAngle), std::sin(segmentAngle));
        ray.distance = m_flashlightRange;
        ray.endPoint = center + ray.direction * m_flashlightRange;
        
        // Check for obstacles in the ray's path
        float minDist = m_flashlightRange;
        sf::Vector2f closestPoint;
        bool hitObstacle = false;
        
        for (const auto& obstacle : m_obstacles) {
            Intersection intersection = rayObstacleIntersection(ray, obstacle);
            if (intersection.valid && intersection.distance < minDist) {
                minDist = intersection.distance;
                closestPoint = intersection.point;
                hitObstacle = true;
            }
        }
        
        // Update ray if it hit an obstacle
        if (hitObstacle) {
            ray.distance = minDist;
            ray.endPoint = closestPoint;
        }
        
        // Calculate alpha for this segment
        sf::Uint8 alpha = static_cast<sf::Uint8>(100 * intensity * (1.0f - static_cast<float>(i) / segments));
        
        // Add ray endpoint to the cone
        flashlightCone.append(sf::Vertex(ray.endPoint, sf::Color(255, 255, 180, alpha)));
        flashlightRays.push_back(ray);
    }
    
    // Draw the flashlight cone
    m_flashlightTexture->draw(flashlightCone);
    
    // Draw a soft halo around the player
    sf::CircleShape halo(50.0f);
    halo.setOrigin(50.0f, 50.0f);
    halo.setPosition(center);
    halo.setFillColor(sf::Color(255, 255, 200, static_cast<sf::Uint8>(50 * intensity)));
    m_flashlightTexture->draw(halo);
}

void DarkLevelSystem::toggleFlashlight() {
    if (m_batteryLevel > 0.0f) {
        m_flashlightOn = !m_flashlightOn;
        std::cout << "[Flashlight] " << (m_flashlightOn ? "ON" : "OFF") << std::endl;
    }
    else {
        std::cout << "[Flashlight] Cannot turn on - battery depleted!" << std::endl;
    }
}

void DarkLevelSystem::updateFlashlightDirection(const sf::Vector2f& playerPos, const sf::Vector2f& targetPos) {
    sf::Vector2f direction = targetPos - playerPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        m_flashlightDirection = direction / length;
    }
}

void DarkLevelSystem::renderLightSources(sf::RenderTexture& target) {
    // Use additive blending for light sources
    sf::BlendMode additiveBlend(sf::BlendMode::One, sf::BlendMode::One);
    
    for (const auto& light : m_lightSources) {
        // Cast rays from this light source to create shadow map
        std::vector<Ray> rays;
        castRays(light.position, rays, light.radius);
        
        // Create the light mesh with shadows
        sf::VertexArray lightMesh(sf::TriangleFan);
        lightMesh.append(sf::Vertex(light.position, light.color));
        
        // Add all ray endpoints
        for (const auto& ray : rays) {
            sf::Color rayColor = light.color;
            rayColor.a = static_cast<sf::Uint8>(255 * light.intensity * 
                                              (1.0f - ray.distance / light.radius));
            lightMesh.append(sf::Vertex(ray.endPoint, rayColor));
        }
        
        // Close the mesh
        if (!rays.empty()) {
            sf::Color rayColor = light.color;
            rayColor.a = static_cast<sf::Uint8>(255 * light.intensity * 
                                              (1.0f - rays[0].distance / light.radius));
            lightMesh.append(sf::Vertex(rays[0].endPoint, rayColor));
        }
        
        // Draw the light with shadows
        target.draw(lightMesh, additiveBlend);
        
        // Draw a soft light halo
        sf::CircleShape halo(light.radius * 0.5f);
        halo.setOrigin(light.radius * 0.5f, light.radius * 0.5f);
        halo.setPosition(light.position);
        
        sf::Color haloColor = light.color;
        haloColor.a = static_cast<sf::Uint8>(100 * light.intensity);
        halo.setFillColor(haloColor);
        
        target.draw(halo, additiveBlend);
    }
}

void DarkLevelSystem::setDarknessLevel(float level) {
    m_darknessLevel = std::clamp(level, 0.0f, 1.0f);
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * m_darknessLevel);
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, alpha));
    std::cout << "[DarkLevelSystem] Darkness level set to: " << m_darknessLevel << std::endl;
}

void DarkLevelSystem::addLightSource(const sf::Vector2f& position, float radius, sf::Color color) {
    LightSource light;
    light.position = position;
    light.radius = radius;
    light.color = color;
    light.intensity = 1.0f;
    m_lightSources.push_back(light);

    std::cout << "[DarkLevelSystem] Added light source at (" << position.x << ", " << position.y
        << ") with radius " << radius << std::endl;
}

void DarkLevelSystem::clearLightSources() {
    m_lightSources.clear();
    std::cout << "[DarkLevelSystem] Cleared all light sources" << std::endl;
}

void DarkLevelSystem::registerObstacle(const sf::FloatRect& bounds) {
    Obstacle obstacle;
    obstacle.bounds = bounds;
    m_obstacles.push_back(obstacle);
}

void DarkLevelSystem::clearObstacles() {
    m_obstacles.clear();
}

void DarkLevelSystem::setObstacles(const std::vector<sf::FloatRect>& obstacles) {
    m_obstacles.clear();
    for (const auto& rect : obstacles) {
        registerObstacle(rect);
    }
    std::cout << "[DarkLevelSystem] Set " << obstacles.size() << " shadow casting obstacles" << std::endl;
}
