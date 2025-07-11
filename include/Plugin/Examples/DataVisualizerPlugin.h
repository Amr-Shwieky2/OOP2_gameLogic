#pragma once

#include "Plugin/PluginTemplate.h"
#include "Plugin/PluginHelper.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <deque>

/**
 * @brief Data visualizer plugin that shows game statistics
 */
class DataVisualizerPlugin : public PluginTemplate<DataVisualizerPlugin> {
public:
    DataVisualizerPlugin() 
        : PluginTemplate(
            "DataVisualizerPlugin",            // Name
            PluginVersion(1, 0, 0),           // Version
            "Visualizes game data in real-time", // Description
            "Plugin Author",                   // Author
            "Visualization",                   // Category
            true                               // Supports hot reload
        ) {
    }
    
    bool onInitialize() override {
        // Create helper
        m_helper = std::make_unique<PluginHelper>(getName(), getGameSession());
        
        // Log initialization
        m_helper->log("Initializing data visualizer plugin");
        
        // Add dependency on example plugin
        addDependency("ExamplePlugin");
        
        // Listen for example plugin's custom events
        m_helper->listenForEvent("ScoreUpdate",
            [this](const PluginCustomEvent& event) {
                int score = std::stoi(event.getEventData());
                addDataPoint(score);
                m_helper->log("Received score update: " + std::to_string(score));
            }
        );
        
        // Set up visualization
        if (!m_font.loadFromFile("arial.ttf")) {
            m_helper->logError("Failed to load font");
            return false;
        }
        
        m_title.setFont(m_font);
        m_title.setCharacterSize(20);
        m_title.setFillColor(sf::Color::White);
        m_title.setPosition(10, 50);
        m_title.setString("Score Visualization");
        
        // Set up graph
        m_graph.setPrimitiveType(sf::LineStrip);
        
        m_helper->log("Initialization complete");
        return true;
    }
    
    void onUpdate(float deltaTime) override {
        // Update graph animation
        m_animationTime += deltaTime;
        
        // Update graph color for visual effect
        float hue = std::fmod(m_animationTime * 20.0f, 360.0f);
        sf::Color color = hsvToRgb(hue, 1.0f, 1.0f);
        
        for (size_t i = 0; i < m_graph.getVertexCount(); ++i) {
            m_graph[i].color = color;
        }
    }
    
    void onRender(sf::RenderWindow& window) override {
        // Render visualization
        window.draw(m_title);
        window.draw(m_graph);
    }
    
    bool onShutdown() override {
        m_helper->log("Shutting down data visualizer plugin");
        return true;
    }
    
    void addDataPoint(int value) {
        // Add data point to history
        m_dataHistory.push_back(value);
        
        // Keep only last 100 points
        if (m_dataHistory.size() > 100) {
            m_dataHistory.pop_front();
        }
        
        // Update graph
        updateGraph();
    }
    
private:
    void updateGraph() {
        if (m_dataHistory.empty()) {
            return;
        }
        
        // Find min/max values for scaling
        int minValue = m_dataHistory[0];
        int maxValue = m_dataHistory[0];
        
        for (int value : m_dataHistory) {
            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
        }
        
        // Ensure reasonable scale
        if (maxValue == minValue) {
            maxValue = minValue + 1;
        }
        
        // Create vertices for graph
        m_graph.clear();
        
        float graphWidth = 300.0f;
        float graphHeight = 100.0f;
        float xStart = 10.0f;
        float yStart = 80.0f;
        
        for (size_t i = 0; i < m_dataHistory.size(); ++i) {
            float x = xStart + (i / static_cast<float>(m_dataHistory.size() - 1)) * graphWidth;
            float y = yStart + graphHeight - ((m_dataHistory[i] - minValue) / static_cast<float>(maxValue - minValue)) * graphHeight;
            
            sf::Vertex vertex(sf::Vector2f(x, y), sf::Color::Green);
            m_graph.append(vertex);
        }
    }
    
    sf::Color hsvToRgb(float h, float s, float v) {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h / 60.0f, 2) - 1));
        float m = v - c;
        
        float r, g, b;
        
        if (h < 60) {
            r = c; g = x; b = 0;
        } else if (h < 120) {
            r = x; g = c; b = 0;
        } else if (h < 180) {
            r = 0; g = c; b = x;
        } else if (h < 240) {
            r = 0; g = x; b = c;
        } else if (h < 300) {
            r = x; g = 0; b = c;
        } else {
            r = c; g = 0; b = x;
        }
        
        return sf::Color(
            static_cast<sf::Uint8>((r + m) * 255),
            static_cast<sf::Uint8>((g + m) * 255),
            static_cast<sf::Uint8>((b + m) * 255)
        );
    }
    
    std::unique_ptr<PluginHelper> m_helper;
    sf::Font m_font;
    sf::Text m_title;
    sf::VertexArray m_graph;
    std::deque<int> m_dataHistory;
    float m_animationTime = 0.0f;
};

// Define plugin factory functions
DEFINE_PLUGIN_FACTORY(DataVisualizerPlugin)