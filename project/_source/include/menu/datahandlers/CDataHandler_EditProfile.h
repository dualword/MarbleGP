#pragma once

#include <menu/datahandlers/IMenuDataHandler.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace gui {
    class CControllerUi_Game;   /**< Forward declaration */
  }

  namespace menu {
    /**
    * The steps of the "Add / Edit Profile" wizard
    */
    enum class enEditProfileStep {
      Unknown,
      Name,
      AiHelp,
      Ctrls,
      Texture,
      Overview
    };

    /**
    * @class CDataHandler_EditProfile
    * @author Christian Keimel
    * The data handler for editing a profile
    */
    class CDataHandler_EditProfile : public IMenuDataHandler {
      private:
        std::vector<std::tuple<std::string, std::string>>  m_vDefaultNames;   /**< Vector with the default names (0 == first name, 1 == surname) */
        data::SPlayerData                                  m_cEditProfile;    /**< The edited profile */
        irr::gui::IGUIEnvironment                         *m_pGui;            /**< The GUI environment */
        int                                                m_iProfileIndex;   /**< Index of the edited profile (-1 == new profile) */
        gui::CControllerUi_Game                           *m_pCtrl;           /**< The controller UI for joystick and gamepad */
        bool                                               m_bConfigCtrl;     /**< Is the controller being configurated? */

        enEditProfileStep m_eStep;    /**< The current edit step */
        
        /**
        * Generate a random name and fill the edit field
        */
        void generateDefaultName();

        /**
        * Update the hint for the AI help dialog and the AI help in m_cEditProfile
        * @param a_iAiHelp the new AI help level
        */
        void updateAiHelp(int a_iAiHelp);

      public:
        CDataHandler_EditProfile(int a_iProfileIndex, const data::SPlayerData &a_cEditProfile);
        virtual ~CDataHandler_EditProfile();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get the edited profile
        * @return the edited profile
        */
        const data::SPlayerData &getEditedProfile();

        /**
        * Get the index of the edited profile (-1 == new profile)
        * @return the index of the edited profile
        */
        int getProfileIndex();

        /**
        * Set to another step in the dialog
        * @param a_eStep the new step
        * @return true if the new active UI element was found
        */
        bool setEditProfileStep(enEditProfileStep a_eStep);

        /**
        * Check for controller if we are in the correct state
        */
        virtual void runDataHandler() override;
    };
  }
}
