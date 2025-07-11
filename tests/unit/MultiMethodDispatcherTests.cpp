#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MultiMethodDispatcher.h"

class MultiMethodDispatcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        dispatcher = &MultiMethodDispatcher::getInstance();
    }
    
    MultiMethodDispatcher* dispatcher = nullptr;
    
    // Test classes
    class TestObject1 {
    public:
        std::string getName() const { return "TestObject1"; }
    };
    
    class TestObject2 {
    public:
        std::string getName() const { return "TestObject2"; }
    };
    
    class TestObject3 {
    public:
        std::string getName() const { return "TestObject3"; }
    };
};

// Test handler registration and dispatch
TEST_F(MultiMethodDispatcherTest, RegisterAndDispatch) {
    // Create test variables
    bool handlerCalled = false;
    std::string obj1Name;
    std::string obj2Name;
    
    // Register handler
    dispatcher->registerHandler<TestObject1, TestObject2>("test",
        [&handlerCalled, &obj1Name, &obj2Name](TestObject1& obj1, TestObject2& obj2) {
            handlerCalled = true;
            obj1Name = obj1.getName();
            obj2Name = obj2.getName();
        }
    );
    
    // Create objects
    TestObject1 obj1;
    TestObject2 obj2;
    
    // Dispatch
    dispatcher->dispatch(obj1, obj2, "test");
    
    // Verify handler was called with correct objects
    EXPECT_TRUE(handlerCalled);
    EXPECT_EQ("TestObject1", obj1Name);
    EXPECT_EQ("TestObject2", obj2Name);
}

// Test dispatch with result
TEST_F(MultiMethodDispatcherTest, DispatchWithResult) {
    // Register handler with result
    dispatcher->registerHandlerWithResult<int, TestObject1, TestObject2>("calculate",
        [](TestObject1&, TestObject2&) -> int {
            return 42;
        },
        0 // Default value
    );
    
    // Create objects
    TestObject1 obj1;
    TestObject2 obj2;
    
    // Dispatch with result
    int result = dispatcher->dispatchWithResult<int>(obj1, obj2, "calculate");
    
    // Verify result
    EXPECT_EQ(42, result);
}

// Test missing handler
TEST_F(MultiMethodDispatcherTest, MissingHandler) {
    // Create objects of types not registered
    TestObject1 obj1;
    TestObject3 obj3;
    
    // Dispatch with unregistered types
    dispatcher->dispatch(obj1, obj3, "test");
    
    // No assertion needed - just making sure it doesn't crash
}

// Test handler overwrite
TEST_F(MultiMethodDispatcherTest, HandlerOverwrite) {
    // Register initial handler
    dispatcher->registerHandler<TestObject1, TestObject2>("overwrite",
        [](TestObject1&, TestObject2&) {
            FAIL() << "Original handler should not be called";
        }
    );
    
    // Register new handler with same key
    bool newHandlerCalled = false;
    dispatcher->registerHandler<TestObject1, TestObject2>("overwrite",
        [&newHandlerCalled](TestObject1&, TestObject2&) {
            newHandlerCalled = true;
        }
    );
    
    // Create objects
    TestObject1 obj1;
    TestObject2 obj2;
    
    // Dispatch
    dispatcher->dispatch(obj1, obj2, "overwrite");
    
    // Verify new handler was called
    EXPECT_TRUE(newHandlerCalled);
}