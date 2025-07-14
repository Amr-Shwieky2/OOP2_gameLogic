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
    // Create unique instances of template-based resource loaders
    // Order of initialization may be important for service dependencies
    m_textureLoader = std::make_unique<TextureLoader>();
    m_fontLoader = std::make_unique<FontLoader>();
    m_soundLoader = std::make_unique<SoundLoader>();

    // Initialize other services
    m_screenManager = std::make_unique<ScreenManager>();
    m_commandInvoker = std::make_unique<CommandInvoker>();
}

// Service accessor implementations - return dereferenced smart pointers
// These methods provide direct access to service instances

TextureLoader& AppContext::textures() {
    // Dereference to provide access to texture/image management
    return *m_textureLoader;
}

FontLoader& AppContext::fonts() {
    // Dereference to provide access to font resource management
    return *m_fontLoader;
}

SoundLoader& AppContext::sounds() {
    // Dereference to provide access to sound effect management
    return *m_soundLoader;
}

ScreenManager& AppContext::screenManager() {
    return *m_screenManager;
}

CommandInvoker& AppContext::commandInvoker() {
    return *m_commandInvoker;
}