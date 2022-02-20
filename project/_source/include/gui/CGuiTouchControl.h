// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  class CGlobal;  /**< Foward declaration of the global class */

  namespace gui {
    const int g_TouchControlId = MAKE_IRR_ID('t', 'o', 'c', 'h');
    const irr::c8 g_TouchControlName[] = "TouchControl";

    /**
    * @class CGuiTouchControl
    * @author Christian Keimel
    * This class receives touch controls and turns them into marble controls
    */
    class CGuiTouchControl : public irr::gui::IGUIElement {
      public:
        enum class enType {
          ControlLeft,
          ControlRight
        };

      private:
        enType   m_eType;
        CGlobal *m_pGlobal;
        int      m_iTouchIdTh,    /**< Throttle touch ID (in digital control this is for both throttle and steer) */
                 m_iTouchIdBk,    /**< Brake touch ID (only used in the digital control versions) */
                 m_iTouchIdRs,    /**< Respawn control */
                 m_iTouchIdRv;    /**< Rearview control */

        irr::s8 m_iCtrlX,       /**< X (Steer) control */
                m_iCtrlY,       /**< Y (Throttle) control */
                m_aCtrl[9][2];  /**< The steer / throttle controls for the nine digital fields */

        bool m_bBrake,
             m_bRespawn,
             m_bRearView;

        irr::video::ITexture *m_aControl[9],  /**< The icons for the 9 control rects */
                             *m_pRespawn,     /**< The respawn control icon */
                             *m_pRearView,    /**< An icon for the rearview control */
                             *m_pBrake;       /**< Brake icon */

        irr::video::IVideoDriver *m_pDrv; /**< The video driver used for drawing */

        irr::core::recti m_cDigital[9],   /**< Nine fields for digial control options */
                         m_cTotal,        /**< The total control rect for digital */
                         m_cThrottle,     /**< Throttle control (not used in the digital versions) */
                         m_cSteer,        /**< Steer control (not used in the digital versions) */
                         m_cBrake,        /**< Brake control (only used in the digital versions) */
                         m_cRespawn,      /**< Respawn control */
                         m_cRearView;     /**< Rearview control */

        irr::core::position2di m_cTouchPos[4];  /**< The touch positions (0 == marble control, 1 == brake, 2 == respawn, 3 == rearview) */

        void initialize(const irr::core::recti &a_cRect);

      public:
        CGuiTouchControl(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTouchControl();

        virtual void draw();

        virtual bool OnEvent(const irr::SEvent& a_cEvent);

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);

        void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView);
    };
  }
}

