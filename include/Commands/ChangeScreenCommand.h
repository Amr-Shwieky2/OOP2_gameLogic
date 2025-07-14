#pragma once
#include "../Core/ICommand.h"
#include "../Config/ScreenTypes.h"
#include <string>

// Command for changing screen with ability to go back
class ChangeScreenCommand : public ICommand {
public:
    // Constructor
    ChangeScreenCommand(ScreenType targetScreen, ScreenType previousScreen);

    // ICommand interface
    void execute() override;
    void undo() override;
    bool canUndo() const override;
    std::string getName() const override;

private:
    ScreenType m_targetScreen;      // Target screen to navigate to
    ScreenType m_previousScreen;    // Previous screen (for going back)

    // Helper function to convert ScreenType to string
    std::string screenTypeToString(ScreenType screen) const;
};