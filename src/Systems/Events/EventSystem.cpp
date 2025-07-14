// EventSystem.cpp
#include "EventSystem.h"

EventSystem& EventSystem::getInstance() {
    static EventSystem* instance = new EventSystem();
    return *instance;
}

void EventSystem::clear() {
    m_listeners.clear();
}