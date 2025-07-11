#include "Scripting/ScriptManager.h"
#include "Scripting/ScriptComponent.h"
#include "Scripting/LuaScriptEngine.h"
#include "Scripting/ScriptBindingGenerator.h"
#include "Scripting/ScriptValidator.h"
#include "Scripting/ScriptDebugger.h"
#include "Scripting/ScriptPerformanceMonitor.h"
#include "GameSession.h"
#include "Entity.h"
#include "PhysicsComponent.h"
#include "RenderComponent.h"
#include "Transform.h"
#include "Exceptions/GameExceptions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

namespace fs = std::filesystem;

// Sample entity for demo
class PlayerEntity : public Entity {
public:
    PlayerEntity(IdType id) : Entity(id), m_health(100), m_maxHealth(100), m_speed(5.0f) {}
    
    void setHealth(int health) { m_health = std::min(health, m_maxHealth); }
    int getHealth() const { return m_health; }
    
    void setMaxHealth(int maxHealth) { m_maxHealth = maxHealth; }
    int getMaxHealth() const { return m_maxHealth; }
    
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    
    void takeDamage(int amount) {
        m_health = std::max(0, m_health - amount);
        std::cout << "Player took " << amount << " damage. Health: " << m_health << std::endl;
    }
    
    void heal(int amount) {
        m_health = std::min(m_maxHealth, m_health + amount);
        std::cout << "Player healed " << amount << " health. Health: " << m_health << std::endl;
    }
    
private:
    int m_health;
    int m_maxHealth;
    float m_speed;
};

// Demo for the scripting system
class ScriptingDemo {
public:
    // Initialize the demo
    static bool initialize(GameSession* gameSession) {
        std::cout << "Initializing Scripting Demo..." << std::endl;
        
        try {
            // Create scripts directory if it doesn't exist
            fs::path scriptsDir = "scripts";
            if (!fs::exists(scriptsDir)) {
                fs::create_directory(scriptsDir);
            }
            
            // Initialize script manager
            auto& scriptManager = Scripting::ScriptManager::getInstance();
            if (!scriptManager.initialize(gameSession)) {
                std::cerr << "Failed to initialize script manager - Lua might not be available" << std::endl;
                return false;
            }
            
            // Check if we have a valid script engine
            if (!scriptManager.getDefaultEngine()) {
                std::cerr << "No script engine available - Lua library may not be properly linked" << std::endl;
                return false;
            }
            
            // Initialize script validator
            auto& validator = Scripting::ScriptValidator::getInstance();
            validator.initialize(scriptManager.getDefaultEngine());
            
            // Initialize script debugger
            auto& debugger = Scripting::ScriptDebugger::getInstance();
            debugger.initialize(gameSession);
            debugger.setScriptEngine(scriptManager.getDefaultEngine());
            
            // Initialize performance monitor
            auto& perfMonitor = Scripting::ScriptPerformanceMonitor::getInstance();
            perfMonitor.initialize();
            perfMonitor.setEnabled(true);
            perfMonitor.setSamplingInterval(0.5f);
            
            // Register performance alert callback
            perfMonitor.registerAlertCallback([](const Scripting::ScriptPerformanceMonitor::PerformanceAlert& alert) {
                std::cout << "Performance Alert: " << alert.message << std::endl;
            });
            
            // Create sample scripts
            createSampleScripts();
            
            // Register script bindings
            registerScriptBindings();
            
            // Enable hot-reloading
            scriptManager.enableHotReload(true, 1.0f);
            scriptManager.addScriptDirectory("scripts", true);
            
            // Register file change callback
            scriptManager.registerFileChangeCallback([](const Scripting::ScriptFileChangeEvent& event) {
                if (event.isNew) {
                    std::cout << "New script detected: " << event.filename << std::endl;
                }
                else if (event.isModified) {
                    std::cout << "Script modified: " << event.filename << std::endl;
                }
                else if (event.isDeleted) {
                    std::cout << "Script deleted: " << event.filename << std::endl;
                }
            });
            
            // Register error handler
            scriptManager.setErrorHandler([](const std::string& scriptName, int line, const std::string& error) {
                std::cout << "Script Error in " << scriptName << " at line " << line << ": " << error << std::endl;
            });
            
            // Load all scripts from the directory
            int scriptsLoaded = scriptManager.loadScriptsFromDirectory("scripts");
            std::cout << "Loaded " << scriptsLoaded << " scripts" << std::endl;
            
            std::cout << "Scripting Demo initialized" << std::endl;
            return true;
            
        } catch (const GameExceptions::Exception& ex) {
            std::cerr << "Game exception during scripting demo initialization: " << ex.what() << std::endl;
            return false;
        } catch (const std::exception& ex) {
            std::cerr << "Standard exception during scripting demo initialization: " << ex.what() << std::endl;
            return false;
        } catch (...) {
            std::cerr << "Unknown exception during scripting demo initialization" << std::endl;
            return false;
        }
    }
    
