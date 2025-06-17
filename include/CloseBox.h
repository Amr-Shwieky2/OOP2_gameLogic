#pragma once
#include "GameObject.h"
#include "ResourceManager.h"

class CloseBox : public GameObject {
public:
    CloseBox(float x, float y, TextureManager& textures);

    void render(sf::RenderTarget& target) const override;
    sf::FloatRect getBounds() const override;
    void accept(GameObjectVisitor& visitor) override;

    bool isOpened() const;
    void open();

private:
    sf::Sprite m_sprite;
    bool m_opened = false;
    TextureManager& m_textures;
};
