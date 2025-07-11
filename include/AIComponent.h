// AIComponent.h
#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <string>
#include <iostream>

class AIStrategy;
class PlayerEntity;
class PhysicsComponent;
class Transform;

/**
 * AIComponent - Manages AI behavior using Strategy Pattern
 * Enhanced to work with specialized template methods in Entity class
 */
class AIComponent