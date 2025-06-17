#include "Coin.h"
#include "GameObjectVisitor.h"
#include "Player.h"
#include <cmath>

Coin::Coin(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("Coin.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void Coin::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect Coin::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void Coin::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void Coin::collect() {
    m_collected = true;
}

bool Coin::isCollected() const {
    return m_collected;
}

void Coin::update(float deltaTime) {
    if (m_collected) return;

    // Optional: add magnetic movement if player is nearby
    // Requires Coin to have access to Player or position update logic elsewhere
}
