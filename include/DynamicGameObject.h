// DynamicGameObject.h
#pragma once
#include "GameObject.h"

class DynamicGameObject : public GameObject {
public:
    ~DynamicGameObject() override = default;

    // All moving/interactive objects must implement update
    virtual void update(float deltaTime) = 0;
};
