#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Property/PropertyValue.h"
#include "Property/PropertyDescriptor.h"
#include "Property/PropertyContainer.h"
#include "Property/PropertyManager.h"
#include "Property/UndoManager.h"
#include <string>

// Mock for property changed notifications
class MockPropertyListener {
public:
    MOCK_METHOD(void, onPropertyChanged, (const std::string&, const PropertyValue&, const PropertyValue&));
};

// Custom property container for testing
class TestPropertyContainer : public PropertyContainer {
public:
    TestPropertyContainer() {
        registerProperties();
    }
    
    void registerProperties() override {
        registerProperty("intProperty", 42, "Integer property");
        registerProperty("floatProperty", 3.14f, "Float property");
        registerProperty("stringProperty", std::string("test"), "String property");
        registerProperty("boolProperty", true, "Boolean property");
    }
    
    // Implement mock notification handler
    void onPropertyChanged(const std::string& name, const PropertyValue& oldValue, const PropertyValue& newValue) override {
        PropertyContainer::onPropertyChanged(name, oldValue, newValue);
        
        if (mockListener) {
            mockListener->onPropertyChanged(name, oldValue, newValue);
        }
    }
    
    // Set mock listener
    void setMockListener(MockPropertyListener* listener) {
        mockListener = listener;
    }
    
private:
    MockPropertyListener* mockListener = nullptr;
};

// Test fixture for property system
class PropertySystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test container
        container = std::make_unique<TestPropertyContainer>();
        
        // Set up mock listener
        container->setMockListener(&mockListener);
        
        // Initialize property manager
        propertyManager = &PropertyManager::getInstance();
        
        // Reset undo manager
        UndoManager::getInstance().clear();
    }
    
    std::unique_ptr<TestPropertyContainer> container;
    MockPropertyListener mockListener;
    PropertyManager* propertyManager = nullptr;
};

// Test property registration and access
TEST_F(PropertySystemTest, PropertyRegistrationAndAccess) {
    // Check registered properties
    EXPECT_TRUE(container->hasProperty("intProperty"));
    EXPECT_TRUE(container->hasProperty("floatProperty"));
    EXPECT_TRUE(container->hasProperty("stringProperty"));
    EXPECT_TRUE(container->hasProperty("boolProperty"));
    
    // Check property values
    EXPECT_EQ(42, container->getProperty("intProperty").toInt());
    EXPECT_FLOAT_EQ(3.14f, container->getProperty("floatProperty").toFloat());
    EXPECT_EQ("test", container->getProperty("stringProperty").toString());
    EXPECT_EQ(true, container->getProperty("boolProperty").toBool());
    
    // Check non-existent property
    EXPECT_FALSE(container->hasProperty("nonExistentProperty"));
    EXPECT_THROW(container->getProperty("nonExistentProperty"), std::runtime_error);
}

// Test property modification
TEST_F(PropertySystemTest, PropertyModification) {
    // Set up expectation for property change notification
    EXPECT_CALL(mockListener, onPropertyChanged("intProperty", 
                                               testing::Property(&PropertyValue::toInt, 42),
                                               testing::Property(&PropertyValue::toInt, 100)))
        .Times(1);
    
    // Modify property
    container->setProperty("intProperty", 100);
    
    // Check new value
    EXPECT_EQ(100, container->getProperty("intProperty").toInt());
    
    // Test other property types
    EXPECT_CALL(mockListener, onPropertyChanged("floatProperty", 
                                               testing::Property(&PropertyValue::toFloat, 3.14f),
                                               testing::Property(&PropertyValue::toFloat, 2.71f)))
        .Times(1);
    
    container->setProperty("floatProperty", 2.71f);
    EXPECT_FLOAT_EQ(2.71f, container->getProperty("floatProperty").toFloat());
    
    EXPECT_CALL(mockListener, onPropertyChanged("stringProperty", 
                                               testing::Property(&PropertyValue::toString, "test"),
                                               testing::Property(&PropertyValue::toString, "modified")))
        .Times(1);
    
    container->setProperty("stringProperty", std::string("modified"));
    EXPECT_EQ("modified", container->getProperty("stringProperty").toString());
    
    EXPECT_CALL(mockListener, onPropertyChanged("boolProperty", 
                                               testing::Property(&PropertyValue::toBool, true),
                                               testing::Property(&PropertyValue::toBool, false)))
        .Times(1);
    
    container->setProperty("boolProperty", false);
    EXPECT_EQ(false, container->getProperty("boolProperty").toBool());
}

