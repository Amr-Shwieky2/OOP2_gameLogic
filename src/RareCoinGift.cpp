#include "RareCoinGift.h"
#include "GameObjectVisitor.h"

RareCoinGift::RareCoinGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("RareCoinGidt.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void RareCoinGift::render(sf::RenderTarget& target) const {
    if (!m_collected)
        target.draw(m_sprite);
}

sf::FloatRect RareCoinGift::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void RareCoinGift::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

void RareCoinGift::update(float deltaTime)
{
}

bool RareCoinGift::isCollected() const {
    return m_collected;
}

void RareCoinGift::collect() {
    m_collected = true;
}
