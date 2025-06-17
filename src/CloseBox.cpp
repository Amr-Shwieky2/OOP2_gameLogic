#include "CloseBox.h"
#include "GameObjectVisitor.h"

CloseBox::CloseBox(float x, float y, TextureManager& textures)
    : m_textures(textures)
{
    sf::Texture& tex = textures.getResource("CloseBox.png");
    m_sprite.setTexture(tex);
    m_sprite.setScale(0.08f, 0.08f);
    m_sprite.setPosition(x, y);
}

void CloseBox::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect CloseBox::getBounds() const {
    return m_sprite.getGlobalBounds();
}

void CloseBox::accept(GameObjectVisitor& visitor) {
    visitor.visit(*this);
}

bool CloseBox::isOpened() const {
    return m_opened;
}

void CloseBox::open() {
    if (!m_opened) {
        m_opened = true;
        m_sprite.setTexture(m_textures.getResource("OpenBox.png"));
    }
}
