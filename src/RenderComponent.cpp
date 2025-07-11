#include "RenderComponent.h"
#include "Entity.h"
#include "Transform.h"
#include <iostream>

RenderComponent::RenderComponent() 
    : m_visible(true)
    , m_hasTexture(false)
    , m_cullWhenOffscreen(false)
    , m_renderLayer(0)
    , m_animMode(AnimationMode::None)
    , m_animTimer(0.0f)
    , m_currentFrame(0)
    , m_totalFrames(0)
    , m_frameSize(0, 0)
    , m_frameOffset(0, 0) {
}

bool RenderComponent::initialize() {
    synchronizeWithTransform();
    return true;
}

void RenderComponent::setTexture(const sf::Texture& texture) {
    m_sprite.setTexture(texture);
    m_hasTexture = true;
    
    // Center origin by default
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    
    // Update position from Transform if available
    synchronizeWithTransform();
}

void RenderComponent::setSprite(const sf::Sprite& sprite) {
    m_sprite = sprite;
    m_hasTexture = true;
    
    // Update position from Transform if available
    synchronizeWithTransform();
}

sf::Sprite& RenderComponent::getSprite() {
    return m_sprite;
}

const sf::Sprite& RenderComponent::getSprite() const {
    return m_sprite;
}

void RenderComponent::setColor(const sf::Color& color) {
    m_sprite.setColor(color);
}

void RenderComponent::setOpacity(float alpha) {
    sf::Color color = m_sprite.getColor();
    color.a = static_cast<sf::Uint8>(std::max(0.0f, std::min(255.0f, alpha * 255.0f)));
    m_sprite.setColor(color);
}

void RenderComponent::setVisible(bool visible) {
    m_visible = visible;
}

bool RenderComponent::isVisible() const {
    return m_visible;
}

void RenderComponent::setRenderLayer(int layer) {
    m_renderLayer = layer;
}

int RenderComponent::getRenderLayer() const {
    return m_renderLayer;
}

bool RenderComponent::isInViewport(const sf::View& view) const {
    if (!m_hasTexture) return false;
    
    // Get sprite global bounds
    sf::FloatRect spriteBounds = m_sprite.getGlobalBounds();
    
    // Get view bounds
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    sf::FloatRect viewBounds(
        viewCenter.x - viewSize.x / 2.0f,
        viewCenter.y - viewSize.y / 2.0f,
        viewSize.x,
        viewSize.y
    );
    
    // Check if sprite intersects with view
    return spriteBounds.intersects(viewBounds);
}

void RenderComponent::setAnimationFrame(int frameX, int frameY, int frameWidth, int frameHeight) {
    if (!m_hasTexture) return;
    
    m_frameSize.x = frameWidth;
    m_frameSize.y = frameHeight;
    m_frameOffset.x = frameX;
    m_frameOffset.y = frameY;
    
    // Update texture rect
    m_sprite.setTextureRect(sf::IntRect(
        frameX * frameWidth,
        frameY * frameHeight,
        frameWidth,
        frameHeight
    ));
    
    // Recenter origin
    m_sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
}

void RenderComponent::updateAnimation(float dt) {
    if (m_animMode == AnimationMode::None || m_totalFrames <= 1 || !m_hasTexture) return;
    
    // Update animation timer
    m_animTimer += dt;
    
    // Update frame at 12 fps
    constexpr float frameDuration = 1.0f / 12.0f;
    if (m_animTimer >= frameDuration) {
        int numFramesToAdvance = static_cast<int>(m_animTimer / frameDuration);
        m_animTimer -= frameDuration * numFramesToAdvance;
        
        // Update frame index based on animation mode
        switch (m_animMode) {
            case AnimationMode::Loop:
                m_currentFrame = (m_currentFrame + numFramesToAdvance) % m_totalFrames;
                break;
                
            case AnimationMode::PingPong:
                // TODO: Implement ping-pong animation
                break;
                
            case AnimationMode::OneShot:
                m_currentFrame = std::min(m_currentFrame + numFramesToAdvance, m_totalFrames - 1);
                break;
                
            default:
                break;
        }
        
        // Update texture rect
        m_sprite.setTextureRect(sf::IntRect(
            (m_frameOffset.x + m_currentFrame) * m_frameSize.x,
            m_frameOffset.y * m_frameSize.y,
            m_frameSize.x,
            m_frameSize.y
        ));
    }
}

