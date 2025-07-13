// WindEffectComponent.h
#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <vector>

class WindEffectComponent : public Component {
public:
    WindEffectComponent();

    void update(float dt) override;
    void startEffect();
    void stopEffect();

    void render(sf::RenderTarget& target) const;

private:
    struct Particle {
        sf::Vector2f position;
        sf::Vector2f velocity;
        float lifetime;
    };

    std::vector<Particle> m_particles;
    bool m_active = false;
};