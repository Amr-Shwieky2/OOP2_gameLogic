#pragma once
#include "Component.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <string>

class Transform;

/**
 * RenderComponent - Handles rendering of game entities
 * Enhanced to work with specialized template methods in Entity class
 */
class RenderComponent : public Component {
public:
    // Animation modes for advanced rendering
    enum class AnimationMode {
        None,
        Loop,
        PingPong,
        OneShot
    };
    
    RenderComponent();
    
    // Enhanced initialization
    bool initialize();
    
    // Texture and sprite setup
    void setTexture(const sf::Texture& texture);
    void setSprite(const sf::Sprite& sprite);
    
    // Enhanced sprite access with validation
    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
    
    // Visual effects and optimization
    void setColor(const sf::Color& color);
    void setOpacity(float alpha);
    void setVisible(bool visible);
    bool isVisible() const { return m_visible; }
    
    // Layer control for z-sorting
    void setRenderLayer(int layer) { m_renderLayer = layer; }
    int getRenderLayer() const { return m_renderLayer; }
    
    // Optimized rendering
    void enableCulling(bool enabled) { m_cullWhenOffscreen = enabled; }
    bool isInViewport(const sf::View& view) const;
    
    // Animation support
    void setAnimationFrame(int frameX, int frameY, int frameWidth, int frameHeight);
    void updateAnimation(float dt);
    void setAnimationMode(AnimationMode mode) { m_animMode = mode; }
    
    // Helper methods for specialized template
    void synchronizeWithTransform();
    bool validateDependencies() const;
    
    // Update method that handles synchronization and animation
    void update(float dt) override;
    
private:
    sf::Sprite m_sprite;
    bool m_visible = true;
    bool m_hasTexture = false;
    bool m_cullWhenOffscreen = false;
    int m_renderLayer = 0;
    
    // Animation properties
    AnimationMode m_animMode = AnimationMode::None;
    float m_animTimer = 0.0f;
    int m_currentFrame = 0;
    int m_totalFrames = 0;
    sf::Vector2i m_frameSize;
    sf::Vector2i m_frameOffset;
    
    // Utility method to get transform component safely
    Transform* getTransformComponent() const;
};