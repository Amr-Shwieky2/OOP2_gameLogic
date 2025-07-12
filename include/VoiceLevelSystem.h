#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "PlayerEntity.h"
#include <vector>
#include <deque>
#include <memory>

/**
 * VoiceLevelSystem - Handles voice input in the voice-controlled level
 * 
 * This system uses audio input level to control player movement in the
 * voice-controlled level. It simulates voice detection by analyzing 
 * microphone input and converting volume levels to movement commands.
 */
class VoiceLevelSystem {
public:
    VoiceLevelSystem();
    ~VoiceLevelSystem();

    /**
     * Initialize the voice system
     * @param window The game window
     */
    void initialize(sf::RenderWindow& window);

    /**
     * Update the voice system
     * @param deltaTime Time elapsed since last frame
     * @param player The player entity
     */
    void update(float deltaTime, PlayerEntity* player);

    /**
     * Render voice feedback UI
     * @param window The render window
     */
    void render(sf::RenderWindow& window);

    /**
     * Enable or disable the voice system
     * @param enabled Whether the system should be enabled
     */
    void setEnabled(bool enabled);

    /**
     * Check if the voice system is enabled
     * @return True if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }

private:
    bool m_enabled = false;
    bool m_initialized = false;
    
    // Audio input detection
    sf::Clock m_audioTimer;
    float m_audioInputLevel = 0.0f;
    float m_audioThreshold = 0.3f;
    float m_jumpThreshold = 0.7f;
    
    // Visual feedback
    sf::RectangleShape m_volumeBar;
    sf::RectangleShape m_volumeBarBackground;
    sf::Text m_instructionText;
    sf::Font m_font;
    
    // Voice detection simulation
    void updateVoiceDetection(float deltaTime);
    void simulateKeyPressBasedOnVoice(PlayerEntity* player);
    
    // Input history for smoothing
    std::deque<float> m_inputHistory;
    float getSmoothedInput() const;
    
    // Debug input (simulates voice when microphone not available)
    void handleDebugInput();
    bool m_debugMode = true;
};