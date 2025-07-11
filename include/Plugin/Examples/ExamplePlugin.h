#pragma once

#include "Plugin/PluginTemplate.h"
#include "Plugin/PluginHelper.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

/**
 * @brief Example plugin that demonstrates plugin capabilities
 */
class ExamplePlugin : public PluginTemplate<ExamplePlugin> {
public:
    ExamplePlugin() 
        : PluginTemplate(
            "ExamplePlugin",                   // Name
            PluginVersion(1, 0, 0),           // Version
            "Example plugin for demonstration", // Description
            "Plugin Author",                   // Author
            "Examples",                        // Category
            true                               // Supports hot reload
        ) {
    }
    
    bool onInitialize() override {
        // Create helper
        m_helper = std::make_unique<PluginHelper>(getName(), getGameSession());
        
        // Log initialization
        m_helper->log("Initializing example plugin");
        
        // Subscribe to score changed event
        m_helper->listenForGameEvent<ScoreChangedEvent>(
            [this](const ScoreChangedEvent& event) {
                onScoreChanged(event);
            }
        );
        
        // Set up UI text
        if (!m_font.loadFromFile("arial.ttf")) {
            m_helper->logError("Failed to load font");
            return false;
        }
        
        m_text.setFont(m_font);
        m_text.setCharacterSize(24);
        m_text.setFillColor(sf::Color::Yellow);
        m_text.setPosition(10, 10);
        m_text.setString("Example Plugin v1.0.0");
        
        m_helper->log("Initialization complete");
        return true;
    }
    
    void onUpdate(float deltaTime) override {
        // Update animation
        m_animationTime += deltaTime;
        float alpha = (std::sin(m_animationTime * 2.0f) + 1.0f) * 0.5f;
        m_text.setFillColor(sf::Color(255, 255, 0, static_cast<sf::Uint8>(alpha * 255)));
    }
    
    void onRender(sf::RenderWindow& window) override {
        // Render plugin UI
        window.draw(m_text);
    }
    
    bool onShutdown() override {
        m_helper->log("Shutting down example plugin");
        return true;
    }
    
    void onScoreChanged(const ScoreChangedEvent& event) {
        // Update text based on score
        m_text.setString("Example Plugin - Score: " + std::to_string(event.newScore));
        
        // Send a plugin event
        m_helper->sendEvent("ScoreUpdate", std::to_string(event.newScore));
    }
    
private:
    std::unique_ptr<PluginHelper> m_helper;
    sf::Font m_font;
    sf::Text m_text;
    float m_animationTime = 0.0f;
};

// Define plugin factory functions
DEFINE_PLUGIN_FACTORY(ExamplePlugin)