#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

/**
 * @brief Responsible for handling input in Settings screen only
 * Single Responsibility: Input validation and event handling
 */
class SettingsInputHandler {
public:
    // Callback types for different input events
    using EscapeCallback = std::function<void()>;
    using UndoCallback = std::function<void()>;
    using RedoCallback = std::function<void()>;
    using HistoryCallback = std::function<void()>;

    SettingsInputHandler();
    ~SettingsInputHandler() = default;

    /**
     * @brief Handle keyboard events
     * @param event SFML event
     * @return true if event was handled
     */
    bool handleKeyboardInput(const sf::Event& event);

    /**
     * @brief Register callbacks for different events
     */
    void setEscapeCallback(EscapeCallback callback) { m_escapeCallback = callback; }
    void setUndoCallback(UndoCallback callback) { m_undoCallback = callback; }
    void setRedoCallback(RedoCallback callback) { m_redoCallback = callback; }
    void setHistoryCallback(HistoryCallback callback) { m_historyCallback = callback; }

    /**
     * @brief Enable/disable input handling
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    // Callbacks for different key events
    EscapeCallback m_escapeCallback;
    UndoCallback m_undoCallback;
    RedoCallback m_redoCallback;
    HistoryCallback m_historyCallback;

    bool m_enabled = true;

    /**
     * @brief Individual key handlers
     */
    bool handleEscapeKey();
    bool handleUndoKey();
    bool handleRedoKey();
    bool handleHistoryKey();

    /**
     * @brief Event validation methods
     */
    bool isValidKeyboardEvent(const sf::Event& event) const;
    bool isModifierKeyPressed(const sf::Event& event) const;
    bool isKeyPressed(const sf::Event& event, sf::Keyboard::Key key) const;
};