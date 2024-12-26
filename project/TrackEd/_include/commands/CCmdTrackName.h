// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    class CCmdEditTrack;    /**< Forward delcaration of the "edit track" command */

    /**
    * @class CCmdTrackName
    * @author Christian Keimel
    * This command let's you edit the track's name
    */
    class CCmdTrackName : public ICommand {
      private:
        irr::gui::IGUIEnvironment *m_pGui;        /**< The GUI environment */
        irr::gui::IGUITab         *m_pParent;     /**< The parent element */
        irr::gui::IGUIComboBox    *m_pCategory;   /**< The "Category" dropdown */
        irr::gui::IGUIEditBox     *m_pTrackName;  /**< The "Track Name" edit field */
        irr::gui::IGUIButton      *m_pBtnOk;      /**< The "OK" button */
        irr::gui::IGUIButton      *m_pBtnCancel;  /**< The "Cancel" button */

        CCmdEditTrack *m_pEditCmd;    /**< The active "edit track" command */

        bool m_bFinished;   /**< Is the command finished? */

        /**
        * Clear the UI elements of this command
        */
        void clearUIElements();

      public:
        /**
        * The constructor
        * @param a_pGui the Irrlicht GUI environment
        */
        CCmdTrackName(irr::gui::IGUIEnvironment *a_pGui, CCmdEditTrack *a_pEditCmd);

        virtual ~CCmdTrackName();

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
