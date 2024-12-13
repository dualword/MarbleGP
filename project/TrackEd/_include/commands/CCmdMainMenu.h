// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>
#include <irrlicht.h>
#include <map>

namespace dustbin {
  namespace commands {

    class CCommandHandler;  /**< Forward declaration of the command handler */

    /**
    * @class CCmdMainMenu
    * @author Christian Kiemel
    * This is the main menu of the editor
    */
    class CCmdMainMenu : public ICommand {
      private:
        enum class enMenuItem {
          NewTrack,
          Load,
          Save,
          SaveAs
        };

        CCommandHandler *m_pHandler;    /**< The command handler */

        std::map<enMenuItem, irr::gui::IGUIButton *> m_mButtons;  /**< The buttons of the menu */

      public:
        CCmdMainMenu(CCommandHandler *a_pHandler, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CCmdMainMenu();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * Callback method for changes in the UI like modifying
        * font size or resizin window
        */
        virtual void onUiSetupModified() override;

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}