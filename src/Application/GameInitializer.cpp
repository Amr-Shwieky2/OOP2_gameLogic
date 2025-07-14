#include <Application/GameInitializer.h>
#include <Services/Logger.h>
#include <Screens/GameplayScreen.h>

void GameInitializer::initializeAllSystems() {
    Logger::log("Starting game systems initialization...");

    try {
        initializeAudioSystem();
        initializeResourceSystem();
        registerAllScreens();

        Logger::log("All game systems initialized successfully");
    }
    catch (const std::exception& e) {
        Logger::log("Critical initialization failure: " + std::string(e.what()), LogLevel::Error);
        throw;
    }
}

void GameInitializer::initializeAudioSystem() {
    try {
        Logger::log("Initializing audio system...");

        AudioSettings settings;
        if (AudioSettingsManager::load(settings)) {
            AudioManager::instance().setMasterVolume(settings.masterVolume);
            AudioManager::instance().setMusicVolume(settings.musicVolume);
            AudioManager::instance().setSFXVolume(settings.sfxVolume);

            Logger::log("Audio settings loaded successfully");
        }
        else {
            Logger::log("Using default audio settings", LogLevel::Warning);
        }

        loadDefaultAudioFiles();
        setDefaultAudioVolumes();

        Logger::log("Audio system initialized successfully");
    }
    catch (const std::exception& e) {
        handleInitializationError("Audio System", e.what());
        Logger::log("Game will continue without audio", LogLevel::Warning);
    }
}

void GameInitializer::loadDefaultAudioFiles() {
    auto& audioManager = AudioManager::instance();

    if (audioManager.loadMusic("loading_music", "intro.wav")) {
        Logger::log("Loading music loaded successfully");
    }
    else {
        Logger::log("Warning: Could not load  music", LogLevel::Warning);
    }
}

void GameInitializer::setDefaultAudioVolumes() {
    AudioSettings settings;
    settings.masterVolume = AudioManager::instance().getMasterVolume();
    settings.musicVolume = AudioManager::instance().getMusicVolume();
    settings.sfxVolume = AudioManager::instance().getSFXVolume();

    AudioSettingsManager::save(settings);
    Logger::log("Set default audio volumes");
}

void GameInitializer::initializeResourceSystem() {
    try {
        Logger::log("Initializing resource system...");

        auto& textures = AppContext::instance().textures();
        auto& fonts = AppContext::instance().fonts();
        auto& sounds = AppContext::instance().sounds();

        Logger::log("Resource system initialized successfully");
    }
    catch (const std::exception& e) {
        handleInitializationError("Resource System", e.what());
        throw;
    }
}

void GameInitializer::registerAllScreens() {
    try {
        Logger::log("Registering all screens...");
        registerScreenFactories();
        AppContext::instance().screenManager().changeScreen(ScreenType::LOADING);
        Logger::log("All screens registered successfully");
    }
    catch (const std::exception& e) {
        handleInitializationError("Screen System", e.what());
        throw;
    }
}

void GameInitializer::registerScreenFactories() {
    auto& screenManager = AppContext::instance().screenManager();

    screenManager.registerScreen(ScreenType::LOADING, []() {
        return std::make_unique<LoadingScreen>();
        });

    screenManager.registerScreen(ScreenType::MENU, []() {
        return std::make_unique<MenuScreen>();
        });

    screenManager.registerScreen(ScreenType::SETTINGS, []() {
        return std::make_unique<SettingsScreen>();
        });

    screenManager.registerScreen(ScreenType::HELP, []() {
        return std::make_unique<HelpScreen>();
        });

    screenManager.registerScreen(ScreenType::ABOUT_US, []() {
        return std::make_unique<AboutScreen>();
        });

    screenManager.registerScreen(ScreenType::PLAY, []() {
        return std::make_unique<GameplayScreen>();
        });

}

void GameInitializer::handleInitializationError(const std::string& system, const std::string& error) {
    Logger::log("Error initializing " + system + ": " + error, LogLevel::Error);
}
