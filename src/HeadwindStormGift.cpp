#include "HeadwindStormGift.h"
#include "GameObjectVisitor.h"

HeadwindStormGift::HeadwindStormGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("HeadwindStormGift.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void HeadwindStormGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect HeadwindStormGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void HeadwindStormGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void HeadwindStormGift::collect() {
    m_collected = true;
}

bool HeadwindStormGift::isCollected() const {
    return m_collected;
}