void RenderComponent::synchronizeWithTransform() {
    if (!m_owner) return;
    
    auto* transform = getTransformComponent();
    if (transform) {
        sf::Vector2f position = transform->getPosition();
        float rotation = transform->getRotation();
        sf::Vector2f scale = transform->getScale();
        
        m_sprite.setPosition(position);
        m_sprite.setRotation(rotation);
        m_sprite.setScale(scale);
    }
}

bool RenderComponent::validateDependencies() const {
    return getTransformComponent() != nullptr;
}

void RenderComponent::update(float dt) {
    // Update animation if active
    if (m_animMode != AnimationMode::None) {
        updateAnimation(dt);
    }
    
    // Synchronize with transform if available
    synchronizeWithTransform();
}

Transform* RenderComponent::getTransformComponent() const {
    return m_owner ? m_owner->getComponent<Transform>() : nullptr;
}

// Property registration for RenderComponent
void RenderComponent::registerProperties() {
    // Call base implementation
    Component::registerProperties();
    
    // Register render properties
    registerProperty<RenderComponent, bool>(
        "visible",
        PropertyValue::Type::Bool,
        &RenderComponent::isVisible,
        &RenderComponent::setVisible,
        PropertyDescriptor::Category::Visual
    );
    
    // Register render layer
    registerProperty<RenderComponent, int>(
        "renderLayer",
        PropertyValue::Type::Int,
        &RenderComponent::getRenderLayer,
        &RenderComponent::setRenderLayer,
        PropertyDescriptor::Category::Visual
    );
    
    // Register culling property
    registerProperty<RenderComponent, bool>(
        "cullWhenOffscreen",
        PropertyValue::Type::Bool,
        [](const RenderComponent* c) -> bool { return c->m_cullWhenOffscreen; },
        [](RenderComponent* c, bool value) { c->m_cullWhenOffscreen = value; },
        PropertyDescriptor::Category::Visual
    );
    
    // Register color properties
    registerProperty<RenderComponent, sf::Color>(
        "color",
        PropertyValue::Type::Color,
        [](const RenderComponent* c) -> sf::Color { return c->m_sprite.getColor(); },
        &RenderComponent::setColor,
        PropertyDescriptor::Category::Visual
    );
    
    registerProperty<RenderComponent, float>(
        "opacity",
        PropertyValue::Type::Float,
        [](const RenderComponent* c) -> float { return c->m_sprite.getColor().a / 255.0f; },
        &RenderComponent::setOpacity,
        PropertyDescriptor::Category::Visual
    );
    
    // Register animation properties
    std::vector<std::string> animModes = {
        "None",
        "Loop",
        "PingPong",
        "OneShot"
    };
    
    registerEnumProperty<RenderComponent, AnimationMode>(
        "animationMode",
        [](const RenderComponent* c) -> AnimationMode { return c->m_animMode; },
        [](RenderComponent* c, AnimationMode mode) { c->setAnimationMode(mode); },
        animModes,
        PropertyDescriptor::Category::Visual
    );
    
    // Configure property display
    configureProperty("visible", "Visible", "Whether the sprite is rendered");
    configureProperty("renderLayer", "Render Layer", "Layer for z-order sorting (higher layers render on top)");
    configureProperty("cullWhenOffscreen", "Cull Offscreen", "Skip rendering when the sprite is off-screen");
    configureProperty("color", "Color", "Color tint applied to the sprite");
    configureProperty("opacity", "Opacity", "Transparency (0.0 - 1.0)");
    configureProperty("animationMode", "Animation Mode", "How sprite animation is played");
    
    // Set ranges for numeric properties
    setPropertyRange("renderLayer", -10, 10);
    setPropertyRange("opacity", 0.0, 1.0);
}