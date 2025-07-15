#pragma once
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

/**
 * Base class for all game events
 */
class Event {
public:
    virtual ~Event() = default;
    virtual const char* getName() const = 0;
};

/**
 * Event listener interface
 */
class IEventListener {
public:
    virtual ~IEventListener() = default;
};

/**
 * Typed event listener
 */
template<typename TEvent>
class EventListener : public IEventListener {
public:
    using Handler = std::function<void(const TEvent&)>;

    EventListener(Handler handler) : m_handler(handler) {}

    void handle(const Event& event) {
        m_handler(static_cast<const TEvent&>(event));
    }

private:
    Handler m_handler;
};

/**
 * Event System - Central event dispatcher
 * Implements Observer Pattern from course material
 */
class EventSystem {
public:
    static EventSystem& getInstance();

    // Subscribe to events
    template<typename TEvent>
    void subscribe(std::function<void(const TEvent&)> handler);

    // Publish events
    template<typename TEvent>
    void publish(const TEvent& event);

    // Clear all listeners
    void clear();

private:
    EventSystem() = default;

    using ListenerList = std::vector<std::unique_ptr<IEventListener>>;
    std::unordered_map<std::type_index, ListenerList> m_listeners;
};

// Template implementations
template<typename TEvent>
void EventSystem::subscribe(std::function<void(const TEvent&)> handler) {
    static_assert(std::is_base_of<Event, TEvent>::value, "TEvent must derive from Event");

    auto listener = std::make_unique<EventListener<TEvent>>(handler);
    m_listeners[typeid(TEvent)].push_back(std::move(listener));
}

template<typename TEvent>
void EventSystem::publish(const TEvent& event) {
    static_assert(std::is_base_of<Event, TEvent>::value, "TEvent must derive from Event");

    auto it = m_listeners.find(typeid(TEvent));
    if (it != m_listeners.end()) {
        for (auto& listener : it->second) {
            auto* typedListener = static_cast<EventListener<TEvent>*>(listener.get());
            typedListener->handle(event);
        }
    }
}