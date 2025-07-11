#include "Plugin/PluginBase.h"
#include <iostream>

PluginBase::PluginBase(
    const std::string& name,
    const PluginVersion& version,
    const std::string& description,
    const std::string& author,
    const std::string& category,
    bool supportsHotReload
) : 
    m_name(name),
    m_version(version),
    m_description(description),
    m_author(author),
    m_category(category),
    m_supportsHotReload(supportsHotReload)
{
    // Set default compatibility checker
    m_compatibilityChecker = [this](const PluginVersion& gameVersion) {
        return m_version.isCompatibleWith(gameVersion);
    };
}

bool PluginBase::initialize(GameSession* gameSession) {
    m_gameSession = gameSession;
    std::cout << "Initializing plugin: " << m_name << " v" << m_version.toString() << std::endl;
    return true;
}

void PluginBase::update(float deltaTime) {
    if (m_updateCallback) {
        m_updateCallback(deltaTime);
    }
}

void PluginBase::render(sf::RenderWindow& window) {
    if (m_renderCallback) {
        m_renderCallback(window);
    }
}

bool PluginBase::shutdown() {
    std::cout << "Shutting down plugin: " << m_name << std::endl;
    m_gameSession = nullptr;
    return true;
}

bool PluginBase::isCompatible(const PluginVersion& gameVersion) const {
    return m_compatibilityChecker(gameVersion);
}

std::vector<std::string> PluginBase::getDependencies() const {
    return m_dependencies;
}