#include "Property/UndoManager.h"
#include "Property/PropertyContainer.h"
#include "Property/PropertyEvents.h"
#include "../EventSystem.h"
#include <iostream>

// PropertyChangeAction implementation
PropertyChangeAction::PropertyChangeAction(
    PropertyContainer* container,
    const std::string& propertyName,
    const PropertyValue& oldValue,
    const PropertyValue& newValue,
    const std::string& description
) : m_container(container),
    m_propertyName(propertyName),
    m_oldValue(oldValue),
    m_newValue(newValue),
    m_description(description) {
}

void PropertyChangeAction::undo() {
    if (m_container && m_container->hasProperty(m_propertyName)) {
        m_container->setPropertyValue(m_propertyName, m_oldValue);
    }
}

void PropertyChangeAction::redo() {
    if (m_container && m_container->hasProperty(m_propertyName)) {
        m_container->setPropertyValue(m_propertyName, m_newValue);
    }
}

// MultiPropertyChangeAction implementation
MultiPropertyChangeAction::MultiPropertyChangeAction(const std::string& description)
    : m_description(description) {
}

void MultiPropertyChangeAction::addPropertyChange(
    PropertyContainer* container,
    const std::string& propertyName,
    const PropertyValue& oldValue,
    const PropertyValue& newValue
) {
    m_changes.push_back({container, propertyName, oldValue, newValue});
}

void MultiPropertyChangeAction::undo() {
    // Undo in reverse order
    for (auto it = m_changes.rbegin(); it != m_changes.rend(); ++it) {
        if (it->container && it->container->hasProperty(it->propertyName)) {
            it->container->setPropertyValue(it->propertyName, it->oldValue);
        }
    }
    
    // Fire a multi-property changed event
    if (!m_changes.empty() && m_changes[0].container) {
        // Use the first container for the event
        PropertyContainer* container = m_changes[0].container;
        
        // Find the container ID (this is a simplified approach)
        std::string containerId = "unknown";
        
        EventSystem::getInstance().publish(
            MultiPropertyChangedEvent(container, containerId)
        );
    }
}

void MultiPropertyChangeAction::redo() {
    // Redo in original order
    for (const auto& change : m_changes) {
        if (change.container && change.container->hasProperty(change.propertyName)) {
            change.container->setPropertyValue(change.propertyName, change.newValue);
        }
    }
    
    // Fire a multi-property changed event
    if (!m_changes.empty() && m_changes[0].container) {
        // Use the first container for the event
        PropertyContainer* container = m_changes[0].container;
        
        // Find the container ID (this is a simplified approach)
        std::string containerId = "unknown";
        
        EventSystem::getInstance().publish(
            MultiPropertyChangedEvent(container, containerId)
        );
    }
}

// UndoManager implementation
UndoManager& UndoManager::getInstance() {
    static UndoManager instance;
    return instance;
}

UndoManager::UndoManager() : m_currentBatch(nullptr) {
}

void UndoManager::recordPropertyChange(
    PropertyContainer* container,
    const std::string& propertyName,
    const PropertyValue& oldValue,
    const PropertyValue& newValue,
    const std::string& description
) {
    // Skip if values are the same
    if (oldValue == newValue) {
        return;
    }
    
    // Get the property descriptor
    auto descriptor = container->getPropertyDescriptor(propertyName);
    if (!descriptor) {
        return;
    }
    
    // Skip if the property has the NoUndo flag
    if (descriptor->hasFlag(PropertyDescriptor::Flags::NoUndo)) {
        return;
    }
    
    // Create action
    auto action = std::make_shared<PropertyChangeAction>(
        container, propertyName, oldValue, newValue, description
    );
    
    // If we're in a batch operation, add to the batch
    if (m_currentBatch) {
        m_currentBatch->addPropertyChange(container, propertyName, oldValue, newValue);
    }
    else {
        addAction(action);
    }
}

void UndoManager::beginBatchOperation(const std::string& description) {
    // If a batch is already in progress, end it first
    if (m_currentBatch) {
        endBatchOperation();
    }
    
    m_currentBatch = std::make_shared<MultiPropertyChangeAction>(description);
}

void UndoManager::endBatchOperation() {
    if (!m_currentBatch) {
        return;
    }
    
    // Only add the batch if it contains changes
    if (!m_currentBatch->isEmpty()) {
        addAction(m_currentBatch);
    }
    
    m_currentBatch = nullptr;
}

void UndoManager::cancelBatchOperation() {
    m_currentBatch = nullptr;
}

void UndoManager::undo() {
    if (m_undoStack.empty()) {
        return;
    }
    
    // End any current batch operation
    if (m_currentBatch) {
        endBatchOperation();
    }
    
    // Get the action to undo
    auto action = m_undoStack.back();
    m_undoStack.pop_back();
    
    // Perform the undo
    action->undo();
    
    // Add to redo stack
    m_redoStack.push_back(action);
    
    // Cap redo stack size
    while (m_redoStack.size() > m_maxUndoLevels) {
        m_redoStack.pop_front();
    }
}

void UndoManager::redo() {
    if (m_redoStack.empty()) {
        return;
    }
    
    // End any current batch operation
    if (m_currentBatch) {
        endBatchOperation();
    }
    
    // Get the action to redo
    auto action = m_redoStack.back();
    m_redoStack.pop_back();
    
    // Perform the redo
    action->redo();
    
    // Add to undo stack
    m_undoStack.push_back(action);
    
    // Cap undo stack size
    while (m_undoStack.size() > m_maxUndoLevels) {
        m_undoStack.pop_front();
    }
}

void UndoManager::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    m_currentBatch = nullptr;
}

std::string UndoManager::getUndoDescription() const {
    if (m_undoStack.empty()) {
        return "";
    }
    return m_undoStack.back()->getDescription();
}

std::string UndoManager::getRedoDescription() const {
    if (m_redoStack.empty()) {
        return "";
    }
    return m_redoStack.back()->getDescription();
}

std::vector<std::string> UndoManager::getUndoStack() const {
    std::vector<std::string> descriptions;
    descriptions.reserve(m_undoStack.size());
    
    for (const auto& action : m_undoStack) {
        descriptions.push_back(action->getDescription());
    }
    
    return descriptions;
}

std::vector<std::string> UndoManager::getRedoStack() const {
    std::vector<std::string> descriptions;
    descriptions.reserve(m_redoStack.size());
    
    for (const auto& action : m_redoStack) {
        descriptions.push_back(action->getDescription());
    }
    
    return descriptions;
}

void UndoManager::addAction(std::shared_ptr<UndoAction> action) {
    // Add to undo stack
    m_undoStack.push_back(action);
    
    // Clear redo stack
    m_redoStack.clear();
    
    // Cap undo stack size
    while (m_undoStack.size() > m_maxUndoLevels) {
        m_undoStack.pop_front();
    }
}