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
        std::vector<std::tuple<std::string, std::string>>  m_vDefaultNames;     /**< Vector with the default names (0 == first name, 1 == surname) */
        std::vector<std::string>                           m_vDefaultPatterns;  /**< A List of the available patterns */
        data::SPlayerData                                  m_cEditProfile;      /**< The edited profile */
        irr::gui::IGUIEnvironment                         *m_pGui;              /**< The GUI environment */
        irr::io::IFileSystem                              *m_pFs;               /**< The Irrlicht file system */
        irr::video::IVideoDriver                          *m_pDrv;              /**< The Irrlicht video driver */
        int                                                m_iProfileIndex;     /**< Index of the edited profile (-1 == new profile) */
        gui::CControllerUi_Game                           *m_pCtrl;             /**< The controller UI for joystick and gamepad */
        bool                                               m_bConfigCtrl;       /**< Is the controller being configurated? */
        irr::scene::ISceneManager                         *m_pPreviewSmgr;      /**< The scene manager for the marble preview */
        irr::scene::ISceneNode                            *m_pMarbleNode;       /**< The marble node for the texture preview */
        irr::video::ITexture                              *m_pTextureRtt;       /**< Render target texture for the marble with it's custom texture */

        std::vector<std::tuple<std::string, std::string, std::string, std::string>> m_vDefaultColors;     /**< The default colors for the random textures */

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

        /**
        * Update the texture of the preview marble in the texture wizard step
        * @param a_sTexture the texture string of the marble
        */
        void updateMarbleTexture(const std::string &a_sTexture);

        /**
        * Update the controller UI
        * @param a_iCtrl the controller index (0 == keyboard, 1 == gamepad, 2 == touch, 3 == gyroscope)
        */
        void updateCtrlUi(int a_iCtrl);

        /**
        * Generate a random texture
        * @return a string with random texture parameters
        */
        std::string createRandomTexture();

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
