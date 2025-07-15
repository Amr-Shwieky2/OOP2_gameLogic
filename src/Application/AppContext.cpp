#include <Application/AppContext.h>

// Singleton instance getter - creates static instance on first call
AppContext& AppContext::instance() {
    // Static local variable ensures single instance creation
    // Thread-safe in C++11 and later standards
    static AppContext context;
    return context;
}

// Private constructor - initializes all services using smart pointers
AppContext::AppContext() {
    // Initialize other services
    m_screenManager = std::make_unique<ScreenManager>();
    m_commandInvoker = std::make_unique<CommandInvoker>();
}

// Service accessor implementations - return dereferenced smart pointers
// These methods provide direct access to service instances

TextureManager& AppContext::textures() {
    if (!m_textureManager)
        m_textureManager = std::make_unique<TextureManager>();
    return *m_textureManager;
}

FontManager& AppContext::fonts() {
    if (!m_fontManager)
        m_fontManager = std::make_unique<FontManager>();
    return *m_fontManager;
}

SoundManager& AppContext::sounds() {
    if (!m_soundManager)
        m_soundManager = std::make_unique<SoundManager>();
    return *m_soundManager;
}


ScreenManager& AppContext::screenManager() {
    return *m_screenManager;
}

CommandInvoker& AppContext::commandInvoker() {
    return *m_commandInvoker;
}