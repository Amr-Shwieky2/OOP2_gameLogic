#pragma once

#include "IButtonObserver.h"
#include "../Core/CommandInvoker.h"
#include "../Commands/ChangeScreenCommand.h"
#include "../Commands/ExitGameCommand.h"
#include "../Config/ScreenTypes.h"
#include <iostream>
#include <memory>

class MenuButtonObserver : public IButtonObserver {
public:
    MenuButtonObserver();
    ~MenuButtonObserver() = default;

    // IButtonObserver interface
    void onButtonClicked(const std::string& buttonId) override;
    void onButtonHovered(const std::string& buttonId) override;

    CommandInvoker& getCommandInvoker() { return m_commandInvoker; }

    bool undoLastAction();
    bool redoLastAction();
    void printCommandHistory();

private:
    CommandInvoker m_commandInvoker; 

    // Command Pattern handlers
    void handleStartButton();
    void handleSettingsButton();
    void handleHelpButton();
    void handleAboutButton();
    void handleExitButton();

    ScreenType getCurrentScreen() const;
};