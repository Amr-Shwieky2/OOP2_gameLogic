// WindEffect.h
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

// Wind particle structure
struct WindParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float life;
    float maxLife;
    float size;
    sf::Color color;
    float angle;
    float rotationSpeed;
};

class WindEffect {
public:
    WindEffect();

    // Start and stop wind effect
    void startEffect(sf::Vector2f playerPosition);
    void stopEffect();

    // Update particles
    void update(float deltaTime, sf::Vector2f playerPosition);

    // Render wind effect
    void render(sf::RenderTarget& target, const sf::View& camera) const;

    // Check if effect is active
    bool isActive() const { return m_isActive; }

private:
    void createWindParticles(sf::Vector2f playerPosition);
    void updateParticles(float deltaTime);
    void cleanupDeadParticles();

    std::vector<WindParticle> m_particles;
    std::mt19937 m_generator;
    bool m_isActive = false;
    float m_spawnTimer = 0.0f;

    // Wind effect settings
    static constexpr float SPAWN_INTERVAL = 0.02f; 
    static constexpr int MAX_PARTICLES = 400;      
    static constexpr float PARTICLE_SPEED = 280.0f;
    static constexpr float PARTICLE_LIFE = 3.5f;   
};