#include "Performance/PerformanceOverlay.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace Performance {

PerformanceOverlay::PerformanceOverlay(const OverlaySettings& settings)
    : m_settings(settings), m_position(10, 10)
{
    // Initialize graph data
    m_graphData.resize(m_settings.graphWidth, 0.0f);
}

bool PerformanceOverlay::initialize() {
    if (m_initialized) {
        return true;
    }
    
    // Load font
    if (!m_font.loadFromFile(m_settings.fontPath)) {
        std::cerr << "Failed to load font for performance overlay: " << m_settings.fontPath << std::endl;
        return false;
    }
    
    // Set up background
    m_background.setFillColor(m_settings.backgroundColor);
    
    // Set up graph background
    m_graphBackground.setFillColor(m_settings.graphBackgroundColor);
    
    // Create text elements
    createTexts();
    
    // Update layout
    updateLayout();
    
    m_initialized = true;
    return true;
}

void PerformanceOverlay::update(float deltaTime) {
    if (!m_visible || !m_initialized) {
        return;
    }
    
    // Update at specified intervals
    m_updateTimer += deltaTime;
    if (m_updateTimer >= m_settings.updateInterval) {
        updateTexts();
        updateGraph();
        m_updateTimer = 0.0f;
    }
}

void PerformanceOverlay::draw(sf::RenderTarget& target) {
    if (!m_visible || !m_initialized) {
        return;
    }
    
    // Draw background
    target.draw(m_background);
    
    // Draw title
    target.draw(m_titleText);
    
    // Draw FPS text
    if (m_settings.showFps) {
        target.draw(m_fpsText);
    }
    
    // Draw frame time text
    if (m_settings.showFrameTime) {
        target.draw(m_frameTimeText);
    }
    
    // Draw memory text
    if (m_settings.showMemory) {
        target.draw(m_memoryText);
    }
    
    // Draw top consumers
    if (m_settings.showTopConsumers) {
        for (const auto& text : m_consumerTexts) {
            target.draw(text);
        }
    }
    
    // Draw hotspots
    if (m_settings.showHotspots) {
        for (const auto& text : m_hotspotTexts) {
            target.draw(text);
        }
    }
    
    // Draw graph
    if (m_settings.showGraph) {
        drawGraph(target);
    }
}

void PerformanceOverlay::setDisplayMode(OverlayDisplayMode mode) {
    m_settings.displayMode = mode;
    
    // Update visibility based on mode
    switch (mode) {
        case OverlayDisplayMode::Minimized:
            m_settings.showFps = true;
            m_settings.showFrameTime = true;
            m_settings.showMemory = false;
            m_settings.showTopConsumers = false;
            m_settings.showHotspots = false;
            m_settings.showGraph = false;
            break;
            
        case OverlayDisplayMode::Basic:
            m_settings.showFps = true;
            m_settings.showFrameTime = true;
            m_settings.showMemory = true;
            m_settings.showTopConsumers = true;
            m_settings.showHotspots = false;
            m_settings.showGraph = false;
            break;
            
        case OverlayDisplayMode::Detailed:
            m_settings.showFps = true;
            m_settings.showFrameTime = true;
            m_settings.showMemory = true;
            m_settings.showTopConsumers = true;
            m_settings.showHotspots = true;
            m_settings.showGraph = false;
            break;
            
        case OverlayDisplayMode::GraphOnly:
            m_settings.showFps = false;
            m_settings.showFrameTime = false;
            m_settings.showMemory = false;
            m_settings.showTopConsumers = false;
            m_settings.showHotspots = false;
            m_settings.showGraph = true;
            break;
            
        case OverlayDisplayMode::Full:
            m_settings.showFps = true;
            m_settings.showFrameTime = true;
            m_settings.showMemory = true;
            m_settings.showTopConsumers = true;
            m_settings.showHotspots = true;
            m_settings.showGraph = true;
            break;
    }
    
    updateLayout();
}

