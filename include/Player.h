#pragma once
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <memory>
#include "DynamicGameObject.h"
#include "InputService.h"
#include "ResourceManager.h"
#include "PlayerEffectManager.h"
#include "PlayerMovement.h"
#include "PlayerRenderer.h"
#include "PlayerWeapon.h"
#include "PlayerStats.h"
#include "Constants.h"
#include "WindEffect.h"

class Player : public DynamicGameObject {
public:
    Player(b2World& world, float x, float y, TextureManager& textures);

    // DynamicGameObject interface
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;

    // Input handling
    void handleInput(const InputService& input);

    // Position and movement
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getVelocity() const;
    void moveForward(float strength);
    void jump();
    bool isOnGround() const;
    bool isFacingRight() const;
    b2Body* getBody() const;


    // Combat
    void shoot(TextureManager& textures);
    void shootCurved(TextureManager& textures);
    const std::vector<std::unique_ptr<Projectile>>& getProjectiles() const;

    // Stats
    void addLife();
    void loseLife();
    void increaseScore(int amount);
    int getScore() const;
    int getLives() const;

    // Effects
    void applyEffect(PlayerEffect effect, float duration);
    bool hasEffect(PlayerEffect effect) const;

    // Physics contacts (called by collision system)
    void beginContact();
    void endContact();
    void applyJumpImpulse();

    // Utility
    TextureManager& getTextureManager()const;
    sf::Vector2f getSpriteCenter() const;

    void kill();
    void pushBackFrom(const sf::Vector2f& sourcePosition);

    void updateCactusCooldown(float deltaTime);
    bool canTakeCactusDamage() const;
    void resetCactusCooldown();

    void updateWindEffect(float deltaTime);

    void renderWindEffect(sf::RenderTarget& target, const sf::View& camera) const;

private:
    // Components
    std::unique_ptr<PlayerMovement> m_movement;
    std::unique_ptr<PlayerRenderer> m_renderer;
    std::unique_ptr<PlayerWeapon> m_weapon;
    std::unique_ptr<PlayerStats> m_stats;
    PlayerEffectManager m_effects;

    TextureManager& m_textures;

    float m_cactusDamageCooldown = 0.f;

    std::unique_ptr<WindEffect> m_windEffect;
};