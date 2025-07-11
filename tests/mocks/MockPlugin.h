#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Plugin/IPlugin.h"
#include "Plugin/PluginVersion.h"

/**
 * Mock plugin for testing plugin system
 */
class MockPlugin : public IPlugin {
public:
    MockPlugin(const std::string& name = "MockPlugin") 
        : m_name(name), m_version(1, 0, 0) {}
    
    MOCK_METHOD(bool, initialize, (GameSession*), (override));
    MOCK_METHOD(bool, shutdown, (), (override));
    MOCK_METHOD(void, update, (float), (override));
    MOCK_METHOD(void, render, (sf::RenderWindow&), (override));
    
    // Default implementations for getters
    const std::string& getName() const override { return m_name; }
    const PluginVersion& getVersion() const override { return m_version; }
    const std::string& getDescription() const override { return m_description; }
    const std::string& getAuthor() const override { return m_author; }
    const std::string& getCategory() const override { return m_category; }
    
    std::vector<std::string> getDependencies() const override { return m_dependencies; }
    
    // Set plugin metadata for testing
    void setName(const std::string& name) { m_name = name; }
    void setVersion(const PluginVersion& version) { m_version = version; }
    void setDescription(const std::string& description) { m_description = description; }
    void setAuthor(const std::string& author) { m_author = author; }
    void setCategory(const std::string& category) { m_category = category; }
    void setDependencies(const std::vector<std::string>& dependencies) { m_dependencies = dependencies; }
    
private:
    std::string m_name;
    PluginVersion m_version;
    std::string m_description = "Mock plugin for testing";
    std::string m_author = "Test Author";
    std::string m_category = "Test";
    std::vector<std::string> m_dependencies;
};