void PerformanceOverlay::toggleVisibility() {
    m_visible = !m_visible;
}

void PerformanceOverlay::setCustomDataSource(std::function<float()> source) {
    m_customDataSource = source;
}

void PerformanceOverlay::setGraphType(GraphType type) {
    m_settings.graphType = type;
}

void PerformanceOverlay::updateSettings(const OverlaySettings& settings) {
    m_settings = settings;
    
    if (m_initialized) {
        // Update visual elements
        m_background.setFillColor(m_settings.backgroundColor);
        m_graphBackground.setFillColor(m_settings.graphBackgroundColor);
        
        // Update text properties
        m_titleText.setFillColor(m_settings.textColor);
        m_titleText.setCharacterSize(m_settings.fontSize + 2);
        
        m_fpsText.setFillColor(m_settings.textColor);
        m_fpsText.setCharacterSize(m_settings.fontSize);
        
        m_frameTimeText.setFillColor(m_settings.textColor);
        m_frameTimeText.setCharacterSize(m_settings.fontSize);
        
        m_memoryText.setFillColor(m_settings.textColor);
        m_memoryText.setCharacterSize(m_settings.fontSize);
        
        for (auto& text : m_consumerTexts) {
            text.setFillColor(m_settings.textColor);
            text.setCharacterSize(m_settings.fontSize);
        }
        
        for (auto& text : m_hotspotTexts) {
            text.setFillColor(m_settings.warningColor);
            text.setCharacterSize(m_settings.fontSize);
        }
        
        // Resize graph data if needed
        if (m_settings.graphWidth != m_graphData.size()) {
            m_graphData.resize(m_settings.graphWidth, 0.0f);
        }
        
        updateLayout();
    }
}

bool PerformanceOverlay::handleEvent(const sf::Event& event) {
    if (!m_visible || !m_initialized) {
        return false;
    }
    
    // Handle mouse events for dragging
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // Check if click is within the overlay area
        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
        if (m_background.getGlobalBounds().contains(mousePos)) {
            m_dragging = true;
            m_dragOffset = m_position - mousePos;
            return true;
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        m_dragging = false;
    }
    else if (event.type == sf::Event::MouseMoved && m_dragging) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        setPosition(mousePos + m_dragOffset);
        return true;
    }
    
    return false;
}

void PerformanceOverlay::setPosition(const sf::Vector2f& position) {
    m_position = position;
    updateLayout();
}

void PerformanceOverlay::addHotspotScope(const std::string& name) {
    // Check if already in the list
    auto it = std::find(m_hotspotScopes.begin(), m_hotspotScopes.end(), name);
    if (it == m_hotspotScopes.end()) {
        m_hotspotScopes.push_back(name);
    }
}

void PerformanceOverlay::clearHotspots() {
    m_hotspotScopes.clear();
    m_hotspotTexts.clear();
}

void PerformanceOverlay::createTexts() {
    // Set up title text
    m_titleText.setFont(m_font);
    m_titleText.setString("Performance Monitor");
    m_titleText.setCharacterSize(m_settings.fontSize + 2);
    m_titleText.setFillColor(m_settings.textColor);
    
    // Set up FPS text
    m_fpsText.setFont(m_font);
    m_fpsText.setCharacterSize(m_settings.fontSize);
    m_fpsText.setFillColor(m_settings.textColor);
    
    // Set up frame time text
    m_frameTimeText.setFont(m_font);
    m_frameTimeText.setCharacterSize(m_settings.fontSize);
    m_frameTimeText.setFillColor(m_settings.textColor);
    
    // Set up memory text
    m_memoryText.setFont(m_font);
    m_memoryText.setCharacterSize(m_settings.fontSize);
    m_memoryText.setFillColor(m_settings.textColor);
    
    // Set up consumer texts
    m_consumerTexts.resize(m_settings.numTopConsumers);
    for (auto& text : m_consumerTexts) {
        text.setFont(m_font);
        text.setCharacterSize(m_settings.fontSize);
        text.setFillColor(m_settings.textColor);
    }
    
    // Set up hotspot texts
    m_hotspotTexts.clear();
}

