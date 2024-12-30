// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>
#include <irrlicht.h>
#include <vector>
#include <tuple>
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
          Thumbnail      = 7,
          TrackName      = 8,
          Count
        };

        CCommandHandler *m_pHandler;    /**< The command handler */
        CCmdEditTrack   *m_pEditTrack;  /**< The edit track command */
        
        irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht GUI environment */
        irr::ITimer               *m_pTimer;  /**< The Irrlicht timer */

        std::map<enMenuItem, irr::gui::IGUIButton *> m_mButtons;  /**< The buttons of the menu */

        irr::gui::IGUIStaticText *m_pEditMode;   /**< The "Edit Mode" or information label in the upper left */

        irr::core::position2di m_cLogPos;   /**< The start position for the log labels */

        /* The log messages (0 == message, 1 == Log Level, 2 == Time to live, 3 == Start time) */
        std::vector<std::tuple<std::wstring, irr::gui::IGUIStaticText *, irr::ELOG_LEVEL, irr::u32, irr::u32>> m_vLog;

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
        * Add a log message
        * @param a_sMessage the message
        * @param a_eLevel the log level
        * @param a_iTtl the time to live for the message
        */
        void addLogMessage(const std::wstring &a_sMessage, irr::ELOG_LEVEL a_eLevel, irr::u32 a_iTtl);

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
        * This method is called on every frame.
        */
        virtual void run() override;

        /**
        * Callback method for changes in the UI like modifying
        * font size or resizing window
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