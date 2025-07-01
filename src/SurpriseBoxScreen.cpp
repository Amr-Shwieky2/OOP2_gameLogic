#include "SurpriseBoxScreen.h"
#include "ResourceManager.h"
#include <iostream>
#include <cmath>
#include <sstream>

SurpriseBoxScreen::SurpriseBoxScreen(sf::RenderWindow& window, TextureManagerType& textures)
    : m_window(window)
    , m_textures(textures)
    , m_gen(std::random_device{}())
{
    // Set box position at center of screen
    m_boxPosition = sf::Vector2f(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);
    m_giftPosition = sf::Vector2f(m_boxPosition.x, m_boxPosition.y - 100.0f);

    // Try to load box texture
    bool textureLoaded = false;
    try {
        // Try both possible texture names
        try {
            m_boxSprite.setTexture(m_textures.getResource("CloseBox.png"));
            textureLoaded = true;
        }
        catch (...) {
            // Try alternative name
            m_boxSprite.setTexture(m_textures.getResource("OpenBox.png"));
            textureLoaded = true;
        }

        if (textureLoaded) {
            sf::Vector2u textureSize = m_boxSprite.getTexture()->getSize();
            if (textureSize.x > 0 && textureSize.y > 0) {
                m_boxSprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
                m_useSprite = true;
            }
        }
    }
    catch (...) {
        std::cout << "[SurpriseBox] Failed to load box texture, using fallback" << std::endl;
        m_useSprite = false;
    }

    // Setup background
    m_background.setSize(sf::Vector2f(m_window.getSize()));
    try {
        m_background.setTexture(&m_textures.getResource("BoxBackground.png"));
    }
    catch (...) {
        m_background.setFillColor(sf::Color(20, 20, 50, 200)); // Dark blue
    }

    // Setup overlay for dimming effect
    m_overlay.setSize(sf::Vector2f(m_window.getSize()));
    m_overlay.setFillColor(sf::Color(0, 0, 0, 0));

    // Setup fallback box
    m_fallbackBox.setSize(sf::Vector2f(120.0f, 120.0f));
    m_fallbackBox.setOrigin(60.0f, 60.0f);
    m_fallbackBox.setFillColor(sf::Color(139, 69, 19)); // Brown
    m_fallbackBox.setOutlineThickness(3.0f);
    m_fallbackBox.setOutlineColor(sf::Color(101, 67, 33)); // Darker brown

    // Load font
    try {
        if (!m_font.loadFromFile("arial.ttf")) {
            std::cerr << "[SurpriseBox] Failed to load font" << std::endl;
        }
    }
    catch (...) {
        std::cerr << "[SurpriseBox] Font loading exception" << std::endl;
    }

    // Setup text elements
    m_instructionText.setFont(m_font);
    m_instructionText.setCharacterSize(28);
    m_instructionText.setFillColor(sf::Color::White);
    m_instructionText.setOutlineThickness(2.0f);
    m_instructionText.setOutlineColor(sf::Color::Black);

    m_giftNameText.setFont(m_font);
    m_giftNameText.setCharacterSize(36);
    m_giftNameText.setFillColor(sf::Color::Yellow);
    m_giftNameText.setOutlineThickness(2.0f);
    m_giftNameText.setOutlineColor(sf::Color::Black);

    m_giftDescriptionText.setFont(m_font);
    m_giftDescriptionText.setCharacterSize(20);
    m_giftDescriptionText.setFillColor(sf::Color::White);
    m_giftDescriptionText.setOutlineThickness(1.0f);
    m_giftDescriptionText.setOutlineColor(sf::Color::Black);
}

SurpriseGiftType SurpriseBoxScreen::showSurpriseBox() {
    m_isRunning = true;
    m_boxOpened = false;
    m_showingGift = false;
    m_canContinue = false;
    m_phase = AnimationPhase::WaitingToOpen;

    // Pre-select the gift
    m_selectedGift = getRandomGiftType();

    // Reset animation state
    m_animationTimer = 0.0f;
    m_boxScale = 1.0f;
    m_giftScale = 0.0f;
    m_particles.clear();
    m_giftImageLoaded = false;

    std::cout << "[SurpriseBox] Starting surprise box sequence. Selected gift: "
        << getGiftName(m_selectedGift) << std::endl;

    sf::Clock clock;

    while (m_isRunning && m_window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();
    }

    std::cout << "[SurpriseBox] Returning gift: " << getGiftName(m_selectedGift) << std::endl;
    return m_selectedGift;
}

