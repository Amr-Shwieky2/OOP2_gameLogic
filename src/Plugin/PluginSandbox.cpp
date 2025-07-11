#include "Plugin/PluginSandbox.h"
#include <iostream>

PluginSandbox::PluginSandbox(std::unique_ptr<IPlugin> plugin, const std::string& name)
    : m_plugin(std::move(plugin)), m_name(name)
{
}

PluginSandbox::~PluginSandbox() {
    try {
        // Ensure plugin is shut down properly
        if (m_plugin) {
            m_plugin->shutdown();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during plugin sandbox destruction: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception during plugin sandbox destruction" << std::endl;
    }
}

bool PluginSandbox::initialize(GameSession* gameSession) {
    return trySafely("initialize", [this, gameSession]() {
        return m_plugin->initialize(gameSession);
    });
}

bool PluginSandbox::update(float deltaTime) {
    return trySafely("update", [this, deltaTime]() {
        m_plugin->update(deltaTime);
        return true;
    });
}

bool PluginSandbox::render(sf::RenderWindow& window) {
    return trySafely("render", [this, &window]() {
        m_plugin->render(window);
        return true;
    });
}

bool PluginSandbox::shutdown() {
    return trySafely("shutdown", [this]() {
        return m_plugin->shutdown();
    });
}

template<typename Func>
bool PluginSandbox::trySafely(const std::string& operation, Func&& func) {
    // Don't execute if plugin is in error state
    if (m_hasError) {
        return false;
    }
    
    try {
        return func();
    }
    catch (const std::exception& e) {
        reportError(operation, e.what());
        return false;
    }
    catch (...) {
        reportError(operation, "Unknown exception");
        return false;
    }
}

void PluginSandbox::reportError(const std::string& operation, const std::string& error) {
    m_hasError = true;
    m_lastError = error;
    
    std::cerr << "Plugin [" << m_name << "] error during " << operation << ": " << error << std::endl;
    
    if (m_errorCallback) {
        m_errorCallback(m_name, error);
    }
}