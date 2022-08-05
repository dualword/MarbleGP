// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <Defines.h>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Foward declaration of the global class */

  namespace gui {
    const int g_TouchControlId = MAKE_IRR_ID('t', 'o', 'c', 'h');
    const irr::c8 g_TouchControlName[] = "TouchControl";

    class CControlLayout;

    /**
    * @class IGuiTouchControl
    * @author Christian Keime
    * The interface for the two implementations of the touch marble controller
    */
    class IGuiMarbleControl : public irr::gui::IGUIElement {
      public:
        /**
        * This enumeration holds the indices for the textures of all control items.
        * Keep the touch-to-activate (rearview, respawn, reset gyro) at the end!!
        */
        enum class enItemIndex {
          ItemForeward  = 0,
          ItemNeutralP  = 1,
          ItemBackward  = 2,
          ItemBrake     = 3,
          ItemLeft      = 4,
          ItemNeutralS  = 5,
          ItemRight     = 6,
          ItemForeLeft  = 7,
          ItemForeRight = 8,
          ItemBackLeft  = 9,
          ItemBackRight = 10,
          ItemRespawn   = 11,
          ItemRearview  = 12,
          ItemResetGyro = 13,
          ItemCount     = 14
        };

      protected:
        /**
        * @class STouchItem
        * @author Christian Keimel
        * This data structure holds all necessary information
        * for the touch marble controls
        */
        struct STouchItem {
          bool m_bTouched;    /**< Is the item currently touched? */
          bool m_bActive;     /**< Can this item be touched? */

          irr::core::recti m_cButton;   /**< The actual button the image is drawn to */
          irr::core::recti m_cTouch;    /**< The touch area which might be bigger than the drawn area */
          irr::core::recti m_cSource;   /**< The source area of the image, necessary for drawing scaled images */

          irr::video::SColor    m_cColor;     /**< The background color if the item is touched */
          irr::video::ITexture *m_pTexture;   /**< The texture of the item */

          STouchItem();
          STouchItem(const STouchItem &a_cOther);   /**< Copy constructor */

          /**
          * Set the texture and initialize the source rect
          * @param a_pTexture the texture
          */
          void setTexture(irr::video::ITexture *a_pTexture);

          /**
          * Set the rectangles and the touch color
          * @param a_cButton the button rectangle on the screen
          * @param a_cTouch the touch rectangle on the screen
          * @param a_cColor the touched color of the item
          */
          void setRectAndColor(const irr::core::recti &a_cButton, const irr::core::recti &a_cTouch, const irr::video::SColor &a_cColor);
        };

        /**
        * @class STouch
        * @author Christian Keimel
        * This structure tracks all the touches on the display
        */
        typedef struct STouch {
          int                    m_iIndex;    /**< ID of the touch */
          bool                   m_bSteering; /**< Was a steering item touched? */
          bool                   m_bThrottle; /**< Was a throttle item touched? */
          irr::core::position2di m_cPos;      /**< The current position */
          irr::core::position2di m_cDown;     /**< The position of the touch-down event */

          STouch();
        } STouch;

        enTouchCtrlType  m_eType;
        CGlobal         *m_pGlobal;

        /**
        * The touch input IDs for click-on control items,
        * i.e. Rearview, Respawn and Gyro Reset
        */
        int m_aTouchIDs[5];

        /**
        * The touch input IDs for items that can also
        * be slided on, i.e. controls and brake
        */
        int m_aCtrlIDs[5];

        irr::video::IVideoDriver *m_pDrv;

        std::vector<STouchItem *> m_vItems[(int)enItemIndex::ItemCount];

        /**
        * The array to detect and trace touch events
        */
        STouch m_aTouch[5];

        /**
        * Build the UI
        * @param a_pLayout the layout
        * @param a_cScreen the screen size
        */
        void buildUI(CControlLayout *a_pLayout, irr::core::dimension2du a_cScreen);

        void handleTouchEvent();

        /**
        * Is the touched point a throttle control?
        * @param a_cPos the position of the touch
        * @return true if a throttle item was touched
        */
        bool isThrottleTouchhed(const irr::core::position2di &a_cPos);

        /**
        * Is the touched point a steering control?
        * @param a_cPos the position of the touch
        * @return true if a steering item was touched
        */
        bool isSteeringTouchhed(const irr::core::position2di &a_cPos);

        /**
        * Implementation necessary for the gyro control
        */
        virtual void resetGyro();

      public:
        IGuiMarbleControl(irr::gui::IGUIElement *a_pParent);
        virtual ~IGuiMarbleControl();

        enTouchCtrlType getType();

        /**
        * This callback gets called from the game state to get the
        * current controller state
        * @param a_iCtrlX steering output
        * @param a_iCtrlY throttle output
        * @param a_bBrake is the brake currently active?
        * @param a_bRespawn is the "respawn" button currently pressed?
        * @param a_bRearView is the "rearview" button currently pressed?
        */
        virtual void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView);

        /**
        * Implementation of the serialization method which does nothing in this case
        */
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;

        /**
        * Implementation of the deserialization method which does nothing in this case
        */
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;

        /**
        * handle Irrlicht events
        * @param a_cEvent the Irrlicht event to handle
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        virtual void draw() override;
    };

    /**
    * @class CGuiGyroControl
    * @author Christian Keimel
    * This class implements the gyroscopic control of the marble
    */
    class CGuiGyroControl : public IGuiMarbleControl {
      private:
        float m_fX;   /**< X Component of the rotation vector */
        float m_fY;   /**< Y Component of the rotation vector */
        float m_fZ;   /**< Z Component of the rotation vector */

      protected:
        virtual void resetGyro() override;

      public:
        CGuiGyroControl(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiGyroControl();

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;
        virtual void getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) override;
    };

    /**
    * @class CGuiTouchControl_Split
    * @authro Christian Keimel
    * This class implements the touch control interface with the
    * power controls on one side and the steering controls on the other
    */
    class CGuiTouchControl_Split : public IGuiMarbleControl {
      private:
        void initialize(const irr::core::recti &a_cRect);

      public:
        CGuiTouchControl_Split(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTouchControl_Split();

        /**
        * handle Irrlicht events
        * @param a_cEvent the Irrlicht event to handle
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };

    /**
    * @class CGuiTouchControl
    * @author Christian Keimel
    * This class receives touch controls and turns them into marble controls
    */
    class CGuiTouchControl : public IGuiMarbleControl {
      private:
        void initialize(const irr::core::recti &a_cRect);

        irr::core::recti m_cRects[9];

        bool checkForTouchEvents();

      public:
        CGuiTouchControl(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTouchControl();

        /**
        * handle Irrlicht events
        * @param a_cEvent the Irrlicht event to handle
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}

