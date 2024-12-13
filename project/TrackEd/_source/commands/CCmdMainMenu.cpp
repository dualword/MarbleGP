
#include <commands/CCmdMainMenu.h>

namespace dustbin {
  namespace commands {
    CCmdMainMenu::CCmdMainMenu(CCommandHandler* a_pHandler, irr::gui::IGUIEnvironment* a_pGui) :
      ICommand  ("CmdMainMenu"),
      m_pHandler(a_pHandler)
    {

    }

    CCmdMainMenu::~CCmdMainMenu() {

    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * return true if the command is finished and can be removed
    */
    bool CCmdMainMenu::isFinished() {
      return false;
    }

    /**
    * Callback method for changes in the UI like modifying
    * font size or resizin window
    */
    void CCmdMainMenu::onUiSetupModified() {

    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdMainMenu::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

  }
}