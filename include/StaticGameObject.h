// StaticGameObject.h
#pragma once
#include "GameObject.h"

class StaticGameObject : public GameObject {
public:
    ~StaticGameObject() override = default;
    // No update function here
};