void PerformanceOverlay::updateTexts() {
    // Update FPS text
    double avgFrameTime, minFrameTime, maxFrameTime, stdDev;
    ProfilingManager::getInstance().getFrameStats(avgFrameTime, minFrameTime, maxFrameTime, stdDev);
    
    double fps = avgFrameTime > 0.0 ? 1000.0 / avgFrameTime : 0.0;
    m_fpsText.setString("FPS: " + std::to_string(static_cast<int>(fps)));
    
    // Set color based on frame rate
    if (avgFrameTime > m_settings.criticalThresholdMs) {
        m_fpsText.setFillColor(m_settings.criticalColor);
    }
    else if (avgFrameTime > m_settings.warningThresholdMs) {
        m_fpsText.setFillColor(m_settings.warningColor);
    }
    else {
        m_fpsText.setFillColor(m_settings.textColor);
    }
    
    // Update frame time text
    std::stringstream frameTimeStr;
    frameTimeStr << "Frame Time: " << std::fixed << std::setprecision(2) << avgFrameTime << " ms";
    if (m_settings.displayMode == OverlayDisplayMode::Detailed || 
        m_settings.displayMode == OverlayDisplayMode::Full) {
        frameTimeStr << " (Min: " << std::fixed << std::setprecision(2) << minFrameTime
                     << " ms, Max: " << std::fixed << std::setprecision(2) << maxFrameTime << " ms)";
    }
    m_frameTimeText.setString(frameTimeStr.str());
    
    // Set color based on frame time
    if (avgFrameTime > m_settings.criticalThresholdMs) {
        m_frameTimeText.setFillColor(m_settings.criticalColor);
    }
    else if (avgFrameTime > m_settings.warningThresholdMs) {
        m_frameTimeText.setFillColor(m_settings.warningColor);
    }
    else {
        m_frameTimeText.setFillColor(m_settings.textColor);
    }
    
    // Update memory text
    size_t currentMemory = MemoryManager::getInstance().getCurrentMemoryUsage();
    size_t peakMemory = MemoryManager::getInstance().getPeakMemoryUsage();
    
    std::stringstream memoryStr;
    memoryStr << "Memory: " << (currentMemory / 1024) << " KB";
    if (m_settings.displayMode == OverlayDisplayMode::Detailed || 
        m_settings.displayMode == OverlayDisplayMode::Full) {
        memoryStr << " (Peak: " << (peakMemory / 1024) << " KB)";
    }
    m_memoryText.setString(memoryStr.str());
    
    // Update top consumers
    if (m_settings.showTopConsumers) {
        auto topConsumers = getTopConsumers();
        
        for (size_t i = 0; i < m_consumerTexts.size(); ++i) {
            if (i < topConsumers.size()) {
                const auto& consumer = topConsumers[i];
                std::stringstream ss;
                ss << consumer.first << ": " << std::fixed << std::setprecision(2) 
                   << consumer.second << " ms";
                m_consumerTexts[i].setString(ss.str());
                
                // Set color based on time
                if (consumer.second > m_settings.criticalThresholdMs) {
                    m_consumerTexts[i].setFillColor(m_settings.criticalColor);
                }
                else if (consumer.second > m_settings.warningThresholdMs) {
                    m_consumerTexts[i].setFillColor(m_settings.warningColor);
                }
                else {
                    m_consumerTexts[i].setFillColor(m_settings.textColor);
                }
            }
            else {
                m_consumerTexts[i].setString("");
            }
        }
    }
    
    // Update hotspots
    if (m_settings.showHotspots) {
        detectHotspots();
    }
}

