#pragma once
#include "Performance/Profiler.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>

namespace Performance {

/**
 * @brief Display mode for the performance overlay
 */
enum class OverlayDisplayMode {
    Minimized,       // Show only essential info (FPS, frame time)
    Basic,           // Show basic stats and top consumers
    Detailed,        // Show detailed stats including memory
    GraphOnly,       // Show only performance graphs
    Full             // Show all available information
};

/**
 * @brief Graph type for visualizing performance metrics
 */
enum class GraphType {
    FrameTime,       // Frame time graph
    FrameTimeHistory,// Frame time over many frames
    Memory,          // Memory usage graph
    Custom           // Custom data source graph
};

/**
 * @brief Settings for performance overlay
 */
struct OverlaySettings {
    OverlayDisplayMode displayMode = OverlayDisplayMode::Basic;
    sf::Color backgroundColor = sf::Color(0, 0, 0, 180);
    sf::Color textColor = sf::Color::White;
    sf::Color graphColor = sf::Color::Green;
    sf::Color warningColor = sf::Color::Yellow;
    sf::Color criticalColor = sf::Color::Red;
    sf::Color graphBackgroundColor = sf::Color(30, 30, 30, 200);
    int fontSize = 12;
    int padding = 10;
    int graphHeight = 60;
    int graphWidth = 200;
    float updateInterval = 0.25f; // Update interval in seconds
    float warningThresholdMs = 16.0f; // Warning threshold for frame time (16ms = ~60 FPS)
    float criticalThresholdMs = 33.0f; // Critical threshold for frame time (33ms = ~30 FPS)
    bool showFps = true;
    bool showFrameTime = true;
    bool showMemory = true;
    bool showTopConsumers = true;
    bool showHotspots = true;
    bool showGraph = true;
    bool autoScaleGraph = true;
    int numTopConsumers = 5;
    GraphType graphType = GraphType::FrameTime;
    std::string fontPath = "arial.ttf";
};

/**
 * @brief Visual overlay that displays performance metrics in real-time
 */
class PerformanceOverlay {
public:
    /**
     * @brief Constructor
     * @param settings Initial settings for the overlay
     */
    explicit PerformanceOverlay(const OverlaySettings& settings = OverlaySettings());
    
    /**
     * @brief Initialize the overlay
     * @return True if initialization succeeded
     */
    bool initialize();
    
    /**
     * @brief Update the overlay
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Draw the overlay to a render target
     * @param target Render target to draw to
     */
    void draw(sf::RenderTarget& target);
    
    /**
     * @brief Set the display mode
     * @param mode New display mode
     */
    void setDisplayMode(OverlayDisplayMode mode);
    
    /**
     * @brief Toggle visibility of the overlay
     */
    void toggleVisibility();
    
    /**
     * @brief Check if the overlay is visible
     * @return True if the overlay is visible
     */
    bool isVisible() const { return m_visible; }
    
    /**
     * @brief Set custom data source for graphing
     * @param source Function that returns data to graph
     */
    void setCustomDataSource(std::function<float()> source);
    
    /**
     * @brief Set the graph type
     * @param type Type of graph to display
     */
    void setGraphType(GraphType type);
    
    /**
     * @brief Update settings
     * @param settings New settings
     */
    void updateSettings(const OverlaySettings& settings);
    
    /**
     * @brief Get current settings
     * @return Current settings
     */
    const OverlaySettings& getSettings() const { return m_settings; }
    
    /**
     * @brief Handle input events
     * @param event SFML event to handle
     * @return True if the event was handled
     */
    bool handleEvent(const sf::Event& event);
    
    /**
     * @brief Set position of the overlay
     * @param position Position in pixels
     */
    void setPosition(const sf::Vector2f& position);
    
    /**
     * @brief Get position of the overlay
     * @return Position in pixels
     */
    sf::Vector2f getPosition() const { return m_position; }
    
    /**
     * @brief Add a hotspot area to highlight
     * @param name Name of the profiling scope
     */
    void addHotspotScope(const std::string& name);
    
    /**
     * @brief Clear hotspot areas
     */
    void clearHotspots();
    
private:
    // Settings
    OverlaySettings m_settings;
    
    // State
    bool m_visible = true;
    bool m_initialized = false;
    bool m_dragging = false;
    sf::Vector2f m_position;
    sf::Vector2f m_dragOffset;
    float m_updateTimer = 0.0f;
    
    // Resources
    sf::Font m_font;
    sf::RectangleShape m_background;
    sf::RectangleShape m_graphBackground;
    
    // Text elements
    sf::Text m_titleText;
    sf::Text m_fpsText;
    sf::Text m_frameTimeText;
    sf::Text m_memoryText;
    std::vector<sf::Text> m_consumerTexts;
    std::vector<sf::Text> m_hotspotTexts;
    
    // Graph data
    std::vector<float> m_graphData;
    std::vector<sf::Vertex> m_graphVertices;
    std::function<float()> m_customDataSource;
    
    // Hotspots
    std::vector<std::string> m_hotspotScopes;
    
    // Helper methods
    void updateTexts();
    void updateGraph();
    void updateLayout();
    void createTexts();
    void drawGraph(sf::RenderTarget& target);
    float getGraphValue();
    
    // Data collection methods
    std::vector<std::pair<std::string, double>> getTopConsumers() const;
    void detectHotspots();
};

} // namespace Performance