#include "PlayerRenderer.h"
#include "Constants.h"

PlayerRenderer::PlayerRenderer(TextureManager& textures) : m_textures(textures) {
    setupSprite();
}

void PlayerRenderer::setupSprite() {
    m_sprite.setTexture(m_textures.getResource("NormalBall.png"));

    // Set origin to center
    sf::Vector2u textureSize = m_sprite.getTexture()->getSize();
    m_sprite.setOrigin(textureSize.x / 2.f, textureSize.y / 2.f);

    // Scale to desired size
    float desiredDiameter = PLAYER_RADIUS * 2 * PPM;
    float scaleX = desiredDiameter / textureSize.x;
    float scaleY = desiredDiameter / textureSize.y;
    m_sprite.setScale(scaleX, scaleY);
}

void PlayerRenderer::updateVisuals(sf::Vector2f position, float rotation, bool hasTransparent, bool hasMagnetic) {
    updateTexture(hasTransparent, hasMagnetic);
    m_sprite.setPosition(position);
    m_sprite.setRotation(rotation);
}

void PlayerRenderer::updateTexture(bool hasTransparent, bool hasMagnetic) {
    if (hasTransparent) {
        m_sprite.setTexture(m_textures.getResource("TransparentBall.png"));
    }
    else if (hasMagnetic) {
        m_sprite.setTexture(m_textures.getResource("MagneticBall.png"));
    }
    else {
        m_sprite.setTexture(m_textures.getResource("NormalBall.png"));
    }
}

void PlayerRenderer::render(sf::RenderTarget& target) const {
    target.draw(m_sprite);
}

sf::FloatRect PlayerRenderer::getBounds() const {
    return m_sprite.getGlobalBounds();
}

sf::Vector2f PlayerRenderer::getSpriteCenter() const {
    return m_sprite.getPosition();
}