// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace gui {
    /**
    * @class CVirtualKeyboard
    * @author Christian Keimel
    * After trying to integrate the original Android soft keyboard which is a pain
    * where the sun doesn't shine I decided to go for a keyboard of my own
    */
    class CVirtualKeyboard : public irr::gui::IGUIElement {
      private:
        enum class enMode {
          VirtualKeys,
          Scrolling
        };

        typedef struct SVirtualKey {
          irr::core::recti m_cRect;
          irr::EKEY_CODE   m_eKey;
          std::wstring     m_sChar;
          bool             m_bHover;
          bool             m_bHasUpper;

          SVirtualKey(const irr::core::recti &a_cRect, irr::EKEY_CODE a_eKey, const std::wstring &a_sChar, bool a_bHasUpper);
        }
        SVirtualKey;

        irr::IrrlichtDevice *m_pDevice;

        std::vector<wchar_t> m_vKeys;
        std::vector<wchar_t>::iterator m_itKey;

        irr::gui::IGUIFont *m_pFont;

        irr::video::IVideoDriver *m_pDrv;

        irr::core::recti m_cInner;
        irr::core::recti m_cChar;
        irr::core::recti m_cUp;
        irr::core::recti m_cDown;

        irr::core::position2di m_cMouse;
        irr::core::position2di m_cMDown;
        irr::core::position2di m_cOffset;
        irr::core::position2di m_cTotalMove;

        irr::video::SColor m_cBackground;

        bool m_bMouseDown;
        bool m_bInside;
        bool m_bMoved;
        bool m_bCtrlActive;
        bool m_bMoving;
        bool m_bShift;

        int m_iMouseDown;

        enMode m_eMode;

        std::vector<SVirtualKey> m_vVirtualKeys;

        irr::core::position2di  m_cVkOffset;
        irr::core::dimension2du m_cVkSize;

        irr::ITimer           *m_pTimer;
        irr::gui::IGUIEditBox *m_pTarget;

        void initUI();
        void hideKeyboard();

        void addChar();
        void delChar();

      public:
        CVirtualKeyboard(bool a_bCtrlActive);
        virtual ~CVirtualKeyboard();

        /** Inherited from irr::gui::IGUIElement **/
        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;
        virtual void draw() override;

        void setTarget(irr::gui::IGUIEditBox *a_pTarget);
    };
  }
}
