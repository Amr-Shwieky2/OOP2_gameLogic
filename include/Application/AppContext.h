﻿
/**
 * @brief Central application context using Singleton pattern for service management
 *
 * AppContext provides a single access point to all application services and manages
 * their lifecycle. It ensures only one instance of each service exists and handles
 * automatic memory management using smart pointers.
 *
 * Services managed:
 * - TextureLoader: Image and texture management
 * - FontLoader: Font resource management
 * - SoundLoader: Sound effect management
 * - ScreenManager: Screen and UI management
 * - CommandInvoker: Command pattern execution
 *
 * Usage: AppContext::instance().serviceName().method()
 */
#pragma once
#include <memory>
#include "ScreenManager.h"
#include <CommandInvoker.h>
#include <AudioSettingsManager.h>
#include <ResourceManager.h>

class AppContext {
public:
    static AppContext& instance();

    // Template-based resource loaders
    TextureManager& textures();
    FontManager& fonts();
    SoundManager& sounds();

    // Other services
    ScreenManager& screenManager();
    CommandInvoker& commandInvoker();

    // Backward compatibility methods (optional - for easy migration)
    sf::Texture& getTexture(const std::string& filename) {
        return textures().getResource(filename);
    }

    sf::Font& getFont(const std::string& filename) {
        return fonts().getResource(filename);
    }

    sf::SoundBuffer& getSound(const std::string& filename) {
        return sounds().getResource(filename);
    }

    AudioSettings& audioSettings() { return m_audioSettings; }

private:
    AppContext();
    AppContext(const AppContext&) = delete;
    AppContext& operator=(const AppContext&) = delete;
    AudioSettings m_audioSettings;

    // Template-based resource loaders
    std::unique_ptr<TextureManager> m_textureManager;
    std::unique_ptr<FontManager> m_fontManager;
    std::unique_ptr<SoundManager> m_soundManager;

    // Other services
    std::unique_ptr<ScreenManager> m_screenManager;
    std::unique_ptr<CommandInvoker> m_commandInvoker;
};