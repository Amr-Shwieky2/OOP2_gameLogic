#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Plugin/IPlugin.h"
#include "Plugin/PluginVersion.h"
#include "Plugin/PluginBase.h"
#include "Plugin/PluginManager.h"
#include "Plugin/PluginFactory.h"
#include "mocks/MockPlugin.h"
#include <memory>

// Test fixture for plugin system
class PluginSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize plugin manager
        pluginManager = &PluginManager::getInstance();
        pluginManager->initialize();
    }
    
    void TearDown() override {
        // Clean up plugin manager
        pluginManager->shutdown();
    }
    
    // Test plugin class
    class TestPlugin : public PluginBase {
    public:
        TestPlugin(const std::string& name, const PluginVersion& version,
                  const std::string& description = "Test plugin",
                  const std::string& author = "Test Author")
            : PluginBase(name, version, description, author) {
        }
        
        bool initialize(GameSession* gameSession) override {
            m_isInitialized = true;
            return m_isInitialized;
        }
        
        bool shutdown() override {
            m_isInitialized = false;
            return true;
        }
        
        void update(float deltaTime) override {
            m_lastDeltaTime = deltaTime;
            m_updateCount++;
        }
        
        void render(sf::RenderWindow& window) override {
            m_renderCount++;
        }
        
        // Test-specific methods
        int getUpdateCount() const { return m_updateCount; }
        int getRenderCount() const { return m_renderCount; }
        float getLastDeltaTime() const { return m_lastDeltaTime; }
        
    private:
        bool m_isInitialized = false;
        int m_updateCount = 0;
        int m_renderCount = 0;
        float m_lastDeltaTime = 0.0f;
    };
    
    PluginManager* pluginManager = nullptr;
};

// Test PluginVersion functionality
TEST_F(PluginSystemTest, PluginVersion) {
    // Create plugin versions
    PluginVersion v1(1, 0, 0);
    PluginVersion v2(1, 1, 0);
    PluginVersion v3(2, 0, 0);
    PluginVersion v1Copy(1, 0, 0);
    
    // Test comparison operators
    EXPECT_LT(v1, v2);
    EXPECT_LT(v2, v3);
    EXPECT_GT(v3, v1);
    EXPECT_EQ(v1, v1Copy);
    EXPECT_NE(v1, v2);
    
    // Test string conversion
    EXPECT_EQ("1.0.0", v1.toString());
    EXPECT_EQ("1.1.0", v2.toString());
    EXPECT_EQ("2.0.0", v3.toString());
    
    // Test compatibility check
    EXPECT_TRUE(v1.isCompatible(v1)); // Same version
    EXPECT_TRUE(v1.isCompatible(v1Copy)); // Equal versions
    EXPECT_TRUE(v2.isCompatible(v1)); // Minor version higher
    EXPECT_FALSE(v3.isCompatible(v1)); // Major version higher
    
    // Test from string
    auto v4 = PluginVersion::fromString("3.4.5");
    EXPECT_EQ(3, v4.getMajor());
    EXPECT_EQ(4, v4.getMinor());
    EXPECT_EQ(5, v4.getPatch());
    EXPECT_EQ("3.4.5", v4.toString());
}

// Test plugin base class
TEST_F(PluginSystemTest, PluginBase) {
    // Create a test plugin
    PluginVersion version(1, 2, 3);
    TestPlugin plugin("TestPlugin", version, "Test description", "Test author");
    
    // Check plugin metadata
    EXPECT_EQ("TestPlugin", plugin.getName());
    EXPECT_EQ(version, plugin.getVersion());
    EXPECT_EQ("Test description", plugin.getDescription());
    EXPECT_EQ("Test author", plugin.getAuthor());
    EXPECT_EQ("Plugin", plugin.getCategory()); // Default category
    
    // Check empty dependencies
    auto deps = plugin.getDependencies();
    EXPECT_TRUE(deps.empty());
    
    // Test plugin lifecycle
    EXPECT_TRUE(plugin.initialize(nullptr));
    
    // Test update
    plugin.update(0.16f);
    EXPECT_EQ(1, plugin.getUpdateCount());
    EXPECT_FLOAT_EQ(0.16f, plugin.getLastDeltaTime());
    
    // Test render
    plugin.render(*(new sf::RenderWindow));
    EXPECT_EQ(1, plugin.getRenderCount());
    
    // Test shutdown
    EXPECT_TRUE(plugin.shutdown());
}

// Test plugin manager
TEST_F(PluginSystemTest, PluginManager) {
    // Create test plugins
    auto plugin1 = std::make_shared<TestPlugin>("Plugin1", PluginVersion(1, 0, 0));
    auto plugin2 = std::make_shared<TestPlugin>("Plugin2", PluginVersion(2, 0, 0));
    
    // Register plugins
    EXPECT_TRUE(pluginManager->registerPlugin(plugin1));
    EXPECT_TRUE(pluginManager->registerPlugin(plugin2));
    
    // Check plugin count
    EXPECT_EQ(2, pluginManager->getPluginCount());
    
    // Find plugins
    auto foundPlugin1 = pluginManager->findPlugin("Plugin1");
    auto foundPlugin2 = pluginManager->findPlugin("Plugin2");
    auto notFoundPlugin = pluginManager->findPlugin("NonExistentPlugin");
    
    EXPECT_EQ(plugin1, foundPlugin1);
    EXPECT_EQ(plugin2, foundPlugin2);
    EXPECT_EQ(nullptr, notFoundPlugin);
    
    // Update all plugins
    pluginManager->updateAll(0.1f);
    
    // Check update counts
    EXPECT_EQ(1, plugin1->getUpdateCount());
    EXPECT_EQ(1, plugin2->getUpdateCount());
    
    // Unregister a plugin
    EXPECT_TRUE(pluginManager->unregisterPlugin("Plugin1"));
    EXPECT_EQ(1, pluginManager->getPluginCount());
    EXPECT_EQ(nullptr, pluginManager->findPlugin("Plugin1"));
    EXPECT_EQ(plugin2, pluginManager->findPlugin("Plugin2"));
    
    // Unregister all plugins
    pluginManager->unregisterAllPlugins();
    EXPECT_EQ(0, pluginManager->getPluginCount());
}