void PerformanceOverlay::updateGraph() {
    // Shift data to the left
    for (size_t i = 0; i < m_graphData.size() - 1; ++i) {
        m_graphData[i] = m_graphData[i + 1];
    }
    
    // Add new data point
    m_graphData[m_graphData.size() - 1] = getGraphValue();
    
    // Find min and max for scaling
    float minValue = m_settings.autoScaleGraph ? std::numeric_limits<float>::max() : 0.0f;
    float maxValue = m_settings.autoScaleGraph ? std::numeric_limits<float>::min() : 60.0f; // Default to 60ms max for frame time
    
    for (float value : m_graphData) {
        if (value > 0.0f) {
            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
        }
    }
    
    if (minValue == std::numeric_limits<float>::max()) {
        minValue = 0.0f;
    }
    
    if (maxValue == std::numeric_limits<float>::min() || maxValue <= minValue) {
        maxValue = minValue + 1.0f;
    }
    
    // Create graph vertices
    m_graphVertices.clear();
    m_graphVertices.reserve(m_graphData.size() * 2);
    
    float graphWidth = static_cast<float>(m_settings.graphWidth);
    float graphHeight = static_cast<float>(m_settings.graphHeight);
    
    for (size_t i = 0; i < m_graphData.size(); ++i) {
        float value = m_graphData[i];
        float x = static_cast<float>(i) / (m_graphData.size() - 1) * graphWidth;
        float y = graphHeight - ((value - minValue) / (maxValue - minValue) * graphHeight);
        
        sf::Color color = m_settings.graphColor;
        
        // Color based on thresholds for frame time graph
        if (m_settings.graphType == GraphType::FrameTime || m_settings.graphType == GraphType::FrameTimeHistory) {
            if (value > m_settings.criticalThresholdMs) {
                color = m_settings.criticalColor;
            }
            else if (value > m_settings.warningThresholdMs) {
                color = m_settings.warningColor;
            }
        }
        
        m_graphVertices.emplace_back(sf::Vector2f(x, y), color);
    }
}

void PerformanceOverlay::updateLayout() {
    if (!m_initialized) {
        return;
    }
    
    float totalHeight = 0.0f;
    float width = 300.0f;
    
    // Calculate total height based on visible elements
    totalHeight += m_settings.padding + m_titleText.getLocalBounds().height;
    
    if (m_settings.showFps) {
        totalHeight += m_settings.padding + m_fpsText.getLocalBounds().height;
    }
    
    if (m_settings.showFrameTime) {
        totalHeight += m_settings.padding + m_frameTimeText.getLocalBounds().height;
    }
    
    if (m_settings.showMemory) {
        totalHeight += m_settings.padding + m_memoryText.getLocalBounds().height;
    }
    
    if (m_settings.showTopConsumers) {
        for (const auto& text : m_consumerTexts) {
            if (!text.getString().isEmpty()) {
                totalHeight += m_settings.padding / 2.0f + text.getLocalBounds().height;
            }
        }
    }
    
    if (m_settings.showHotspots) {
        for (const auto& text : m_hotspotTexts) {
            if (!text.getString().isEmpty()) {
                totalHeight += m_settings.padding / 2.0f + text.getLocalBounds().height;
            }
        }
    }
    
    if (m_settings.showGraph) {
        totalHeight += m_settings.padding * 2 + m_settings.graphHeight;
        width = std::max(width, static_cast<float>(m_settings.graphWidth + m_settings.padding * 2));
    }
    
    // Update background size and position
    m_background.setSize(sf::Vector2f(width, totalHeight));
    m_background.setPosition(m_position);
    
    // Position text elements
    float y = m_position.y + m_settings.padding;
    
    m_titleText.setPosition(m_position.x + m_settings.padding, y);
    y += m_titleText.getLocalBounds().height + m_settings.padding;
    
    if (m_settings.showFps) {
        m_fpsText.setPosition(m_position.x + m_settings.padding, y);
        y += m_fpsText.getLocalBounds().height + m_settings.padding;
    }
    
    if (m_settings.showFrameTime) {
        m_frameTimeText.setPosition(m_position.x + m_settings.padding, y);
        y += m_frameTimeText.getLocalBounds().height + m_settings.padding;
    }
    
    if (m_settings.showMemory) {
        m_memoryText.setPosition(m_position.x + m_settings.padding, y);
        y += m_memoryText.getLocalBounds().height + m_settings.padding;
    }
    
    if (m_settings.showTopConsumers) {
        for (auto& text : m_consumerTexts) {
            if (!text.getString().isEmpty()) {
                text.setPosition(m_position.x + m_settings.padding, y);
                y += text.getLocalBounds().height + m_settings.padding / 2.0f;
            }
        }
    }
    
    if (m_settings.showHotspots) {
        for (auto& text : m_hotspotTexts) {
            if (!text.getString().isEmpty()) {
                text.setPosition(m_position.x + m_settings.padding, y);
                y += text.getLocalBounds().height + m_settings.padding / 2.0f;
            }
        }
    }
    
    if (m_settings.showGraph) {
        m_graphBackground.setSize(sf::Vector2f(m_settings.graphWidth, m_settings.graphHeight));
        m_graphBackground.setPosition(m_position.x + m_settings.padding, y);
    }
}

