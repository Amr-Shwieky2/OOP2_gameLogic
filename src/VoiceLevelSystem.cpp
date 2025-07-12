#include "VoiceLevelSystem.h"
#include "PhysicsComponent.h"
#include "InputService.h"
#include <iostream>
#include <numeric>
#include <algorithm>

VoiceLevelSystem::VoiceLevelSystem() {
    // Create visual elements for voice feedback
    m_volumeBarBackground.setSize(sf::Vector2f(300.0f, 30.0f));
    m_volumeBarBackground.setFillColor(sf::Color(50, 50, 50, 200));
    m_volumeBarBackground.setOutlineColor(sf::Color::White);
    m_volumeBarBackground.setOutlineThickness(2.0f);
    
    m_volumeBar.setSize(sf::Vector2f(0.0f, 20.0f));
    m_volumeBar.setFillColor(sf::Color(100, 200, 100, 200));
    
    // Initialize input history with zeros
    for (int i = 0; i < 10; ++i) {
        m_inputHistory.push_back(0.0f);
    }
    
    // Load font for instructions
    if (!m_font.loadFromFile("arial.ttf")) {
        std::cout << "[VoiceLevelSystem] Warning: Could not load font" << std::endl;
    }
    
    // Set up instruction text
    m_instructionText.setFont(m_font);
    m_instructionText.setCharacterSize(18);
    m_instructionText.setFillColor(sf::Color::White);
    m_instructionText.setOutlineThickness(1.0f);
    m_instructionText.setOutlineColor(sf::Color::Black);
    m_instructionText.setString("Voice Control: Make noise to move right\nLouder = Faster, Very Loud = Jump");
}

VoiceLevelSystem::~VoiceLevelSystem() {
    // Nothing to clean up
}

void VoiceLevelSystem::initialize(sf::RenderWindow& window) {
    // Position UI elements based on window size
    sf::Vector2u windowSize = window.getSize();
    
    m_volumeBarBackground.setPosition(windowSize.x / 2 - 150.0f, 50.0f);
    m_volumeBar.setPosition(windowSize.x / 2 - 145.0f, 55.0f);
    
    sf::FloatRect textBounds = m_instructionText.getLocalBounds();
    m_instructionText.setOrigin(textBounds.width / 2.0f, 0.0f);
    m_instructionText.setPosition(windowSize.x / 2.0f, 100.0f);
    
    m_initialized = true;
    std::cout << "[VoiceLevelSystem] Initialized" << std::endl;
}

void VoiceLevelSystem::update(float deltaTime, PlayerEntity* player) {
    if (!m_enabled || !player) return;
    
    // In a real implementation, we would analyze microphone input here
    // Since we're simulating, we'll use keyboard input in debug mode
    if (m_debugMode) {
        handleDebugInput();
    }
    else {
        updateVoiceDetection(deltaTime);
    }
    
    // Apply voice-based controls to player
    simulateKeyPressBasedOnVoice(player);
    
    // Update UI
    float smoothedLevel = getSmoothedInput();
    m_volumeBar.setSize(sf::Vector2f(smoothedLevel * 290.0f, 20.0f));
    
    // Change color based on volume
    if (smoothedLevel < m_audioThreshold) {
        m_volumeBar.setFillColor(sf::Color(100, 200, 100, 200));  // Green
    }
    else if (smoothedLevel < m_jumpThreshold) {
        m_volumeBar.setFillColor(sf::Color(200, 200, 100, 200));  // Yellow
    }
    else {
        m_volumeBar.setFillColor(sf::Color(200, 100, 100, 200));  // Red
    }
}

void VoiceLevelSystem::render(sf::RenderWindow& window) {
    if (!m_enabled || !m_initialized) return;
    
    // Save current view
    sf::View currentView = window.getView();
    
    // Switch to default view for UI
    window.setView(window.getDefaultView());
    
    // Draw volume bar and instructions
    window.draw(m_volumeBarBackground);
    window.draw(m_volumeBar);
    window.draw(m_instructionText);
    
    // Restore previous view
    window.setView(currentView);
}

void VoiceLevelSystem::setEnabled(bool enabled) {
    m_enabled = enabled;
    std::cout << "[VoiceLevelSystem] " << (enabled ? "Enabled" : "Disabled") << std::endl;
    
    // Reset input level when enabling/disabling
    m_audioInputLevel = 0.0f;
    for (auto& level : m_inputHistory) {
        level = 0.0f;
    }
}

void VoiceLevelSystem::updateVoiceDetection(float deltaTime) {
    // In a real implementation, we would capture audio and analyze volume here
    // For this simulation, we'll use a timer to vary the volume
    
    // Create a pulsing volume effect
    float time = m_audioTimer.getElapsedTime().asSeconds();
    m_audioInputLevel = (std::sin(time * 2.0f) + 1.0f) / 2.0f * 0.5f;
    
    // Update the input history
    m_inputHistory.pop_front();
    m_inputHistory.push_back(m_audioInputLevel);
}

void VoiceLevelSystem::simulateKeyPressBasedOnVoice(PlayerEntity* player) {
    if (!player) return;
    
    float smoothedLevel = getSmoothedInput();
    
    auto* physics = player->getComponent<PhysicsComponent>();
    if (!physics) return;
    
    // Apply horizontal force based on volume
    if (smoothedLevel >= m_audioThreshold) {
        float forceMultiplier = (smoothedLevel - m_audioThreshold) / (1.0f - m_audioThreshold);
        forceMultiplier = std::min(forceMultiplier * 2.0f, 1.0f);
        
        
        // Update the applyForce call to pass two float arguments instead of a single sf::Vector2f
        physics->applyForce(40.0f * forceMultiplier, 0.0f);
    }
    
    // Jump if volume exceeds jump threshold and player is on ground
    if (smoothedLevel >= m_jumpThreshold && player->isOnGround()) {
        // Update the applyImpulse call to pass two float arguments instead of a single sf::Vector2f
        physics->applyImpulse(0.0f, -15.0f);
        
        // Visual feedback for jump
        static float lastJumpTime = 0.0f;
        float currentTime = m_audioTimer.getElapsedTime().asSeconds();
        
        if (currentTime - lastJumpTime > 0.5f) {
            lastJumpTime = currentTime;
            std::cout << "[VoiceLevelSystem] Jump triggered by loud noise!" << std::endl;
        }
    }
}

float VoiceLevelSystem::getSmoothedInput() const {
    // Average the input history for smoother response
    float sum = std::accumulate(m_inputHistory.begin(), m_inputHistory.end(), 0.0f);
    return sum / static_cast<float>(m_inputHistory.size());
}

void VoiceLevelSystem::handleDebugInput() {
    // Use keyboard to simulate voice input for debugging
    
    static InputService inputService;
    inputService.update();
    
    // Get current input level
    float newLevel = m_audioInputLevel;
    
    // Increase level when V key is pressed
    if (inputService.isKeyPressed(sf::Keyboard::V)) {
        newLevel = std::min(newLevel + 0.1f, 1.0f);
    }
    else {
        // Decrease level when not pressing
        newLevel = std::max(newLevel - 0.05f, 0.0f);
    }
    
    // Assign the new level
    m_audioInputLevel = newLevel;
    
    // Update input history
    m_inputHistory.pop_front();
    m_inputHistory.push_back(m_audioInputLevel);
}