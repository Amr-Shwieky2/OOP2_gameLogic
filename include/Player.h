#pragma once

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "DynamicGameObject.h"
#include "InputService.h"
#include "ResourceManager.h"
#include "PlayerEffectManager.h"
#include "Constants.h"


class Player : public DynamicGameObject {
public:
    Player(b2World& world, float x, float y, TextureManager& textures);

    void handleInput(const InputService& input);
    void update(float deltaTime) override;
    void render(sf::RenderTarget& target) const override;

    void accept(GameObjectVisitor& visitor) override;
    sf::FloatRect getBounds() const override;
    sf::Vector2f getPosition() const;

    void addLife();
    void increaseScore(int amount);
    void loseLife();
    int getScore() const;
    int getLives() const;

    void applyEffect(PlayerEffect effect, float duration);
    bool hasEffect(PlayerEffect effect) const;

    TextureManager& getTextureManager();

private:
    void updateVisuals();
    void updatePhysics(float deltaTime);

    b2Body* m_body = nullptr;
    sf::Sprite m_sprite;
    TextureManager& m_textures;

    PlayerEffectManager m_effects;

    int m_score = 0;
    int m_lives = 3;
    bool m_onGround = false;
};
