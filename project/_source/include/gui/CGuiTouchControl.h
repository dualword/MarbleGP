// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Foward declaration of the global class */

  namespace gui {
    const int g_TouchControlId = MAKE_IRR_ID('t', 'o', 'c', 'h');
    const irr::c8 g_TouchControlName[] = "TouchControl";

    enum class enTouchCtrlType {
      SteerLeft    = 0,
      SteerRIght   = 1,
      ControlLeft  = 2,
      ControlRight = 3
    };

    /**
    * @class IGuiTouchControl
    * @author Christian Keime
    * The interface for the two implementations of the touch marble controller
    */
    class IGuiTouchControl : public irr::gui::IGUIElement {
      protected:
        enTouchCtrlType  m_eType;
        CGlobal         *m_pGlobal;

        irr::video::IVideoDriver *m_pDrv;

      public:
        IGuiTouchControl(irr::gui::IGUIElement *a_pParent);
        virtual ~IGuiTouchControl();

        virtual void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) = 0;
    };

    /**
    * @class CGuiTouchControl_Split
    * @authro Christian Keimel
    * This class implements the touch control interface with the
    * power controls on one side and the steering controls on the other
    */
    class CGuiTouchControl_Split : public IGuiTouchControl {
      private:
        /**
        * This enumeration holds the indices for the textures of all control items
        */
        enum enItemIndex {
          ItemForeward = 0,
          ItemBackward = 1,
          ItemLeft     = 2,
          ItemRight    = 3,
          ItemNeutralP = 4,
          ItemNeutralS = 5,
          ItemBrake    = 6,
          ItemRespawn  = 7,
          ItemRearview = 8,
          ItemCount    = 9
        };

        /**
        * This enumeration holds the indices for the touch IDs
        */
        enum enTouchId {
          IdPower    = 0,
          IdSteer    = 1,
          IdBrake    = 2,
          IdRespawn  = 3,
          IdRearView = 4,
          IdCount    = 5
        };

        irr::core::recti m_cPower;        /**< The rect which contains all power items */
        irr::core::recti m_cSteer;        /**< The rect which contains all steering items */
        irr::core::recti m_cPwrItems[3];  /**< The three items for the speed (foreward, neutral, backward) */
        irr::core::recti m_cStrItems[3];  /**< The three items for steering (left, neutral, right) */

        irr::core::recti   m_aItems [enItemIndex::ItemCount];   /**< The rects of all items of the touch controller */
        irr::core::recti   m_aSource[enItemIndex::ItemCount];   /**< The source rects of the textures */
        irr::video::SColor m_aColors[enItemIndex::ItemCount];   /**< The background colors if an item is touched */

        bool m_aTouched[enItemIndex::ItemCount];    /**< Is the item touched? */

        irr::video::ITexture *m_aTextures[enItemIndex::ItemCount];  /**< All the images of the control items */

        std::vector<enItemIndex> m_aItemMap[enTouchId::IdCount];    /**< Map that controls which touch ID controls which item */

        int m_aTouchIDs[enTouchId::IdCount];

        void initialize(const irr::core::recti &a_cRect);

      public:
        CGuiTouchControl_Split(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTouchControl_Split();

        virtual void draw() override;

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;

        virtual void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) override;
    };

    /**
    * @class CGuiTouchControl
    * @author Christian Keimel
    * This class receives touch controls and turns them into marble controls
    */
    class CGuiTouchControl : public IGuiTouchControl {
      private:
        enTouchCtrlType  m_eType;
        CGlobal         *m_pGlobal;
        int              m_iTouchIdTh;    /**< Throttle touch ID (in digital control this is for both throttle and steer) */
        int              m_iTouchIdBk;    /**< Brake touch ID (only used in the digital control versions) */
        int              m_iTouchIdRs;    /**< Respawn control */
        int              m_iTouchIdRv;    /**< Rearview control */

        irr::s8 m_iCtrlX;       /**< X (Steer) control */
        irr::s8 m_iCtrlY;       /**< Y (Throttle) control */
        irr::s8 m_aCtrl[9][2];  /**< The steer / throttle controls for the nine digital fields */

        bool m_bBrake;
        bool m_bRespawn;
        bool m_bRearView;
        bool m_bOneSide;    /**< Is the one-sided control active, i.e. 9 buttons for controlling the marble */

        irr::video::ITexture *m_aControl[9];  /**< The icons for the 9 control rects */
        irr::video::ITexture *m_pRespawn;     /**< The respawn control icon */
        irr::video::ITexture *m_pRearView;    /**< An icon for the rearview control */
        irr::video::ITexture *m_pBrake;       /**< Brake icon */

        irr::core::recti m_cDigital[9];   /**< Nine fields for digial control options */
        irr::core::recti m_cTotal;        /**< The total control rect for digital */
        irr::core::recti m_cThrottle;     /**< Throttle control (not used in the digital versions) */
        irr::core::recti m_cSteer;        /**< Steer control (not used in the digital versions) */
        irr::core::recti m_cBrake;        /**< Brake control (only used in the digital versions) */
        irr::core::recti m_cRespawn;      /**< Respawn control */
        irr::core::recti m_cRearView;     /**< Rearview control */

        irr::core::position2di m_cTouchPos[4];  /**< The touch positions (0 == marble control, 1 == brake, 2 == respawn, 3 == rearview) */

        void initialize(const irr::core::recti &a_cRect);

      public:
        CGuiTouchControl(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTouchControl();

        virtual void draw() override;

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;

        virtual void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) override;
    };
  }
}