void SurpriseBoxScreen::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            m_isRunning = false;
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            switch (m_phase) {
            case AnimationPhase::WaitingToOpen:
                // Open the box
                m_boxOpened = true;
                m_animationTimer = 0.0f;
                m_phase = AnimationPhase::Opening;
                createParticles();
                std::cout << "[SurpriseBox] Box opened!" << std::endl;
                break;

            case AnimationPhase::WaitingToContinue:
                // Return to game
                m_isRunning = false;
                std::cout << "[SurpriseBox] Returning to game" << std::endl;
                break;

            default:
                // Ignore space press during animations
                break;
            }
        }
    }
}

void SurpriseBoxScreen::update(float deltaTime) {
    m_animationTimer += deltaTime;

    // Update animations based on current phase
    switch (m_phase) {
    case AnimationPhase::WaitingToOpen:
        // Gentle breathing animation
        updateBoxAnimation(deltaTime);
        break;

    case AnimationPhase::Opening:
        updateBoxAnimation(deltaTime);
        if (m_animationTimer > 0.5f) {
            m_phase = AnimationPhase::Exploding;
            m_animationTimer = 0.0f;
        }
        break;

    case AnimationPhase::Exploding:
        updateBoxAnimation(deltaTime);
        updateParticles(deltaTime);
        if (m_animationTimer > 1.0f) {
            m_phase = AnimationPhase::ShowingGift;
            m_animationTimer = 0.0f;
            m_showingGift = true;
        }
        break;

    case AnimationPhase::ShowingGift:
        updateParticles(deltaTime);
        // Fade in gift
        m_giftScale = std::min(1.0f, m_animationTimer * 2.0f);

        // Load gift image if not loaded
        if (!m_giftImageLoaded && m_giftScale > 0.5f) {
            std::string giftImages[] = {
                "LifeHeartGift.png",         // LifeHeart
                "SpeedGift.png",             // SpeedBoost
                "ProtectiveShieldGift.png",  // Shield
                "RareCoinGift.png",          // RareCoin
                "ReverseMovementGift.png",   // ReverseMovement
                "HeadwindStormGift.png",     // HeadwindStorm
                "MagneticGift.png"           // Magnetic
            };

            try {
                m_giftSprite.setTexture(m_textures.getResource(giftImages[static_cast<int>(m_selectedGift)]));
                sf::Vector2u giftSize = m_giftSprite.getTexture()->getSize();
                m_giftSprite.setOrigin(giftSize.x / 2.0f, giftSize.y / 2.0f);
                m_giftSprite.setPosition(m_giftPosition);
                m_giftSprite.setScale(0.3f, 0.3f);
                m_giftImageLoaded = true;
            }
            catch (...) {
                std::cout << "[SurpriseBox] Failed to load gift image" << std::endl;
                m_giftImageLoaded = true; // Prevent retry
            }
        }

        if (m_animationTimer > 1.5f) {
            m_phase = AnimationPhase::WaitingToContinue;
            m_canContinue = true;
        }
        break;

    case AnimationPhase::WaitingToContinue:
        updateParticles(deltaTime);
        // Add floating animation to gift
        m_giftPosition.y = (m_boxPosition.y - 100.0f) + std::sin(m_animationTimer * 2.0f) * 10.0f;
        if (m_giftImageLoaded) {
            m_giftSprite.setPosition(m_giftPosition);
        }
        break;
    }

    // Update overlay fade
    if (m_phase != AnimationPhase::WaitingToOpen) {
        float alpha = std::min(150.0f, m_animationTimer * 150.0f);
        m_overlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
    }
}

