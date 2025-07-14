#pragma once  
#include "../Core/ICommand.h"  
#include "../Config/ScreenTypes.h"  
#include <string>  

class EscapeKeyCommand : public ICommand {  
public:  
   // Constructor  
   EscapeKeyCommand(ScreenType currentScreen, ScreenType targetScreen = ScreenType::MENU);  

   // ICommand interface  
   void execute() override;  
   void undo() override;  
   bool canUndo() const override;  
   std::string getName() const override;  

private:  
   ScreenType m_currentScreen;     
   ScreenType m_targetScreen;      
   bool m_hasExecuted = false;     

   // Helper function  
   std::string screenTypeToString(ScreenType screen) const;  
};