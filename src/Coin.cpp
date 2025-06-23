#include "Coin.h"
#include <iostream>

Coin::Coin(float x, float y, TextureManager& textures, int value)
    : m_textures(textures), m_value(value)
{
    // Determine coin type based on value
    if (value >= 50) {
        m_type = CollectableType::RareCoin;
    }
    else {
        m_type = CollectableType::Coin;
    }

    setupSprite();
    m_sprite.setPosition(x, y);
}

std::unique_ptr<Coin> Coin::createRareCoin(float x, float y, TextureManager& textures) {
    return std::make_unique<Coin>(x, y, textures, 50);
}

void Coin::setupSprite() {
    // Choose texture based on coin type
    std::string textureName = (m_type == CollectableType::RareCoin) ? "rare_coin.png" : "coin.png";

    try {
        sf::Texture& tex = m_textures.getResource(textureName);
        m_sprite.setTexture(tex);
        m_sprite.setScale(0.08f, 0.08f);

        // Rare coins are slightly larger
        if (m_type == CollectableType::RareCoin) {
            m_sprite.setScale(0.12f, 0.12f);
        }
    }
    catch (const std::exception& e) {
        // Fallback to regular coin texture
        sf::Texture& tex = m_textures.getResource("coin.png");
        m_sprite.setTexture(tex);
        m_sprite.setScale(0.08f, 0.08f);
        std::cerr << "Warning: Could not load " << textureName << ", using fallback\n";
    }
}

void Coin::render(sf::RenderTarget& target) const {
    if (!m_collected) {
        target.draw(m_sprite);
    }
}

sf::FloatRect Coin::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Coin::update(float deltaTime) {
    if (m_collected) return;

    // Simple rotation animation
    static float rotationSpeed = 90.0f; // degrees per second
    float currentRotation = m_sprite.getRotation();
    m_sprite.setRotation(currentRotation + rotationSpeed * deltaTime);
}

void Coin::onCollect(GameState& gameState) {
    if (!m_collected) {
        m_collected = true;
        gameState.addScore(m_value);

        // Log collection
        if (m_type == CollectableType::RareCoin) {
            std::cout << "Rare coin collected! +" << m_value << " points! Total: "
                << gameState.getScore() << std::endl;
        }
        else {
            std::cout << " Coin collected! +" << m_value << " points! Total: "
                << gameState.getScore() << std::endl;
        }
    }
}

PlayerEffect Coin::getEffect() const {
    // Only rare coins have effects
    if (m_type == CollectableType::RareCoin) {
        return PlayerEffect::Magnetic; // Rare coins give magnetic effect
    }
    return PlayerEffect::None;
}

float Coin::getEffectDuration() const {
    if (m_type == CollectableType::RareCoin) {
        return 5.0f; // 5 seconds of magnetic effect
    }
    return 0.0f;
}