// Test property descriptors
TEST_F(PropertySystemTest, PropertyDescriptors) {
    // Get descriptor
    const PropertyDescriptor* desc = container->getPropertyDescriptor("intProperty");
    ASSERT_NE(nullptr, desc);
    
    // Check descriptor details
    EXPECT_EQ("intProperty", desc->getName());
    EXPECT_EQ("Integer property", desc->getDescription());
    EXPECT_EQ(PropertyType::Int, desc->getType());
    
    // Check string property descriptor
    desc = container->getPropertyDescriptor("stringProperty");
    ASSERT_NE(nullptr, desc);
    EXPECT_EQ("stringProperty", desc->getName());
    EXPECT_EQ("String property", desc->getDescription());
    EXPECT_EQ(PropertyType::String, desc->getType());
}

// Test undo/redo system
TEST_F(PropertySystemTest, UndoRedoSystem) {
    auto& undoManager = UndoManager::getInstance();
    undoManager.setEnabled(true);
    
    // Make some changes with undo recording
    undoManager.beginGroup("Test Changes");
    container->setProperty("intProperty", 100);
    container->setProperty("stringProperty", std::string("modified"));
    undoManager.endGroup();
    
    // Verify changes
    EXPECT_EQ(100, container->getProperty("intProperty").toInt());
    EXPECT_EQ("modified", container->getProperty("stringProperty").toString());
    
    // Set up expectations for property change notifications during undo
    EXPECT_CALL(mockListener, onPropertyChanged("stringProperty", 
                                              testing::Property(&PropertyValue::toString, "modified"),
                                              testing::Property(&PropertyValue::toString, "test")))
        .Times(1);
    
    EXPECT_CALL(mockListener, onPropertyChanged("intProperty", 
                                              testing::Property(&PropertyValue::toInt, 100),
                                              testing::Property(&PropertyValue::toInt, 42)))
        .Times(1);
    
    // Undo changes
    EXPECT_TRUE(undoManager.canUndo());
    undoManager.undo();
    
    // Verify original values restored
    EXPECT_EQ(42, container->getProperty("intProperty").toInt());
    EXPECT_EQ("test", container->getProperty("stringProperty").toString());
    
    // Set up expectations for property change notifications during redo
    EXPECT_CALL(mockListener, onPropertyChanged("intProperty", 
                                              testing::Property(&PropertyValue::toInt, 42),
                                              testing::Property(&PropertyValue::toInt, 100)))
        .Times(1);
    
    EXPECT_CALL(mockListener, onPropertyChanged("stringProperty", 
                                              testing::Property(&PropertyValue::toString, "test"),
                                              testing::Property(&PropertyValue::toString, "modified")))
        .Times(1);
    
    // Redo changes
    EXPECT_TRUE(undoManager.canRedo());
    undoManager.redo();
    
    // Verify changes reapplied
    EXPECT_EQ(100, container->getProperty("intProperty").toInt());
    EXPECT_EQ("modified", container->getProperty("stringProperty").toString());
}

