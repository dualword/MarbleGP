// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CHudAiHelp
    * @author Christian Keimel
    * This class encapsulates the display items for AI help shown in the HUD
    */
    class CHudAiHelp {
      public:
        /**
        * Enum of all the icons this class renders
        */
        enum class enIcons {
          Left      = 0,
          Forward   = 1,
          Brake     = 2,
          Automatic = 3,
          Respawn   = 4,
          Backward  = 5,
          Right     = 6,
          Count     = 7
        };

        /**
        * State of the icons
        */
        enum class enState {
          Active   = 0,
          Inactive = 1
        };

      private:
        irr::video::ITexture     *m_aIcons[7][2];   /**< All the icons in on [..][0] and off [..][1] version */
        irr::core::recti          m_aRects[7];      /**< The screen rects of all the icons */
        irr::core::recti          m_cSource;        /**< The source rect */
        irr::core::vector2di      m_cOffset;        /**< The render offset */
        enState                   m_aState[7];      /**< Define the icon state (0 == on, 1 == off) for all the icons */
        irr::video::IVideoDriver *m_pDrv;           /**< The Irrlicht video driver */

      public:
        /**
        * The constructor
        * @param a_pDrv the video driver
        * @param a_cRect the rect to render to
        * @param a_eAiHelp the AI help level for the player
        */
        CHudAiHelp(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cRect, data::SPlayerData::enAiHelp a_eAiHelp);
        ~CHudAiHelp();

        /**
        * Change the state of an icon
        * @param a_eIcons the icon to change
        * @param a_eState the new state of the icon
        */
        void setIconState(enIcons a_eIcon, enState a_eState);

        /**
        * Render the AI help
        * @param a_cPosition the position (lower center) to render to
        * @param a_cClip the clipping rectangle
        */
        void render(const irr::core::position2di a_cPosition, const irr::core::recti &a_cClip);
    };
  }
}

