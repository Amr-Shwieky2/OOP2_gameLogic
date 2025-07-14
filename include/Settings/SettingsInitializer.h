#pragma once
#include <memory>
#include "SettingsResourceManager.h"
#include "SettingsUIRenderer.h"
#include "../../include/Commands/SettingsCommandHandler.h"
#include "VolumeControlPanel.h"

class SettingsInitializer {
public:
    struct InitResult {
        std::unique_ptr<SettingsResourceManager> resourceManager;
        std::unique_ptr<SettingsUIRenderer> uiRenderer;
        std::unique_ptr<SettingsCommandHandler> commandHandler;
        std::shared_ptr<VolumeControlPanel> volumePanel;
        bool success;
        std::string errorMessage;
    };

    static InitResult initialize();
};
