#include "RenderComponent.h"

//-------------------------------------------------------------------------------------
void RenderComponent::setTexture(const sf::Texture& texture) {
    m_sprite.setTexture(texture);
}
//-------------------------------------------------------------------------------------
void RenderComponent::setSprite(const sf::Sprite& sprite) {
    m_sprite = sprite;
}
//-------------------------------------------------------------------------------------
sf::Sprite& RenderComponent::getSprite() {
    return m_sprite;
}
//-------------------------------------------------------------------------------------
const sf::Sprite& RenderComponent::getSprite() const {
    return m_sprite;
}
//-------------------------------------------------------------------------------------