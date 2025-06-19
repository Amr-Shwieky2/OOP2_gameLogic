#include "RareCoinGift.h"
#include "Player.h"
#include "GameObjectVisitor.h"

RareCoinGift::RareCoinGift(float x, float y, TextureManager& textures) {
    sf::Texture& tex = textures.getResource("RareCoinGidt.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.5f, 0.5f);
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

void RareCoinGift::onCollect(Player& player) {
    if (!m_collected) {
        m_collected = true;
        player.applyEffect(PlayerEffect::Magnetic, 5);
    }
}

bool RareCoinGift::isCollected() const {
    return m_collected;
}
