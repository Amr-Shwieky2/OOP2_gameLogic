#include "Performance/Profiler.h"
#include "Performance/PerformanceOverlay.h"
#include "Performance/MemoryTracker.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>

// Performance budget examples
namespace PerformanceBudgets {
    constexpr double PHYSICS_UPDATE_MS = 5.0;
    constexpr double RENDER_MS = 8.0;
    constexpr double AI_UPDATE_MS = 3.0;
    constexpr double FRAME_BUDGET_MS = 16.0; // ~60 FPS
}

class PerformanceDemo {
public:
    // Simulated workload for physics update
    void simulatePhysicsUpdate() {
        // Profile this function
        PROFILE_FUNCTION();
        
        // Track memory usage
        TRACK_MEMORY_SCOPE("PhysicsUpdate");
        
        // Allocate some objects to demonstrate memory tracking
        std::vector<float> positions(1000, 0.0f);
        std::vector<float> velocities(1000, 0.0f);
        
        // Simulate workload
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(1.0f, 10.0f);
        
        float workload = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(workload)));
        
        // More allocations
        std::vector<float> forces(1000, 0.0f);
        
        // Update physics (simulate work)
        for (int i = 0; i < 1000; ++i) {
            velocities[i] += forces[i] * 0.016f; // dt
            positions[i] += velocities[i] * 0.016f;
        }
    }
    
    // Simulated workload for rendering
    void simulateRender() {
        // Profile this function
        PROFILE_FUNCTION();
        
        // Nested profiling to demonstrate hierarchical profiling
        {
            PROFILE_SCOPE("PrepareRenderData");
            
            // Simulate workload
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(0.5f, 2.0f);
            
            float workload = dist(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(workload)));
        }
        
        // More nested profiling
        {
            PROFILE_SCOPE("RenderScene");
            
            // Simulate workload
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(2.0f, 6.0f);
            
            float workload = dist(gen);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(workload)));
        }
    }
    
    // Simulated workload for AI update
    void simulateAIUpdate() {
        // Profile this function
        PROFILE_FUNCTION();
        
        // Track memory usage
        TRACK_MEMORY_SCOPE("AIUpdate");
        
        // Simulate workload
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0.5f, 4.0f);
        
        float workload = dist(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(workload)));
        
        // Allocate some memory to demonstrate tracking
        std::vector<int> aiStates(100, 0);
        
        // Occasionally spike the CPU usage to demonstrate spike detection
        std::uniform_real_distribution<float> spikeDist(0.0f, 1.0f);
        if (spikeDist(gen) < 0.05f) { // 5% chance of spike
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    }
    
    // Simulated workload that's inefficient to demonstrate hotspot detection
    void simulateHotspot() {
        // Profile this function
        PROFILE_SCOPE_CAT("ExpensiveOperation", "Hotspot");
        
        // Simulate heavy workload
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        // Allocate a large amount of memory
        std::vector<double> largeAlloc(100000, 0.0);
    }
    
    // Run the demo
    void run() {
        // Set performance budgets
        Performance::ProfilingManager::getInstance().setPerformanceBudget("simulatePhysicsUpdate", PerformanceBudgets::PHYSICS_UPDATE_MS);
        Performance::ProfilingManager::getInstance().setPerformanceBudget("simulateRender", PerformanceBudgets::RENDER_MS);
        Performance::ProfilingManager::getInstance().setPerformanceBudget("simulateAIUpdate", PerformanceBudgets::AI_UPDATE_MS);
        
        // Create SFML window for visualization
        sf::RenderWindow window(sf::VideoMode(800, 600), "Performance Monitoring Demo");
        window.setFramerateLimit(60);
        
        // Create performance overlay
        Performance::OverlaySettings settings;
        settings.displayMode = Performance::OverlayDisplayMode::Full;
        settings.graphType = Performance::GraphType::FrameTime;
        
        Performance::PerformanceOverlay overlay(settings);
        if (!overlay.initialize()) {
            std::cerr << "Failed to initialize performance overlay" << std::endl;
            return;
        }
        
        // Add hotspot detection
        overlay.addHotspotScope("ExpensiveOperation");
        
        // Main loop
        while (window.isOpen()) {
            // Profile frame
            PROFILE_BEGIN_FRAME();
            
            // Handle events
            sf::Event event;
            while (window.pollEvent(event)) {
                // Close window on request
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                
                // Toggle overlay visibility with F3
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3) {
                    overlay.toggleVisibility();
                }
                
                // Change display mode with F4
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F4) {
                    int currentMode = static_cast<int>(settings.displayMode);
                    currentMode = (currentMode + 1) % 5; // 5 modes available
                    overlay.setDisplayMode(static_cast<Performance::OverlayDisplayMode>(currentMode));
                }
                
                // Change graph type with F5
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5) {
                    int currentType = static_cast<int>(settings.graphType);
                    currentType = (currentType + 1) % 3; // 3 graph types available
                    overlay.setGraphType(static_cast<Performance::GraphType>(currentType));
                }
                
                // Handle overlay input (for dragging)
                overlay.handleEvent(event);
            }
            
            // Update simulation
            simulatePhysicsUpdate();
            simulateAIUpdate();
            
            // Occasionally trigger a hotspot
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(gen) < 0.1f) { // 10% chance per frame
                simulateHotspot();
            }
            
            // Clear the window
            window.clear(sf::Color(50, 50, 50));
            
            // Render the scene
            simulateRender();
            
            // Update and draw overlay
            {
                PROFILE_SCOPE("PerformanceOverlay");
                overlay.update(1.0f / 60.0f); // Approximate delta time
                overlay.draw(window);
            }
            
            // Display the window
            window.display();
            
            // End frame profiling
            PROFILE_END_FRAME();
            
            // Every 300 frames, print metrics report
            static int frameCount = 0;
            if (++frameCount % 300 == 0) {
                Performance::ProfilingManager::getInstance().printMetrics();
                Performance::MemoryTracker::printStats();
                
                // Export metrics to CSV
                Performance::ProfilingManager::getInstance().exportMetricsToCSV("performance_metrics.csv");
                
                // Check for performance spikes
                int spikeCount = Performance::ProfilingManager::getInstance().detectPerformanceSpikes(2.0, 300);
                if (spikeCount > 0) {
                    std::cout << "Detected " << spikeCount << " performance spikes in the last 300 frames" << std::endl;
                }
            }
        }
    }
};

// Entry point for the performance demo
void runPerformanceDemo() {
    std::cout << "Starting Performance Monitoring Demo" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  F3 - Toggle overlay visibility" << std::endl;
    std::cout << "  F4 - Change display mode" << std::endl;
    std::cout << "  F5 - Change graph type" << std::endl;
    
    PerformanceDemo demo;
    demo.run();
}