    // Run the demo
    static void run(GameSession* gameSession) {
        std::cout << "Running Scripting Demo..." << std::endl;
        
        // Check that initialization was successful
        auto& scriptManager = Scripting::ScriptManager::getInstance();
        if (!scriptManager.isInitialized()) {
            std::cerr << "Cannot run demo - Script manager not initialized" << std::endl;
            return;
        }
        
        auto& debugger = Scripting::ScriptDebugger::getInstance();
        auto& perfMonitor = Scripting::ScriptPerformanceMonitor::getInstance();
        
        try {
            // Create a demo entity
            PlayerEntity player(1);
            
            // Add some components
            Transform* transform = player.addComponent<Transform>();
            transform->setPosition(100.0f, 100.0f);
            
            PhysicsComponent* physics = player.addComponent<PhysicsComponent>();
            physics->setVelocity(0.0f, 0.0f);
            
            RenderComponent* render = player.addComponent<RenderComponent>();
            
            // Add script component
            Scripting::ScriptComponent* script = player.addComponent<Scripting::ScriptComponent>();
            script->addScriptSource("scripts/player.lua");
            script->initialize();
            
            // Expose player to script
            scriptManager.getDefaultEngine()->exposeEntity(&player, "player");
            
            // Create window for visualization
            sf::RenderWindow window(sf::VideoMode(800, 600), "Scripting Demo");
            window.setFramerateLimit(60);
            
            sf::Font font;
            if (!font.loadFromFile("resources/fonts/arial.ttf")) {
                std::cout << "Warning: Failed to load font, using system default" << std::endl;
                // Use a fallback method to display text
            }
            
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(14);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(10, 10);
            
            sf::Clock clock;
            
            // Main loop
            while (window.isOpen()) {
                float deltaTime = clock.restart().asSeconds();
                
                // Handle events
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    else if (event.type == sf::Event::KeyPressed) {
                        // Toggle debugger with F5
                        if (event.key.code == sf::Keyboard::F5) {
                            debugger.toggleVisibility();
                        }
                        // Reload scripts with F6
                        else if (event.key.code == sf::Keyboard::F6) {
                            script->reloadScripts();
                            std::cout << "Reloaded scripts" << std::endl;
                        }
                        // Toggle profiling with F7
                        else if (event.key.code == sf::Keyboard::F7) {
                            bool enabled = !perfMonitor.isEnabled();
                            perfMonitor.setEnabled(enabled);
                            std::cout << "Performance monitoring " << (enabled ? "enabled" : "disabled") << std::endl;
                        }
                        // Export performance data with F8
                        else if (event.key.code == sf::Keyboard::F8) {
                            perfMonitor.exportStatsToCSV("script_performance.csv");
                            std::cout << "Exported performance data to script_performance.csv" << std::endl;
                        }
                        // Validate scripts with F9
                        else if (event.key.code == sf::Keyboard::F9) {
                            auto& validator = Scripting::ScriptValidator::getInstance();
                            auto results = validator.validateDirectory("scripts", true);
                            std::cout << "Validated " << results.size() << " scripts" << std::endl;
                            for (const auto& [filename, errors] : results) {
                                if (!errors.empty()) {
                                    std::cout << "Errors in " << filename << ":" << std::endl;
                                    for (const auto& error : errors) {
                                        std::cout << "  Line " << error.line << ": " << error.message << std::endl;
                                    }
                                }
                            }
                        }
                        
                        // Handle debugger keyboard