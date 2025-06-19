#include "Coin.h"
#include "GameObjectVisitor.h"

Coin::Coin(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    sf::Texture& tex = m_textures.getResource("coin.png");
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
//
void Coin::update(float deltaTime) {
    if (m_collected) return;

    // Optional magnetic logic can be re-added if needed
    // Currently a placeholder update
}

void Coin::collect() {
    m_collected = true;
}

bool Coin::isCollected() const {
    return m_collected;
}