void PerformanceOverlay::drawGraph(sf::RenderTarget& target) {
    if (m_graphVertices.empty()) {
        return;
    }
    
    // Draw graph background
    target.draw(m_graphBackground);
    
    // Draw threshold lines
    if (m_settings.graphType == GraphType::FrameTime || m_settings.graphType == GraphType::FrameTimeHistory) {
        float graphHeight = static_cast<float>(m_settings.graphHeight);
        float graphWidth = static_cast<float>(m_settings.graphWidth);
        
        // Find min and max for scaling
        float minValue = std::numeric_limits<float>::max();
        float maxValue = std::numeric_limits<float>::min();
        
        for (float value : m_graphData) {
            if (value > 0.0f) {
                minValue = std::min(minValue, value);
                maxValue = std::max(maxValue, value);
            }
        }
        
        if (minValue == std::numeric_limits<float>::max()) {
            minValue = 0.0f;
        }
        
        if (maxValue == std::numeric_limits<float>::min() || maxValue <= minValue) {
            maxValue = minValue + 1.0f;
        }
        
        // Draw warning threshold line
        if (m_settings.warningThresholdMs >= minValue && m_settings.warningThresholdMs <= maxValue) {
            float y = m_graphBackground.getPosition().y + graphHeight - 
                ((m_settings.warningThresholdMs - minValue) / (maxValue - minValue) * graphHeight);
            
            sf::VertexArray line(sf::Lines, 2);
            line[0] = sf::Vertex(sf::Vector2f(m_graphBackground.getPosition().x, y), m_settings.warningColor);
            line[1] = sf::Vertex(sf::Vector2f(m_graphBackground.getPosition().x + graphWidth, y), m_settings.warningColor);
            target.draw(line);
        }
        
        // Draw critical threshold line
        if (m_settings.criticalThresholdMs >= minValue && m_settings.criticalThresholdMs <= maxValue) {
            float y = m_graphBackground.getPosition().y + graphHeight - 
                ((m_settings.criticalThresholdMs - minValue) / (maxValue - minValue) * graphHeight);
            
            sf::VertexArray line(sf::Lines, 2);
            line[0] = sf::Vertex(sf::Vector2f(m_graphBackground.getPosition().x, y), m_settings.criticalColor);
            line[1] = sf::Vertex(sf::Vector2f(m_graphBackground.getPosition().x + graphWidth, y), m_settings.criticalColor);
            target.draw(line);
        }
    }
    
    // Draw graph lines
    sf::VertexArray lines(sf::LineStrip, m_graphVertices.size());
    for (size_t i = 0; i < m_graphVertices.size(); ++i) {
        lines[i] = sf::Vertex(
            sf::Vector2f(m_graphBackground.getPosition().x + m_graphVertices[i].position.x,
                        m_graphBackground.getPosition().y + m_graphVertices[i].position.y),
            m_graphVertices[i].color
        );
    }
    target.draw(lines);
}

