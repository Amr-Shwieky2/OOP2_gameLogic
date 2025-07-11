#pragma once
#include "PropertyValue.h"
#include <vector>
#include <memory>
#include <string>
#include <deque>
#include <functional>

class PropertyContainer;

/**
 * UndoAction - Base class for undoable actions
 */
class UndoAction {
public:
    virtual ~UndoAction() = default;
    
    virtual void undo() = 0;
    virtual void redo() = 0;
    
    virtual std::string getDescription() const = 0;
};

/**
 * PropertyChangeAction - Undoable action for property changes
 */
class PropertyChangeAction : public UndoAction {
public:
    PropertyChangeAction(
        PropertyContainer* container,
        const std::string& propertyName,
        const PropertyValue& oldValue,
        const PropertyValue& newValue,
        const std::string& description
    );
    
    void undo() override;
    void redo() override;
    
    std::string getDescription() const override { return m_description; }
    
private:
    PropertyContainer* m_container;
    std::string m_propertyName;
    PropertyValue m_oldValue;
    PropertyValue m_newValue;
    std::string m_description;
};

/**
 * MultiPropertyChangeAction - Undoable action for multiple property changes
 */
class MultiPropertyChangeAction : public UndoAction {
public:
    MultiPropertyChangeAction(const std::string& description);
    
    void addPropertyChange(
        PropertyContainer* container,
        const std::string& propertyName,
        const PropertyValue& oldValue,
        const PropertyValue& newValue
    );
    
    void undo() override;
    void redo() override;
    
    std::string getDescription() const override { return m_description; }
    
    bool isEmpty() const { return m_changes.empty(); }
    
private:
    struct PropertyChange {
        PropertyContainer* container;
        std::string propertyName;
        PropertyValue oldValue;
        PropertyValue newValue;
    };
    
    std::vector<PropertyChange> m_changes;
    std::string m_description;
};

/**
 * UndoManager - Manages undo/redo operations for property changes
 */
class UndoManager {
public:
    // Get the singleton instance
    static UndoManager& getInstance();
    
    // Record property change for undo/redo
    void recordPropertyChange(
        PropertyContainer* container,
        const std::string& propertyName,
        const PropertyValue& oldValue,
        const PropertyValue& newValue,
        const std::string& description = "Property Changed"
    );
    
    // Begin/end batch operations
    void beginBatchOperation(const std::string& description = "Multiple Changes");
    void endBatchOperation();
    void cancelBatchOperation();
    
    // Undo/redo operations
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }
    
    void undo();
    void redo();
    
    // Clear undo/redo history
    void clear();
    
    // Get descriptions for undo/redo actions
    std::string getUndoDescription() const;
    std::string getRedoDescription() const;
    
    // Get the current state of the undo/redo stacks
    std::vector<std::string> getUndoStack() const;
    std::vector<std::string> getRedoStack() const;
    
    // Set maximum stack size
    void setMaxUndoLevels(size_t levels) { m_maxUndoLevels = levels; }
    size_t getMaxUndoLevels() const { return m_maxUndoLevels; }
    
private:
    UndoManager();
    ~UndoManager() = default;
    
    // Undo/redo stacks
    std::deque<std::shared_ptr<UndoAction>> m_undoStack;
    std::deque<std::shared_ptr<UndoAction>> m_redoStack;
    
    // Current batch operation (if any)
    std::shared_ptr<MultiPropertyChangeAction> m_currentBatch;
    
    // Maximum number of actions to store
    size_t m_maxUndoLevels = 100;
    
    // Add an action to the undo stack
    void addAction(std::shared_ptr<UndoAction> action);
};