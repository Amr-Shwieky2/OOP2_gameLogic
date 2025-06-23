#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "DynamicGameObject.h"
#include "InputService.h"
#include "ResourceManager.h"
#include "PlayerEffectManager.h"
#include "Constants.h"

class Player : public DynamicGameObject {
private:
    b2Body* m_body = nullptr;
    sf::Sprite m_sprite;
    TextureManager& m_textures;
    PlayerEffectManager m_effects;
    int m_groundContacts = 0;
    bool m_onGround = false;
    void updatePhysics(float deltaTime);

public:
    Player(b2World& world, float x, float y, TextureManager& textures);

    // ✅ مسؤوليات Player الأساسية فقط
    void handleInput(const InputService& input);
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;

    sf::FloatRect getBounds() const override;
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;

    // Movement controls
    void moveForward(float strength);
    void jump();
    bool isOnGround() const;

    // Ground contact tracking (for physics)
    void beginContact();
    void endContact();

    // Effect management (Player's visual/movement effects)
    void applyEffect(PlayerEffect effect, float duration);
    bool hasEffect(PlayerEffect effect) const;

    // Visual updates based on effects
    void updateVisuals();
};