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

    // Initialize flashlight settings for better visibility
    m_flashlightRange = 500.0f; // Increased range
    m_flashlightAngle = 60.0f;  // Wider angle
    m_flashlightIntensity = 1.0f; // Full intensity
    m_flashlightOn = true;

    // Set default darkness level
    m_darknessLevel = 0.92f;

    // Initialize darkness overlay - this will be updated dynamically in render()
    m_darknessOverlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    m_darknessOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * m_darknessLevel)));

    // Try to load shadow shader
    try {
        if (sf::Shader::isAvailable()) {
            m_useShaders = m_shadowShader.loadFromFile("resources/shaders/shadow.frag", sf::Shader::Fragment);
            if (m_useShaders) {
                std::cout << "[DarkLevelSystem] Successfully loaded shadow shader" << std::endl;
                m_shadowShader.setUniform("u_resolution", sf::Glsl::Vec2(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
            } else {
                std::cout << "[DarkLevelSystem] Failed to load shadow shader, using fallback" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "[DarkLevelSystem] Shader error: " << e.what() << std::endl;
        m_useShaders = false;
    }

    std::cout << "[DarkLevelSystem] Advanced shadow casting system initialized with improved settings" << std::endl;
    std::cout << "[DarkLevelSystem] Flashlight range: " << m_flashlightRange << ", angle: " << m_flashlightAngle << std::endl;
}

void DarkLevelSystem::update(float dt, PlayerEntity* player) {
    if (!m_enabled) return;

    updatePlayerLight(player);  

    // Update animation timers
    m_flickerTimer += dt;
    m_ambientTimer += dt;

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

void DarkLevelSystem::updatePlayerLight(PlayerEntity* player) {
    if (!player) {
        std::cout << "[DEBUG DarkLevelSystem] No player provided for light update" << std::endl;
        return;
    }
    
    auto* transform = player->getComponent<Transform>();
    if (transform) {
        m_playerLightPos = transform->getPosition();
       
    }
}

void DarkLevelSystem::render(sf::RenderWindow& window) {
    if (!m_enabled) {
        return;
    }

    sf::View currentView = window.getView();
    sf::Vector2f viewCenter = currentView.getCenter();
    sf::Vector2f viewSize = currentView.getSize();

    // 🔲 1. رسم العتمة الشفافة على كامل الشاشة (تغطي كل شيء)
    sf::RectangleShape darknessOverlay;
    darknessOverlay.setSize(viewSize);
    darknessOverlay.setPosition(viewCenter - viewSize / 2.0f);
    darknessOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(255 * m_darknessLevel)));

    window.draw(darknessOverlay);  // تغطي كل الخلفية

    // 💡 2. تجهيز Texture للضوء (الكشاف والضوء المحيط)
    m_flashlightTexture->setView(currentView);
    m_flashlightTexture->clear(sf::Color::Transparent);

    // ✨ 3. رسم الضوء حول اللاعب (هالة ثابتة)
    sf::CircleShape playerLight;
    float lightRadius = 50.0f;
    playerLight.setRadius(lightRadius);
    playerLight.setOrigin(lightRadius, lightRadius);
    playerLight.setPosition(m_playerLightPos);
    playerLight.setFillColor(sf::Color(255, 255, 220, 200));  // ضوء دافئ واضح

    m_flashlightTexture->draw(playerLight, sf::BlendAdd);

    // 🔦 4. إذا الكشاف شغال، ارسمه داخل نفس Texture
    if (m_flashlightOn && m_playerLightPos.x != 0 && m_playerLightPos.y != 0) {
        drawFlashlightCone(m_flashlightIntensity);
    }

    // 🖼️ 5. عرض محتوى الضوء على الشاشة بعد رسم الكائنات
    m_flashlightTexture->display();

    sf::Sprite flashlightSprite(m_flashlightTexture->getTexture());
    flashlightSprite.setPosition(window.mapPixelToCoords({ 0, 0 }));
    flashlightSprite.setOrigin(0, 0);
    window.draw(flashlightSprite, sf::RenderStates(sf::BlendAdd));
}



void DarkLevelSystem::renderShadowMap(const sf::Vector2f& lightPos, float radius, sf::RenderTexture& target) {
    // Cast rays in all directions from the light source
    std::vector<Ray> rays;
    castRays(lightPos, rays, radius);
    
    // Create a vertex array to draw the light with shadows and better color preservation
    sf::VertexArray shadowMesh(sf::TriangleFan);
    
    // Start at the light source position with brighter center
    shadowMesh.append(sf::Vertex(lightPos, sf::Color(255, 255, 255, 255)));
    
    // Add vertices for each ray's endpoint with better light distribution
    for (const auto& ray : rays) {
        // Calculate distance-based alpha for realistic light falloff
        float distanceFactor = 1.0f - (ray.distance / radius);
        distanceFactor = std::max(0.0f, distanceFactor);
        
        // Use brighter, more natural light color that preserves object colors
        sf::Uint8 alpha = static_cast<sf::Uint8>(150 * distanceFactor);
        sf::Color rayColor = sf::Color(255, 255, 240, alpha); // Warm white light
        
        shadowMesh.append(sf::Vertex(ray.endPoint, rayColor));
    }
    
    // Close the shape by adding the first ray endpoint again
    if (!rays.empty()) {
        float distanceFactor = 1.0f - (rays[0].distance / radius);
        distanceFactor = std::max(0.0f, distanceFactor);
        sf::Uint8 alpha = static_cast<sf::Uint8>(150 * distanceFactor);
        shadowMesh.append(sf::Vertex(rays[0].endPoint, sf::Color(255, 255, 240, alpha)));
    }
    
    // Draw the shadow mesh with additive blending for better color mixing
    target.draw(shadowMesh, sf::BlendAdd);
    
    // Add a secondary light layer for better object illumination
    sf::CircleShape lightGlow(radius * 0.7f);
    lightGlow.setOrigin(radius * 0.7f, radius * 0.7f);
    lightGlow.setPosition(lightPos);
    lightGlow.setFillColor(sf::Color(255, 255, 240, 60)); // Subtle glow
    target.draw(lightGlow, sf::BlendAdd);
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
    float currentIntensity = m_flashlightIntensity ;
    float flicker = 1.0f;

    currentIntensity *= flicker;

    sf::VertexArray cone(sf::TriangleFan);
    cone.append(sf::Vertex(m_playerLightPos, sf::Color(255, 255, 200, 200)));

    float spreadAngle = 60.0f;
    float angleStep = spreadAngle / m_rayCount;
    float startAngle = (m_flashlightDirection.x > 0 ? 0.0f : 180.0f) - spreadAngle / 2.0f;

    for (int i = 0; i <= m_rayCount; ++i) {
        float angleDeg = startAngle + i * angleStep;
        float angleRad = angleDeg * M_PI / 180.0f;

        sf::Vector2f direction(std::cos(angleRad), std::sin(angleRad));
        sf::Vector2f endPoint = m_playerLightPos + direction * 250.0f;

        cone.append(sf::Vertex(endPoint, sf::Color(255, 255, 200, 0)));
    }

    m_flashlightTexture->draw(cone);  
}

void DarkLevelSystem::drawFlashlightCone(float intensity) {
    if (m_playerLightPos.x == 0 && m_playerLightPos.y == 0) return;

    // Create a vertex array for the flashlight cone with better illumination
    sf::VertexArray flashlightCone(sf::TriangleFan);
    
    // Start at the player position (flashlight center)
    sf::Vector2f center = m_playerLightPos;
    flashlightCone.append(sf::Vertex(center, sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * intensity))));

    // Calculate the cone direction and angle
    float angleRad = m_flashlightAngle * static_cast<float>(M_PI) / 180.0f;
    float directionAngle = std::atan2(m_flashlightDirection.y, m_flashlightDirection.x);
    
    // Create the cone segments with better light distribution
    int segments = 30; // More segments for smoother cone
    std::vector<Ray> flashlightRays;
    
    for (int i = 0; i <= segments; ++i) {
        float segmentAngle = directionAngle - angleRad / 2 + (angleRad * i / segments);
        
        // Create ray for shadow casting and light refraction
        Ray ray;
        ray.origin = center;
        ray.direction = sf::Vector2f(std::cos(segmentAngle), std::sin(segmentAngle));
        ray.distance = m_flashlightRange;
        ray.endPoint = center + ray.direction * m_flashlightRange;
        
        // Check for obstacles and create light refraction effect
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
            
            // Add light refraction/reflection effect
            sf::Vector2f reflectionDir = ray.direction;
            // Simple reflection calculation (bounce off surface)
            reflectionDir.x = -reflectionDir.x * 0.3f; // Reduced reflection
            reflectionDir.y = -reflectionDir.y * 0.3f;
            
            // Create secondary light ray for reflection
            sf::Vector2f reflectedEnd = closestPoint + reflectionDir * (m_flashlightRange * 0.2f);
            
            // Add reflection light vertex with dimmer intensity
            sf::Uint8 reflectionAlpha = static_cast<sf::Uint8>(30 * intensity);
            flashlightCone.append(sf::Vertex(reflectedEnd, sf::Color(255, 255, 200, reflectionAlpha)));
        }
        
        // Calculate alpha based on distance and angle for realistic light falloff
        float distanceFactor = 1.0f - (ray.distance / m_flashlightRange);
        float angleFactor = 1.0f - (std::abs(static_cast<float>(i) - segments/2.0f) / (segments/2.0f));
        sf::Uint8 alpha = static_cast<sf::Uint8>(200 * intensity * distanceFactor * angleFactor);
        
        // Use brighter, more natural light color that reveals object colors
        flashlightCone.append(sf::Vertex(ray.endPoint, sf::Color(255, 255, 240, alpha)));
        flashlightRays.push_back(ray);
    }
    
    // Draw the flashlight cone
    m_flashlightTexture->draw(flashlightCone);
    
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
}
