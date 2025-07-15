#pragma once
#include <string>
#include "AppContext.h"
#include "../Config/ScreenTypes.h"
#include "../Core/AudioManager.h"
#include "../Core/AudioSettingsManager.h"
#include "Logger.h"

// Screen headers
#include "../Screens/LoadingScreen.h"
#include "../Screens/MenuScreen.h"
#include "../Screens/SettingsScreen.h"
#include "../Screens/AboutScreen.h"
#include "../Screens/HelpScreen.h"

/**
 * @brief Responsible for initializing all core systems before the game starts.
 *
 * This class follows the Single Responsibility Principle (SRP) by focusing solely
 * on setting up essential systems like audio, resource managers, and screen registration.
 * It ensures that everything is ready before entering the main game loop.
 */
class GameInitializer {
public:
    /**
     * @brief Default constructor.
     */
    GameInitializer() = default;

    /**
     * @brief Default destructor.
     */
    ~GameInitializer() = default;

    /**
     * @brief Initializes all core game systems in order: audio, resources, and screens.
     *
     * This is the main entry point to prepare the game environment.
     * Throws on critical failure.
     */
    void initializeAllSystems();

private:
    /**
     * @brief Initializes the audio system and loads saved or default settings.
     */
    void initializeAudioSystem();

    /**
     * @brief Initializes core resource managers (textures, fonts, sounds).
     */
    void initializeResourceSystem();

    /**
     * @brief Registers all available game screens in the screen manager.
     */
    void registerAllScreens();

    /**
     * @brief Loads all default audio assets (music and sound effects).
     */
    void loadDefaultAudioFiles();

    /**
     * @brief Preloads all textures used throughout the game.
     */
    void loadDefaultTextures();

    /**
     * @brief Preloads fonts required by the UI system.
     */
    void loadDefaultFonts();

    /**
     * @brief Saves current audio volume levels as defaults.
     */
    void setDefaultAudioVolumes();

    /**
     * @brief Registers all screen factories to enable runtime screen switching.
     */
    void registerScreenFactories();

    /**
     * @brief Handles initialization errors and logs them appropriately.
     *
     * @param system The name of the system that failed.
     * @param error The error message.
     */
    void handleInitializationError(const std::string& system, const std::string& error);
};