float PerformanceOverlay::getGraphValue() {
    switch (m_settings.graphType) {
        case GraphType::FrameTime:
        case GraphType::FrameTimeHistory: {
            double avgFrameTime, minFrameTime, maxFrameTime, stdDev;
            ProfilingManager::getInstance().getFrameStats(avgFrameTime, minFrameTime, maxFrameTime, stdDev, 1);
            return static_cast<float>(avgFrameTime);
        }
        
        case GraphType::Memory: {
            size_t currentMemory = MemoryManager::getInstance().getCurrentMemoryUsage();
            return static_cast<float>(currentMemory) / 1024.0f; // Convert to KB
        }
        
        case GraphType::Custom: {
            if (m_customDataSource) {
                return m_customDataSource();
            }
            return 0.0f;
        }
        
        default:
            return 0.0f;
    }
}

std::vector<std::pair<std::string, double>> PerformanceOverlay::getTopConsumers() const {
    std::vector<std::pair<std::string, double>> result;
    
    // Get all metrics
    auto metrics = ProfilingManager::getInstance().getAllMetrics();
    
    // Sort by total time
    std::sort(metrics.begin(), metrics.end(), 
              [](const ProfileMetrics& a, const ProfileMetrics& b) {
                  return a.lastTimeMs > b.lastTimeMs;
              });
    
    // Take top N
    int count = 0;
    for (const auto& metric : metrics) {
        if (count >= m_settings.numTopConsumers) {
            break;
        }
        
        result.emplace_back(metric.name, metric.lastTimeMs);
        count++;
    }
    
    return result;
}

void PerformanceOverlay::detectHotspots() {
    // Clear existing hotspot texts
    m_hotspotTexts.clear();
    
    // Check user-defined hotspot scopes
    for (const auto& scope : m_hotspotScopes) {
        const auto* metrics = ProfilingManager::getInstance().getMetrics(scope);
        if (metrics && metrics->lastTimeMs > m_settings.warningThresholdMs) {
            sf::Text text;
            text.setFont(m_font);
            text.setCharacterSize(m_settings.fontSize);
            
            std::stringstream ss;
            ss << "Hotspot: " << metrics->name << " (" << std::fixed << std::setprecision(2) 
               << metrics->lastTimeMs << " ms)";
            text.setString(ss.str());
            
            // Set color based on severity
            if (metrics->lastTimeMs > m_settings.criticalThresholdMs) {
                text.setFillColor(m_settings.criticalColor);
            }
            else {
                text.setFillColor(m_settings.warningColor);
            }
            
            m_hotspotTexts.push_back(text);
        }
    }
    
    // Check for budget exceeded metrics
    auto metrics = ProfilingManager::getInstance().getAllMetrics();
    for (const auto& metric : metrics) {
        if (metric.budgetMs > 0.0 && metric.budgetExceededCount > 0 && 
            metric.lastTimeMs > metric.budgetMs) {
            
            sf::Text text;
            text.setFont(m_font);
            text.setCharacterSize(m_settings.fontSize);
            
            std::stringstream ss;
            ss << "Budget Exceeded: " << metric.name << " (" << std::fixed << std::setprecision(2) 
               << metric.lastTimeMs << " ms, budget: " << metric.budgetMs << " ms)";
            text.setString(ss.str());
            
            // Set color based on how much the budget was exceeded
            double ratio = metric.lastTimeMs / metric.budgetMs;
            if (ratio > 2.0) {
                text.setFillColor(m_settings.criticalColor);
            }
            else {
                text.setFillColor(m_settings.warningColor);
            }
            
            m_hotspotTexts.push_back(text);
        }
    }
    
    // Update layout if hotspot texts have been added
    if (!m_hotspotTexts.empty()) {
        updateLayout();
    }
}

} // namespace Performance