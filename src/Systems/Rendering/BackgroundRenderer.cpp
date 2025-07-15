#include "BackgroundRenderer.h"
#include "Constants.h"
#include <stdexcept>

//-------------------------------------------------------------------------------------
BackgroundRenderer::BackgroundRenderer(TextureManager&) {
    if (!m_backgroundTexture.loadFromFile("backGroundGame.jpeg")) {
        throw std::runtime_error("Failed to load background image.");
    }
    setupBackground();
}
//-------------------------------------------------------------------------------------
void BackgroundRenderer::setupBackground() {
    m_backgroundSprite.setTexture(m_backgroundTexture);
    float scaleY = WINDOW_HEIGHT / m_backgroundTexture.getSize().y;
    m_backgroundSprite.setScale(scaleY, scaleY);
}
//-------------------------------------------------------------------------------------
void BackgroundRenderer::render(sf::RenderWindow& window, const sf::View& camera) const {
    float bgWidth = m_backgroundTexture.getSize().x * m_backgroundSprite.getScale().x;
    float camLeft = camera.getCenter().x - camera.getSize().x / 2.f;
    float camRight = camLeft + camera.getSize().x;

    int startTile = static_cast<int>(camLeft / bgWidth) - 1;
    int endTile = static_cast<int>(camRight / bgWidth) + 1;

    for (int i = startTile; i <= endTile; ++i) {
        sf::Sprite repeatedBg = m_backgroundSprite;
        repeatedBg.setPosition(i * bgWidth, 0.f);
        window.draw(repeatedBg);
    }
}
//-------------------------------------------------------------------------------------