void SurpriseBoxScreen::updateBoxAnimation(float deltaTime) {
    switch (m_phase) {
    case AnimationPhase::WaitingToOpen:
        // Breathing animation
        m_boxScale = 1.0f + 0.05f * std::sin(m_animationTimer * 3.0f);
        break;

    case AnimationPhase::Opening:
        // Shake animation
    {
        float shake = std::sin(m_animationTimer * 50.0f) * 10.0f * (1.0f - m_animationTimer * 2.0f);
        sf::Vector2f shakePos = m_boxPosition + sf::Vector2f(shake, 0);

        if (m_useSprite) {
            m_boxSprite.setPosition(shakePos);
        }
        else {
            m_fallbackBox.setPosition(shakePos);
        }
    }
    break;

    case AnimationPhase::Exploding:
        // Expand and fade
        m_boxScale = 1.0f + m_animationTimer * 3.0f;
        break;

    default:
        break;
    }

    // Apply scale
    if (m_useSprite) {
        m_boxSprite.setScale(m_boxScale, m_boxScale);
        if (m_phase != AnimationPhase::Opening) {
            m_boxSprite.setPosition(m_boxPosition);
        }
    }
    else {
        m_fallbackBox.setScale(m_boxScale, m_boxScale);
        if (m_phase != AnimationPhase::Opening) {
            m_fallbackBox.setPosition(m_boxPosition);
        }
    }
}

void SurpriseBoxScreen::updateParticles(float deltaTime) {
    for (auto& particle : m_particles) {
        particle.position += particle.velocity * deltaTime;
        particle.life -= deltaTime;

        // Fade out
        float alpha = std::max(0.0f, particle.life / particle.maxLife);
        particle.color.a = static_cast<sf::Uint8>(255 * alpha);

        // Apply gravity
        particle.velocity.y += 300.0f * deltaTime;

        // Slow down horizontally
        particle.velocity.x *= 0.98f;
    }

    // Remove dead particles
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const ExplosionParticle& p) { return p.life <= 0.0f; }),
        m_particles.end()
    );
}

void SurpriseBoxScreen::createParticles() {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(200.0f, 500.0f);
    std::uniform_real_distribution<float> sizeDist(3.0f, 10.0f);
    std::uniform_real_distribution<float> lifeDist(1.0f, 2.0f);

    // Get color based on gift type
    sf::Color giftColor = getGiftColor(m_selectedGift);

    for (int i = 0; i < 60; ++i) {
        ExplosionParticle particle;
        particle.position = m_boxPosition;

        float angle = angleDist(m_gen);
        float speed = speedDist(m_gen);
        particle.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed - 200.0f);

        // Mix gift color with random variations
        particle.color = giftColor;
        particle.color.r = std::min(255, particle.color.r + rand() % 50);
        particle.color.g = std::min(255, particle.color.g + rand() % 50);
        particle.color.b = std::min(255, particle.color.b + rand() % 50);

        particle.size = sizeDist(m_gen);
        particle.life = particle.maxLife = lifeDist(m_gen);

        m_particles.push_back(particle);
    }
}

void SurpriseBoxScreen::render() {
    m_window.clear();

    // Draw background
    m_window.draw(m_background);

    // Draw overlay
    m_window.draw(m_overlay);

    // Draw particles
    for (const auto& particle : m_particles) {
        sf::CircleShape circle(particle.size);
        circle.setOrigin(particle.size, particle.size);
        circle.setPosition(particle.position);
        circle.setFillColor(particle.color);
        m_window.draw(circle);
    }

    // Draw box (if still visible)
    if (m_phase == AnimationPhase::WaitingToOpen ||
        m_phase == AnimationPhase::Opening ||
        (m_phase == AnimationPhase::Exploding && m_animationTimer < 0.5f)) {

        float alpha = (m_phase == AnimationPhase::Exploding) ?
            255.0f * (1.0f - m_animationTimer * 2.0f) : 255.0f;

        if (m_useSprite) {
            m_boxSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
            m_window.draw(m_boxSprite);
        }
        else {
            sf::Color boxColor = m_fallbackBox.getFillColor();
            boxColor.a = static_cast<sf::Uint8>(alpha);
            m_fallbackBox.setFillColor(boxColor);
            m_window.draw(m_fallbackBox);
        }
    }

    // Draw gift
    if (m_showingGift && m_giftImageLoaded) {
        m_giftSprite.setScale(0.3f * m_giftScale, 0.3f * m_giftScale);
        m_window.draw(m_giftSprite);
    }

    // Draw UI elements
    drawInstructions();
    if (m_showingGift) {
        drawGiftInfo();
    }

    m_window.display();
}

