#include "../../include/UI/MenuButtonObserver.h"
#include "../../include/Application/AppContext.h"

//-------------------------------------------------------------------------------------
MenuButtonObserver::MenuButtonObserver() {
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::onButtonClicked(const std::string& buttonId) {
    if (buttonId == "start") {
        handleStartButton();
    }
    else if (buttonId == "settings") {
        handleSettingsButton();
    }
    else if (buttonId == "help") {
        handleHelpButton();
    }
    else if (buttonId == "about") {
        handleAboutButton();
    }
    else if (buttonId == "exit") {
        handleExitButton();
    }
    else {
        std::cout << "MenuButtonObserver: Unknown button ID '" << buttonId << "'" << std::endl;
    }
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::onButtonHovered(const std::string& buttonId) {
    std::cout << "MenuButtonObserver: Button '" << buttonId << "' hovered" << std::endl;
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::handleStartButton() {
    std::cout << "MenuButtonObserver: Handling Start Game..." << std::endl;

    ScreenType currentScreen = getCurrentScreen();
    // Show the help screen before starting the actual gameplay
    auto command = std::make_unique<ChangeScreenCommand>(ScreenType::HELP, currentScreen);
    m_commandInvoker.execute(std::move(command));
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::handleSettingsButton() {
    std::cout << "MenuButtonObserver: Handling Settings..." << std::endl;

    ScreenType currentScreen = getCurrentScreen();
    auto command = std::make_unique<ChangeScreenCommand>(ScreenType::SETTINGS, currentScreen);
    m_commandInvoker.execute(std::move(command));
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::handleHelpButton() {
    std::cout << "MenuButtonObserver: Handling Help..." << std::endl;

    ScreenType currentScreen = getCurrentScreen();
    auto command = std::make_unique<ChangeScreenCommand>(ScreenType::HELP, currentScreen);
    m_commandInvoker.execute(std::move(command));
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::handleAboutButton() {
    std::cout << "MenuButtonObserver: Handling About Us..." << std::endl;

    ScreenType currentScreen = getCurrentScreen();
    auto command = std::make_unique<ChangeScreenCommand>(ScreenType::ABOUT_US, currentScreen);
    m_commandInvoker.execute(std::move(command));
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::handleExitButton() {
    std::cout << "MenuButtonObserver: Handling Exit..." << std::endl;

    auto command = std::make_unique<ExitGameCommand>();
    m_commandInvoker.execute(std::move(command));
}
//-------------------------------------------------------------------------------------
bool MenuButtonObserver::undoLastAction() {
    std::cout << "MenuButtonObserver: Attempting to undo last action..." << std::endl;
    return m_commandInvoker.undo();
}
//-------------------------------------------------------------------------------------
bool MenuButtonObserver::redoLastAction() {
    std::cout << "MenuButtonObserver: Attempting to redo last action..." << std::endl;
    return m_commandInvoker.redo();
}
//-------------------------------------------------------------------------------------
void MenuButtonObserver::printCommandHistory() {
    m_commandInvoker.printHistory();
}
//-------------------------------------------------------------------------------------
ScreenType MenuButtonObserver::getCurrentScreen() const {
    return ScreenType::MENU;
}
//-------------------------------------------------------------------------------------