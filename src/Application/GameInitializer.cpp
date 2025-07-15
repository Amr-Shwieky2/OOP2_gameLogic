#include <Application/GameInitializer.h>
#include <Services/Logger.h>
#include <Screens/GameplayScreen.h>

//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
void GameInitializer::loadDefaultAudioFiles() {
    auto& audio = AudioManager::instance();

    audio.loadMusic("loading_music", "intro.wav");
    audio.loadMusic("gameplay", "play_music.wav");
    audio.loadSound("coin", "coin-received.wav");
    audio.loadSound("jump", "jump.wav");
    audio.loadSound("falcon", "falcon.wav");
    audio.loadSound("gameover", "GameOver.wav");
    audio.loadSound("kill", "kill-enemy.wav");
    audio.loadSound("lostlife", "lost-life.wav");
    audio.loadSound("openbox", "open-box.wav");
    audio.loadSound("win", "win.wav");
    audio.loadSound("well", "well.wav");
    audio.loadSound("gifts", "gifts.wav");
    audio.loadSound("shoot", "shoot.wav");
    audio.loadSound("pushbox", "push_box.wav");
    Logger::log("All default audio files loaded");
}
//-------------------------------------------------------------------------------------
void GameInitializer::setDefaultAudioVolumes() {
    AudioSettings settings;
    settings.masterVolume = AudioManager::instance().getMasterVolume();
    settings.musicVolume = AudioManager::instance().getMusicVolume();
    settings.sfxVolume = AudioManager::instance().getSFXVolume();

    AudioSettingsManager::save(settings);
    Logger::log("Set default audio volumes");
}
//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
void GameInitializer::registerAllScreens() {
    try {
        Logger::log("Registering all screens...");
        registerScreenFactories();
        AppContext::instance().screenManager().requestScreenChange(ScreenType::LOADING);
        Logger::log("All screens registered successfully");
    }
    catch (const std::exception& e) {
        handleInitializationError("Screen System", e.what());
        throw;
    }
}
//-------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------
void GameInitializer::handleInitializationError(const std::string& system, const std::string& error) {
    Logger::log("Error initializing " + system + ": " + error, LogLevel::Error);
}
//-------------------------------------------------------------------------------------