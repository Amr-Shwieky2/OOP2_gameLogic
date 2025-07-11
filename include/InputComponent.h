// ==========================================
// InputComponent.h - Optional future enhancement
// ==========================================


#pragma once
#include "Component.h"

/**
 * InputComponent - Handles input for entities
 */
class InputComponent : public Component {
public:
    InputComponent() = default;
    virtual ~InputComponent() = default;
    
    // Called once per frame to process input
    void update(float dt) override;
    
    // Enable or disable input processing
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
private:
    bool m_enabled = true;
};

// This would allow input handling to be completely component-based
// The PlayerEntity would just have an InputComponent instead of
// handling input directly
