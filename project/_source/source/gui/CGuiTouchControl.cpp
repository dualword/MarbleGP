// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGuiTouchControl.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace gui {
    CGuiTouchControl::CGuiTouchControl(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId, 
        CGlobal::getInstance()->getGuiEnvironment(), 
        a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
        -1, 
        irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
      ),
      m_eType     (enType::ControlLeft),
      m_pGlobal   (CGlobal::getInstance()),
      m_iTouchIdTh(-1),
      m_iTouchIdBk(-1),
      m_iTouchIdRs(-1),
      m_iTouchIdRv(-1),
      m_iCtrlX    (0),
      m_iCtrlY    (0),
      m_bBrake    (false),
      m_bRespawn  (false),
      m_bRearView (false),
      m_pRespawn  (nullptr),
      m_pRearView (nullptr),
      m_pBrake    (nullptr),
      m_pDrv      (CGlobal::getInstance()->getVideoDriver())
    {
      m_eType = (enType)(m_pGlobal->getSettingData().m_iTouchType);
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
    }

    void CGuiTouchControl::initialize(const irr::core::recti &a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16,
               l_iOffset = a_cRect.getWidth() / 256;

      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);

      m_cRespawn = irr::core::recti(
        m_eType == enType::ControlRight ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, a_cRect.getHeight() / 4) : irr::core::position2di(l_iOffset, a_cRect.getHeight() / 4),
        l_cSize
      );

      m_cRearView = irr::core::recti(
        m_eType == enType::ControlLeft ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, a_cRect.getHeight() / 4) : irr::core::position2di(l_iOffset, a_cRect.getHeight() / 4),
        l_cSize
      );

      irr::core::position2di l_cStart = irr::core::position2di(
        m_eType == enType::ControlLeft ? l_iOffset : a_cRect.LowerRightCorner.X - 3 * (l_iSize + l_iOffset),
        a_cRect.LowerRightCorner.Y - 3 * (l_iSize + l_iOffset) - 3 * l_iOffset
      );

      for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3; x++) {
          irr::core::position2di l_cOffset = irr::core::position2di(x * (l_iSize + l_iOffset), y * (l_iSize + l_iOffset));

          int l_iIndex = x + 3 * y;

          m_cDigital[l_iIndex] = irr::core::recti(l_cStart + l_cOffset, l_cSize);
          m_aControl[l_iIndex] = m_pDrv->getTexture((std::string("data/images/control") + std::to_string(l_iIndex) + ".png").c_str());
        }
      }

      m_aCtrl[0][0] = -127; m_aCtrl[0][1] =  127;
      m_aCtrl[1][0] =    0; m_aCtrl[1][1] =  127;
      m_aCtrl[2][0] =  127; m_aCtrl[2][1] =  127;
      m_aCtrl[3][0] = -127; m_aCtrl[3][1] =    0;
      m_aCtrl[4][0] =    0; m_aCtrl[4][1] =    0;
      m_aCtrl[5][0] =  127; m_aCtrl[5][1] =    0;
      m_aCtrl[6][0] = -127; m_aCtrl[6][1] = -127;
      m_aCtrl[7][0] =    0; m_aCtrl[7][1] = -127;
      m_aCtrl[8][0] =  127; m_aCtrl[8][1] = -127;

      m_cTotal = irr::core::recti(m_cDigital[0].UpperLeftCorner, m_cDigital[8].LowerRightCorner);

      l_cSize.Width *= 3;

      m_cBrake = irr::core::recti(
        m_eType == enType::ControlLeft ? 
          irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, a_cRect.getHeight() - l_cSize.Height - l_iOffset)
        :
          irr::core::position2di(l_iOffset                                     , a_cRect.getHeight() - l_cSize.Height - l_iOffset), l_cSize
      );

      m_pRearView = m_pDrv->getTexture("data/images/rearview.png");
      m_pRespawn  = m_pDrv->getTexture("data/images/respawn.png" );
      m_pBrake    = m_pDrv->getTexture("data/images/brake.png"   );
    }

    CGuiTouchControl::~CGuiTouchControl() {
    }

    void CGuiTouchControl::draw() {
      if (IsVisible) {
        m_pDrv->draw2DRectangle((m_iTouchIdRs != -1 && m_cRespawn.isPointInside(m_cTouchPos[2])) ? irr::video::SColor(192, 192, 192, 96) : irr::video::SColor(192, 192, 192, 192), m_cRespawn);
        m_pDrv->draw2DRectangleOutline(m_cRespawn, irr::video::SColor(0xFF, 0, 0, 0));

        if (m_pRespawn != nullptr)
          m_pDrv->draw2DImage(m_pRespawn, m_cRespawn, irr::core::recti(irr::core::position2di(0, 0), m_pRespawn->getSize()), nullptr, nullptr, true);

        m_pDrv->draw2DRectangle((m_iTouchIdRv != -1 && m_cRearView.isPointInside(m_cTouchPos[3])) ? irr::video::SColor(192, 96, 96, 192) : irr::video::SColor(192, 192, 192, 192), m_cRearView);
        m_pDrv->draw2DRectangleOutline(m_cRearView, irr::video::SColor(0xFF, 0, 0, 0));

        if (m_pRearView != nullptr)
          m_pDrv->draw2DImage(m_pRearView, m_cRearView, irr::core::recti(irr::core::position2di(0, 0), m_pRearView->getSize()), nullptr, nullptr, true);

        for (int i = 0; i < 9; i++) {
          m_pDrv->draw2DRectangle((m_iTouchIdTh != -1 && m_cDigital[i].isPointInside(m_cTouchPos[0])) ? irr::video::SColor(192, 96, 192, 96) : irr::video::SColor(192, 192, 192, 192), m_cDigital[i]);
          m_pDrv->draw2DRectangleOutline(m_cDigital[i], irr::video::SColor(0xFF, 0, 0, 0));

          if (m_aControl[i] != nullptr)
            m_pDrv->draw2DImage(m_aControl[i], m_cDigital[i], irr::core::recti(irr::core::position2di(0, 0), m_aControl[i]->getSize()), nullptr, nullptr, true);
        }

        m_pDrv->draw2DRectangle((m_iTouchIdBk != -1 && m_cBrake.isPointInside(m_cTouchPos[1])) ? irr::video::SColor(192, 255, 96, 96) : irr::video::SColor(192, 192, 192, 192), m_cBrake);

        if (m_pBrake != nullptr)
          m_pDrv->draw2DImage(m_pBrake, m_cBrake, irr::core::recti(irr::core::position2di(0, 0), m_pBrake->getSize()), nullptr, nullptr, true);
      }
    }

    bool CGuiTouchControl::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          if (m_cRespawn.isPointInside(l_cPos) && m_iTouchIdRs == -1) {
            m_iTouchIdRs = a_cEvent.TouchInput.ID;
            m_cTouchPos[2] = l_cPos;
            m_bRespawn = true;
            l_bRet = true;
          }

          if (m_cRearView.isPointInside(l_cPos) && m_iTouchIdRv == -1) {
            m_iTouchIdRv = a_cEvent.TouchInput.ID;
            m_cTouchPos[3] = l_cPos;
            m_bRearView = true;
            l_bRet = true;
          }

          if (m_iTouchIdTh == -1) {
            if (m_cTotal.isPointInside(l_cPos)) {
              m_iTouchIdTh = a_cEvent.TouchInput.ID;
              m_cTouchPos[0] = l_cPos;

              for (int i = 0; i < 9; i++)
                if (m_cDigital[i].isPointInside(l_cPos)) {
                  m_iCtrlX = m_aCtrl[i][0];
                  m_iCtrlY = m_aCtrl[i][1];

                  l_bRet = true;
                }
            }
          }

          if (m_iTouchIdBk == -1) {
            if (m_cBrake.isPointInside(l_cPos)) {
              m_iTouchIdBk = a_cEvent.TouchInput.ID;
              m_cTouchPos[1] = l_cPos;
              m_bBrake = true;

              l_bRet = true;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (m_iTouchIdRs != -1) {
            m_cTouchPos[2] = l_cPos;
            m_bRespawn = m_cRespawn.isPointInside(l_cPos);
            l_bRet = true;
          }

          if (m_iTouchIdRv != -1) {
            m_cTouchPos[3] = l_cPos;
            m_bRearView = m_cRearView.isPointInside(l_cPos);
            l_bRet = true;
          }

          if (m_iTouchIdTh == a_cEvent.TouchInput.ID) {
            m_cTouchPos[0] = l_cPos;

            for (int i = 0; i < 9; i++)
              if (m_cDigital[i].isPointInside(l_cPos)) {
                m_iCtrlX = m_aCtrl[i][0];
                m_iCtrlY = m_aCtrl[i][1];
              }

            l_bRet = true;
          }

          if (m_iTouchIdBk == a_cEvent.TouchInput.ID) {
            m_cTouchPos[1] = l_cPos;
            m_bBrake = m_cBrake.isPointInside(l_cPos);

            l_bRet = true;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (m_iTouchIdRs == a_cEvent.TouchInput.ID) {
            m_iTouchIdRs = -1;
            m_bRespawn = false;
          }

          if (m_iTouchIdRv == a_cEvent.TouchInput.ID) {
            m_iTouchIdRv = -1;
            m_bRearView = false;
          }

          if (m_iTouchIdTh == a_cEvent.TouchInput.ID) {
            m_iTouchIdTh = -1;
            m_iCtrlX = 0;
            m_iCtrlY = 0;

            l_bRet = true;
          }

          if (m_iTouchIdBk == a_cEvent.TouchInput.ID) {
            m_iTouchIdBk = -1;
            m_bBrake = false;

            l_bRet = true;
          }
        }
      }

      return l_bRet;
    }

    void CGuiTouchControl::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
    }

    void CGuiTouchControl::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
    }

    void CGuiTouchControl::getControl(irr::s8& a_iCtrlX, irr::s8& a_iCtrlY, bool& a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      a_iCtrlX    = m_iCtrlX;
      a_iCtrlY    = m_iCtrlY;
      a_bBrake    = m_bBrake;
      a_bRespawn  = m_bRespawn;
      a_bRearView = m_bRearView;
    }
  }
}
