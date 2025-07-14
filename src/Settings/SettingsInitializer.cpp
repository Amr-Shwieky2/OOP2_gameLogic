#include "../../include/Settings/SettingsInitializer.h"
#include <Application/AppContext.h>
#include "GameExceptions.h"

SettingsInitializer::InitResult SettingsInitializer::initialize() {
    InitResult result;

    try {
        // Resource manager
        result.resourceManager = std::make_unique<SettingsResourceManager>();
        if (!result.resourceManager->initializeResources()) {
            THROW_INITIALIZATION_EXCEPTION("Failed to initialize SettingsResourceManager");
        }

        // UI Renderer
        result.uiRenderer = std::make_unique<SettingsUIRenderer>(result.resourceManager->getFont());
        result.uiRenderer->initializeTexts();
        result.uiRenderer->enableGlowEffect(true);
        result.uiRenderer->enableShadowEffect(true);
        result.uiRenderer->setAnimationSpeed(1.0f);

        // Command handler
        result.commandHandler = std::make_unique<SettingsCommandHandler>();
        result.commandHandler->enableAutoSave(true);
        result.commandHandler->setAutoSaveDelay(0.5f);

        // Volume panel
        result.volumePanel = std::make_shared<VolumeControlPanel>(
            result.resourceManager->getFont(),
            AudioManager::instance(),
            AppContext::instance().audioSettings()
        );
        result.commandHandler->setVolumePanel(result.volumePanel);

        result.success = true;
    }
    catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }

    return result;
}