// Test property value conversions
TEST_F(PropertySystemTest, PropertyValueConversions) {
    // Integer conversions
    PropertyValue intValue(42);
    EXPECT_EQ(42, intValue.toInt());
    EXPECT_FLOAT_EQ(42.0f, intValue.toFloat());
    EXPECT_EQ("42", intValue.toString());
    EXPECT_EQ(true, intValue.toBool());  // Non-zero is true
    
    // Float conversions
    PropertyValue floatValue(3.14f);
    EXPECT_EQ(3, floatValue.toInt());    // Truncated
    EXPECT_FLOAT_EQ(3.14f, floatValue.toFloat());
    EXPECT_EQ("3.14", floatValue.toString());
    EXPECT_EQ(true, floatValue.toBool());
    
    // String conversions
    PropertyValue stringValue(std::string("42"));
    EXPECT_EQ(42, stringValue.toInt());  // String to int conversion
    EXPECT_FLOAT_EQ(42.0f, stringValue.toFloat());  // String to float conversion
    EXPECT_EQ("42", stringValue.toString());
    EXPECT_EQ(true, stringValue.toBool());  // Non-empty string is true
    
    // Bool conversions
    PropertyValue boolValue(true);
    EXPECT_EQ(1, boolValue.toInt());
    EXPECT_FLOAT_EQ(1.0f, boolValue.toFloat());
    EXPECT_EQ("true", boolValue.toString());
    EXPECT_EQ(true, boolValue.toBool());
}

// Test property manager 
TEST_F(PropertySystemTest, PropertyManager) {
    // Register our test container
    propertyManager->registerContainer("TestContainer", container.get());
    
    // Verify registration
    EXPECT_TRUE(propertyManager->hasContainer("TestContainer"));
    
    // Get container
    auto retrievedContainer = propertyManager->getContainer("TestContainer");
    EXPECT_EQ(container.get(), retrievedContainer);
    
    // Get property by path
    auto propValue = propertyManager->getProperty("TestContainer.intProperty");
    EXPECT_EQ(42, propValue.toInt());
    
    // Set property by path
    propertyManager->setProperty("TestContainer.intProperty", 200);
    EXPECT_EQ(200, container->getProperty("intProperty").toInt());
    
    // Unregister container
    propertyManager->unregisterContainer("TestContainer");
    EXPECT_FALSE(propertyManager->hasContainer("TestContainer"));
}

// Test property binding
class BindingTestPropertyContainer : public PropertyContainer {
public:
    BindingTestPropertyContainer(const std::string& name) : m_name(name) {
        registerProperties();
    }
    
    void registerProperties() override {
        registerProperty("value", 0, "Bindable value");
    }
    
    std::string getName() const { return m_name; }
    
private:
    std::string m_name;
};

TEST_F(PropertySystemTest, PropertyBinding) {
    // Create two containers
    auto containerA = std::make_unique<BindingTestPropertyContainer>("ContainerA");
    auto containerB = std::make_unique<BindingTestPropertyContainer>("ContainerB");
    
    // Register with property manager
    propertyManager->registerContainer("ContainerA", containerA.get());
    propertyManager->registerContainer("ContainerB", containerB.get());
    
    // Create binding
    PropertyBinding binding("ContainerA.value", "ContainerB.value");
    
    // Test one-way binding
    containerA->setProperty("value", 42);
    EXPECT_EQ(42, containerB->getProperty("value").toInt());
    
    // Test the other way
    containerB->setProperty("value", 100);
    EXPECT_EQ(100, containerA->getProperty("value").toInt());
    
    // Clean up
    propertyManager->unregisterContainer("ContainerA");
    propertyManager->unregisterContainer("ContainerB");
}

// Test property events
TEST_F(PropertySystemTest, PropertyEvents) {
    // Register property change event
    bool eventReceived = false;
    int newValue = 0;
    
    auto eventId = PropertyEvents::registerPropertyChangeHandler(
        "intProperty", [&eventReceived, &newValue](const PropertyValue& value) {
            eventReceived = true;
            newValue = value.toInt();
        });
    
    // Set property to trigger event
    container->setProperty("intProperty", 123);
    
    // Check event was triggered
    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(123, newValue);
    
    // Unregister event
    PropertyEvents::unregisterPropertyChangeHandler(eventId);
    
    // Reset flags
    eventReceived = false;
    newValue = 0;
    
    // Set property again - should not trigger event
    container->setProperty("intProperty", 456);
    
    // Check event was not triggered
    EXPECT_FALSE(eventReceived);
    EXPECT_EQ(0, newValue);
}