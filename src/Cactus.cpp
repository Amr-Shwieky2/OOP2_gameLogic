#include "Cactus.h"
#include "Constants.h"

Cactus::Cactus(float x, float y, TextureManager& textures)
{
	sf::Texture& tex = textures.getResource("cactus.png");
	m_sprite.setTexture(tex);
	m_sprite.setOrigin(BOX_SIZE / 2.0f, BOX_SIZE / 2.0f);
	m_sprite.setScale(0.2f, 0.2f); // Adjust scale as needed
	m_sprite.setPosition(x, y);
	m_bounds = m_sprite.getGlobalBounds();
}
void Cactus::render(sf::RenderTarget& target) const
{
	target.draw(m_sprite);
}
sf::FloatRect Cactus::getBounds() const
{
	return m_bounds;
}