// Test plugin factory
TEST_F(PluginSystemTest, PluginFactory) {
    // Register a plugin creator
    PluginFactory::registerCreator("TestPlugin", [](const std::string& name) -> std::shared_ptr<IPlugin> {
        return std::make_shared<TestPlugin>(name, PluginVersion(1, 0, 0));
    });
    
    // Create a plugin using factory
    auto plugin = PluginFactory::createPlugin("TestPlugin", "FactoryCreatedPlugin");
    
    // Check plugin was created correctly
    ASSERT_NE(nullptr, plugin);
    EXPECT_EQ("FactoryCreatedPlugin", plugin->getName());
    EXPECT_EQ(PluginVersion(1, 0, 0), plugin->getVersion());
    
    // Try creating a non-registered plugin
    auto nonExistent = PluginFactory::createPlugin("NonExistentType", "Test");
    EXPECT_EQ(nullptr, nonExistent);
}

// Test plugin dependency resolution
TEST_F(PluginSystemTest, PluginDependencyResolution) {
    // Create plugin with dependencies
    class DependentPlugin : public TestPlugin {
    public:
        DependentPlugin(const std::string& name, const PluginVersion& version)
            : TestPlugin(name, version) {}
        
        std::vector<std::string> getDependencies() const override {
            return {"BasePlugin1", "BasePlugin2"};
        }
    };
    
    // Create base plugins
    auto basePlugin1 = std::make_shared<TestPlugin>("BasePlugin1", PluginVersion(1, 0, 0));
    auto basePlugin2 = std::make_shared<TestPlugin>("BasePlugin2", PluginVersion(1, 0, 0));
    auto dependentPlugin = std::make_shared<DependentPlugin>("DependentPlugin", PluginVersion(1, 0, 0));
    
    // Register base plugins
    EXPECT_TRUE(pluginManager->registerPlugin(basePlugin1));
    EXPECT_TRUE(pluginManager->registerPlugin(basePlugin2));
    
    // Register dependent plugin
    EXPECT_TRUE(pluginManager->registerPlugin(dependentPlugin));
    
    // Check dependencies
    auto deps = dependentPlugin->getDependencies();
    EXPECT_EQ(2, deps.size());
    EXPECT_EQ("BasePlugin1", deps[0]);
    EXPECT_EQ("BasePlugin2", deps[1]);
    
    // Check dependency resolution
    auto loadOrder = pluginManager->getDependencyLoadOrder();
    
    // Base plugins should come before dependent plugin
    auto basePlugin1Pos = std::find(loadOrder.begin(), loadOrder.end(), basePlugin1->getName());
    auto basePlugin2Pos = std::find(loadOrder.begin(), loadOrder.end(), basePlugin2->getName());
    auto dependentPluginPos = std::find(loadOrder.begin(), loadOrder.end(), dependentPlugin->getName());
    
    ASSERT_NE(loadOrder.end(), basePlugin1Pos);
    ASSERT_NE(loadOrder.end(), basePlugin2Pos);
    ASSERT_NE(loadOrder.end(), dependentPluginPos);
    
    EXPECT_LT(std::distance(loadOrder.begin(), basePlugin1Pos), 
             std::distance(loadOrder.begin(), dependentPluginPos));
    EXPECT_LT(std::distance(loadOrder.begin(), basePlugin2Pos), 
             std::distance(loadOrder.begin(), dependentPluginPos));
}

// Test plugin events
TEST_F(PluginSystemTest, PluginEvents) {
    // Create test plugins
    auto plugin = std::make_shared<TestPlugin>("EventTestPlugin", PluginVersion(1, 0, 0));
    
    // Track event calls
    bool onRegisterCalled = false;
    bool onUnregisterCalled = false;
    
    // Register event handlers
    auto regHandler = pluginManager->registerEventHandler(PluginEvent::PluginRegistered,
        [&onRegisterCalled](const std::string& pluginName) {
            onRegisterCalled = true;
            EXPECT_EQ("EventTestPlugin", pluginName);
        });
        
    auto unregHandler = pluginManager->registerEventHandler(PluginEvent::PluginUnregistered,
        [&onUnregisterCalled](const std::string& pluginName) {
            onUnregisterCalled = true;
            EXPECT_EQ("EventTestPlugin", pluginName);
        });
    
    // Register plugin to trigger event
    EXPECT_TRUE(pluginManager->registerPlugin(plugin));
    EXPECT_TRUE(onRegisterCalled);
    
    // Unregister plugin to trigger event
    EXPECT_TRUE(pluginManager->unregisterPlugin("EventTestPlugin"));
    EXPECT_TRUE(onUnregisterCalled);
    
    // Unregister event handlers
    pluginManager->unregisterEventHandler(PluginEvent::PluginRegistered, regHandler);
    pluginManager->unregisterEventHandler(PluginEvent::PluginUnregistered, unregHandler);
    
    // Reset flags
    onRegisterCalled = false;
    onUnregisterCalled = false;
    
    // Register plugin again - should not trigger events
    EXPECT_TRUE(pluginManager->registerPlugin(plugin));
    EXPECT_FALSE(onRegisterCalled);
    
    // Clean up
    pluginManager->unregisterPlugin("EventTestPlugin");
}