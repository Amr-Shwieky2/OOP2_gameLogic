#include "WindEffect.h"
#include <cmath>
#include <algorithm>
#include <iostream>

WindEffect::WindEffect() : m_generator(std::random_device{}()) {
    m_particles.reserve(MAX_PARTICLES);
}

void WindEffect::startEffect(sf::Vector2f playerPosition) {
    m_isActive = true;
    m_spawnTimer = 0.0f;

    // Create massive initial burst of sand particles
    for (int i = 0; i < 120; ++i) { // 
        createWindParticles(playerPosition);
    }
}

void WindEffect::stopEffect() {
    m_isActive = false;
}

void WindEffect::update(float deltaTime, sf::Vector2f playerPosition) {
    if (m_isActive) {
        m_spawnTimer += deltaTime;

        // Spawn particles more frequently for dense effect
        if (m_spawnTimer >= SPAWN_INTERVAL && m_particles.size() < MAX_PARTICLES) {
            createWindParticles(playerPosition);
            m_spawnTimer = 0.0f;
        }
    }

    updateParticles(deltaTime);
    cleanupDeadParticles();
}

void WindEffect::createWindParticles(sf::Vector2f playerPosition) {
    std::uniform_real_distribution<float> xStartOffset(0.0f, 800.0f);    
    std::uniform_real_distribution<float> yFullScreen(-800.0f, 800.0f); 
    std::uniform_real_distribution<float> sizeRand(2.0f, 12.0f);
    std::uniform_real_distribution<float> speedRand(0.7f, 1.3f);
    std::uniform_real_distribution<float> angleRand(-15.0f, 15.0f);
    std::uniform_int_distribution<int> colorVariant(0, 7);

    // Create many more particles to fill the wide area
    std::uniform_int_distribution<int> particleCount(8, 12); 
    int count = particleCount(m_generator);

    for (int i = 0; i < count; ++i) {
        WindParticle particle;

        // Position: Fill entire right side of screen (800px width)
        float startX = playerPosition.x + 100.0f + xStartOffset(m_generator); 
        float startY = playerPosition.y + yFullScreen(m_generator);

        particle.position = sf::Vector2f(startX, startY);

        // Velocity: All particles move leftward across the screen
        float baseSpeed = PARTICLE_SPEED * speedRand(m_generator);
        float angle = angleRand(m_generator) * (3.14159f / 180.0f);

        particle.velocity = sf::Vector2f(
            -baseSpeed * std::cos(angle), // Always negative (leftward)
            baseSpeed * std::sin(angle) * 0.5f // Reduced vertical movement
        );

        // Visual properties
        particle.size = sizeRand(m_generator);
        particle.life = particle.maxLife = PARTICLE_LIFE;
        particle.angle = 0.0f;
        particle.rotationSpeed = std::uniform_real_distribution<float>(-120.0f, 120.0f)(m_generator);

        // More varied desert sand colors with good opacity
        sf::Color sandColors[] = {
            sf::Color(194, 178, 128, 180), // Sandy beige 
            sf::Color(210, 180, 140, 160), // Light tan
            sf::Color(205, 133, 63, 170),  // Sandy brown
            sf::Color(244, 164, 96, 150),  // Desert orange
            sf::Color(222, 184, 135, 190), // Natural sand
            sf::Color(188, 143, 143, 140), // Dusty pink
            sf::Color(160, 130, 98, 200),  // Dark sand
            sf::Color(218, 165, 32, 160)   // Goldenrod
        };

        particle.color = sandColors[colorVariant(m_generator)];

        m_particles.push_back(particle);
    }
}
void WindEffect::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        // Update position
        particle.position += particle.velocity * deltaTime;

        // Update life
        particle.life -= deltaTime;

        // Fade out over time (slower fade for more visibility)
        float lifeRatio = particle.life / particle.maxLife;
        particle.color.a = static_cast<sf::Uint8>(particle.color.a * lifeRatio);

        // Update rotation - faster rotation for sand effect
        particle.angle += particle.rotationSpeed * deltaTime * 1.5f;

        // More intense turbulence for realistic sandstorm
        float turbulence1 = std::sin(particle.position.x * 0.01f) * 25.0f;
        float turbulence2 = std::cos(particle.position.y * 0.008f) * 15.0f;
        particle.velocity.y += (turbulence1 + turbulence2) * deltaTime;

        // Add swirling motion
        float swirl = std::sin(particle.position.x * 0.005f + particle.position.y * 0.003f) * 30.0f;
        particle.velocity.x += swirl * deltaTime * 0.1f;
    }
}

void WindEffect::cleanupDeadParticles() {
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const WindParticle& p) { return p.life <= 0.0f; }),
        m_particles.end()
    );
}

void WindEffect::render(sf::RenderTarget& target, const sf::View& camera) const {
    // Get camera bounds for culling
    sf::FloatRect viewBounds(
        camera.getCenter().x - camera.getSize().x / 2.0f,
        camera.getCenter().y - camera.getSize().y / 2.0f,
        camera.getSize().x,
        camera.getSize().y
    );

    for (const auto& particle : m_particles) {
        // Render particles even slightly outside view for seamless effect
        sf::FloatRect expandedBounds = viewBounds;
        expandedBounds.left -= 100.0f;
        expandedBounds.top -= 100.0f;
        expandedBounds.width += 200.0f;
        expandedBounds.height += 200.0f;

        if (expandedBounds.contains(particle.position)) {
            // Main particle shape - varied shapes for more realistic sand
            if (particle.size < 6.0f) {
                // Small particles - circles
                sf::CircleShape shape(particle.size);
                shape.setOrigin(particle.size, particle.size);
                shape.setPosition(particle.position);
                shape.setFillColor(particle.color);
                shape.setRotation(particle.angle);
                target.draw(shape);
            }
            else {
                // Larger particles - rectangles for sand grain effect
                sf::RectangleShape rect(sf::Vector2f(particle.size, particle.size * 0.7f));
                rect.setOrigin(particle.size / 2.0f, particle.size * 0.35f);
                rect.setPosition(particle.position);
                rect.setFillColor(particle.color);
                rect.setRotation(particle.angle);
                target.draw(rect);
            }

            // Enhanced streak effect for all particles
            sf::RectangleShape streak(sf::Vector2f(particle.size * 3.5f, 2.0f));
            streak.setOrigin(streak.getSize().x / 2.0f, streak.getSize().y / 2.0f);
            streak.setPosition(particle.position);
            streak.setRotation(particle.angle + 15.0f); // Slight angle offset

            sf::Color streakColor = particle.color;
            streakColor.a = static_cast<sf::Uint8>(streakColor.a * 0.3f);
            streak.setFillColor(streakColor);

            target.draw(streak);

            // Add dust cloud effect for larger particles
            if (particle.size > 8.0f) {
                sf::CircleShape dustCloud(particle.size * 1.5f);
                dustCloud.setOrigin(particle.size * 1.5f, particle.size * 1.5f);
                dustCloud.setPosition(particle.position.x - 10.0f, particle.position.y + 5.0f);

                sf::Color dustColor = particle.color;
                dustColor.a = static_cast<sf::Uint8>(dustColor.a * 0.15f);
                dustCloud.setFillColor(dustColor);

                target.draw(dustCloud);
            }
        }
    }
}