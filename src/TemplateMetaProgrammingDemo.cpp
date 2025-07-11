#include "TemplateMetaProgrammingDemo.h"
#include "ComponentTraits.h"
#include "ComponentUtils.h"
#include "DependencyResolver.h"
#include "ComponentSFINAE.h"
#include "Entity.h"
#include "Component.h"
#include "Memory/MemoryPoolBase.h"
#include "Memory/ObjectPool.h"
#include "Memory/MemoryManager.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <random>
#include <typeinfo>
#include <string>
#include <cassert>

// Forward declaration for Box2D
class b2World;

// Simplified mock component classes for the demo
class MockTransform : public Component {
public:
    void setPosition(float x, float y) { m_x = x; m_y = y; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
private:
    float m_x = 0.0f;
    float m_y = 0.0f;
};

class MockRenderComponent : public Component {
public:
    bool initialize() { return true; }
    void synchronizeWithTransform() {}
};

class MockPhysics