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
    class CCmdEditTrack;    /**< Forward declaration of the edit track command */

    /**
    * @class CCmdMainMenu
    * @author Christian Kiemel
    * This is the main menu of the editor
    */
    class CCmdMainMenu : public ICommand {
      private:
        enum class enMenuItem {
          NewTrack       = 0,
          Load           = 1,
          Save           = 2,
          SaveAs         = 3,
          Base           = 4,
          ExportMesh     = 5,
          ExportMarbleGP = 6,
          Count
        };

        CCommandHandler *m_pHandler;    /**< The command handler */
        CCmdEditTrack   *m_pEditTrack;  /**< The edit track command */
        
        irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht GUI environment */

        std::map<enMenuItem, irr::gui::IGUIButton *> m_mButtons;  /**< The buttons of the menu */

        irr::gui::IGUIStaticText  *m_pEditMode;   /**< The "Edit Mode" or information label in the upper left */
        irr::gui::IGUIStaticText  *m_pWarning;    /**< An edit field showing some warning */

        const std::wstring getButtonText(enMenuItem a_eButton);

      public:
        CCmdMainMenu(CCommandHandler *a_pHandler, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CCmdMainMenu();

        /**
        * Set the edit mode text
        * @param a_sText the new edit mode text
        */
        void setEditMode(const std::wstring &a_sText);

        /**
        * Set the warning text
        * @param a_sWarning the new warning text
        */
        void setWarningText(const std::wstring &a_sWarning);

        /**
        * Hide the warning label
        */
        void hideWarning();

        /**
        * Set the edit track command
        * @param a_pCmd the edit track command
        */
        void setEditTrackCommand(CCmdEditTrack *a_pCmd);

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
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