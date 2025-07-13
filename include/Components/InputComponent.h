// ==========================================
// InputComponent.h - Optional future enhancement
// ==========================================


#pragma once
#include "Component.h"
#include "InputService.h"

class InputComponent : public Component {
public:
    void update(float dt) override;
    void setInputService(const InputService* input) { m_input = input; }

private:
    const InputService* m_input = nullptr;
};

// This would allow input handling to be completely component-based
// The PlayerEntity would just have an InputComponent instead of
// handling input directly
