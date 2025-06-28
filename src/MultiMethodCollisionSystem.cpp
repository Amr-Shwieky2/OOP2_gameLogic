#include "MultiMethodCollisionSystem.h"
#include <iostream>

// Most of the implementation is in the header due to templates
// This file can contain any non-template implementations if needed

// Example: Debug function to print all registered handlers
void MultiMethodCollisionSystem::debugPrintHandlers() const {
    std::cout << "Registered collision handlers: " << m_handlers.size() << "\n";
    for (const auto& [key, handler] : m_handlers) {
        std::cout << "  " << key.first.name() << " <-> " << key.second.name() << "\n";
    }
}