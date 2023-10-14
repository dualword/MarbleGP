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
      Data,
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
        /**
        * The direction for step changes
        */
        enum class enDirection {
          Previous,
          NoChange,
          Next
        };

        std::vector<std::tuple<std::string, std::string>>  m_vDefaultNames;     /**< Vector with the default names (0 == first name, 1 == surname) */
        std::vector<std::string>                           m_vDefaultPatterns;  /**< A List of the available patterns */
        data::SPlayerData                                  m_cEditProfile;      /**< The edited profile */
        int                                                m_iProfileIndex;     /**< Index of the edited profile (-1 == new profile) */
        gui::CControllerUi_Game                           *m_pCtrl;             /**< The controller UI for joystick and gamepad */
        bool                                               m_bConfigCtrl;       /**< Is the controller being configurated? */
        bool                                               m_bConfirmDelete;    /**< Are we waiting for a confirmation of a delete profile button click? */
        bool                                               m_bDeleteProfile;    /**< Shall the profile be deleted? */
        irr::scene::ISceneManager                         *m_pPreviewSmgr;      /**< The scene manager for the marble preview */
        irr::scene::ISceneNode                            *m_pMarbleNode;       /**< The marble node for the texture preview */
        irr::video::ITexture                              *m_pTextureRtt;       /**< Render target texture for the marble with it's custom texture */
        std::string                                        m_sEditColor;        /**< The currently edited color parameter */

        std::vector<std::tuple<std::string, std::string, std::string, std::string>> m_vDefaultColors;     /**< The default colors for the random textures */

        std::vector<enEditProfileStep> m_vSteps;    /**< The steps of the editor */

        std::vector<enEditProfileStep>::iterator m_itStep;    /**< The current editor step */
        
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

        /**
        * Update the pattern images starting with "m_iPatternPage"
        * @see m_iPatternPage
        */
        void updatePatterns();

        /**
        * Initialize the color dialog
        * @param a_cColor the initial color
        */
        void initializeColorDialog(const std::string &a_sColor);

        /**
        * Update the color dialog
        */
        void updateColorDialog();

        /**
        * Modify a texture parameter
        * @param a_sKey the parameter key
        * @param a_sValue the new value
        */
        void modifyTextureParameter(const std::string &a_sKey, const std::string &a_sValue);

        /**
        * Set the visibility of a GUI element
        * @param a_sName name of the GUI element
        * @param a_bVisible new visibility flag
        * @return true if the element was found
        */
        bool setElementVisibility(const std::string &a_sName, bool a_bVisible);

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
        * @param a_eDirection the direction for the next step
        * @return true if the new active UI element was found
        */
        bool setEditProfileStep(enDirection a_eDirection);

        /**
        * Check for controller if we are in the correct state
        */
        virtual void runDataHandler() override;

        /**
        * Shall the profile be deleted?
        */
        bool deleteProfile();
    };
  }
}