void SurpriseBoxScreen::drawInstructions() {
    std::string instruction;

    switch (m_phase) {
    case AnimationPhase::WaitingToOpen:
        instruction = "Press SPACE to open the surprise box!";
        break;
    case AnimationPhase::WaitingToContinue:
        instruction = "Press SPACE to continue";
        break;
    default:
        return; // No instruction during animations
    }

    m_instructionText.setString(instruction);

    // Center the text
    sf::FloatRect bounds = m_instructionText.getLocalBounds();
    m_instructionText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    m_instructionText.setPosition(m_window.getSize().x / 2.0f, m_window.getSize().y - 100.0f);

    m_window.draw(m_instructionText);
}

void SurpriseBoxScreen::drawGiftInfo() {
    // Draw gift name
    m_giftNameText.setString(getGiftName(m_selectedGift));
    sf::FloatRect nameBounds = m_giftNameText.getLocalBounds();
    m_giftNameText.setOrigin(nameBounds.width / 2.0f, nameBounds.height / 2.0f);
    m_giftNameText.setPosition(m_window.getSize().x / 2.0f, m_giftPosition.y + 100.0f);

    // Draw gift description
    std::string description;
    switch (m_selectedGift) {
    case SurpriseGiftType::LifeHeart:
        description = "Restores one life point!";
        break;
    case SurpriseGiftType::SpeedBoost:
        description = "Move 50% faster for 8 seconds!";
        break;
    case SurpriseGiftType::Shield:
        description = "Become invulnerable for 7 seconds!";
        break;
    case SurpriseGiftType::RareCoin:
        description = "Worth 50 points!";
        break;
    case SurpriseGiftType::ReverseMovement:
        description = "Reverses enemy movement for 10 seconds!";
        break;
    case SurpriseGiftType::HeadwindStorm:
        description = "Creates wind that slows enemies!";
        break;
    case SurpriseGiftType::Magnetic:
        description = "Attracts coins to you for 15 seconds!";
        break;
    }

    m_giftDescriptionText.setString(description);
    sf::FloatRect descBounds = m_giftDescriptionText.getLocalBounds();
    m_giftDescriptionText.setOrigin(descBounds.width / 2.0f, descBounds.height / 2.0f);
    m_giftDescriptionText.setPosition(m_window.getSize().x / 2.0f, m_giftPosition.y + 140.0f);

    // Apply fade-in effect
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * m_giftScale);
    sf::Color nameColor = m_giftNameText.getFillColor();
    nameColor.a = alpha;
    m_giftNameText.setFillColor(nameColor);

    sf::Color descColor = m_giftDescriptionText.getFillColor();
    descColor.a = alpha;
    m_giftDescriptionText.setFillColor(descColor);

    m_window.draw(m_giftNameText);
    m_window.draw(m_giftDescriptionText);
}

SurpriseGiftType SurpriseBoxScreen::getRandomGiftType() {
    std::uniform_int_distribution<int> dist(0, 6);
    return static_cast<SurpriseGiftType>(dist(m_gen));
}

std::string SurpriseBoxScreen::getGiftName(SurpriseGiftType type) const {
    switch (type) {
    case SurpriseGiftType::LifeHeart:      return "Life Heart";
    case SurpriseGiftType::SpeedBoost:     return "Speed Boost";
    case SurpriseGiftType::Shield:         return "Shield";
    case SurpriseGiftType::RareCoin:       return "Rare Coin";
    case SurpriseGiftType::ReverseMovement: return "Reverse Movement";
    case SurpriseGiftType::HeadwindStorm:  return "Headwind Storm";
    case SurpriseGiftType::Magnetic:       return "Magnetic Power";
    default:                               return "Unknown Gift";
    }
}

sf::Color SurpriseBoxScreen::getGiftColor(SurpriseGiftType type) const {
    switch (type) {
    case SurpriseGiftType::LifeHeart:      return sf::Color(255, 100, 100); // Red
    case SurpriseGiftType::SpeedBoost:     return sf::Color(255, 255, 100); // Yellow
    case SurpriseGiftType::Shield:         return sf::Color(100, 200, 255); // Light Blue
    case SurpriseGiftType::RareCoin:       return sf::Color(255, 215, 0);   // Gold
    case SurpriseGiftType::ReverseMovement: return sf::Color(200, 100, 255); // Purple
    case SurpriseGiftType::HeadwindStorm:  return sf::Color(150, 150, 255); // Light Purple
    case SurpriseGiftType::Magnetic:       return sf::Color(255, 150, 100); // Orange
    default:                               return sf::Color::White;
    }
}