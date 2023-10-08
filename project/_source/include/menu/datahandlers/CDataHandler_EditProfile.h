#pragma once

#include <menu/datahandlers/IMenuDataHandler.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
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
        std::vector<data::SPlayerData                   >::iterator  m_itEditProfile;   /**< Iterator pointing to the edited profile, might be end() if we create a new profile */
        std::vector<std::tuple<std::string, std::string>>            m_vDefaultNames;   /**< Vector with the default names (0 == first name, 1 == surname) */
        data::SPlayerData                                            m_cEditProfile;    /**< The edited profile */
        irr::gui::IGUIEnvironment                                   *m_pGui;            /**< The GUI environment */

        enEditProfileStep m_eStep;    /**< The current edit step */
        
        irr::gui::IGUIElement *m_pAiHelp;   /**< The currently visible AI help */

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
        CDataHandler_EditProfile(std::vector<data::SPlayerData>::iterator a_itEditProfile, const data::SPlayerData &a_cEditProfile);
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
        * Get the iterator of the profile to see if we are editing or adding a new one
        */
        std::vector<data::SPlayerData>::iterator &getProfileIterator();

        /**
        * Set to another step in the dialog
        * @param a_eStep the new step
        * @return true if the new active UI element was found
        */
        bool setEditProfileStep(enEditProfileStep a_eStep);
    };
  }
}
