#pragma once
#include <string>
#include "AppContext.h"
#include "../Config/ScreenTypes.h"
#include "../Core/AudioManager.h"
#include "../Core/AudioSettingsManager.h"
#include "Logger.h"

// Screens
#include "../Screens/LoadingScreen.h"
#include "../Screens/MenuScreen.h"
#include "../Screens/SettingsScreen.h"
#include "../Screens/AboutScreen.h"
#include "../Screens/HelpScreen.h"

class GameInitializer {
public:
    GameInitializer() = default;
    ~GameInitializer() = default;

    void initializeAllSystems();

private:
    void initializeAudioSystem();
    void initializeResourceSystem();
    void registerAllScreens();

    void loadDefaultAudioFiles();
    void setDefaultAudioVolumes();
    void registerScreenFactories();

    void handleInitializationError(const std::string& system, const std::string& error);
};
