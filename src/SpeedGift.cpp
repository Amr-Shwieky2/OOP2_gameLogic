#include "SpeedGift.h"
#include "GameObjectVisitor.h"

SpeedGift::SpeedGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("SpeedGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void SpeedGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect SpeedGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void SpeedGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void SpeedGift::collect() {
    m_collected = true;
}

bool SpeedGift::isCollected() const {
    return m_collected;
}
