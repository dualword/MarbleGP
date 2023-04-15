// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/IGuiControllerUiCallback.h>
#include <controller/CControllerBase.h>
#include <gui/CControllerUi_Menu.h>
#include <gui/CControllerUi_Game.h>
#include <gui/CDustbinScrollPane.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <gui/CControllerUi.h>
#include <menu/IMenuManager.h>
#include <irrlicht.h>
#include <Defines.h>
#include <algorithm>
#include <CGlobal.h>
#include <string>


namespace dustbin {
  namespace helpers {
    
  }

  namespace gui {
    CControllerUi::SGuiElement::SGuiElement() {
    }

    CControllerUi::SJoystickState::SJoystickState() : m_iPov(0xFFFF) {
      for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_AXES; i++)
        m_iAxis[i] = 0;

      for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS; i++)
        m_bButton[i] = false;
    }

    void CControllerUi::SJoystickState::update(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_AXES; i++)
          m_iAxis[i] = a_cEvent.JoystickEvent.Axis[i];

        for (int i = 0; i < a_cEvent.JoystickEvent.NUMBER_OF_BUTTONS; i++)
          m_bButton[i] = a_cEvent.JoystickEvent.IsButtonPressed(i);
      }
    }

    bool CControllerUi::SJoystickState::isNeutral() {
      for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_AXES; i++)
        if (m_iAxis[i] < -5000 || m_iAxis[i] > 5000)
          return false;

      for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS; i++)
        if (m_bButton[i])
          return false;

      if (m_iPov != 0xFFFF)
        return false;

      return true;
    }

    bool CControllerUi::SJoystickState::hasChanged(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_AXES; i++) {
          if ((a_cEvent.JoystickEvent.Axis[i] < -5000 || a_cEvent.JoystickEvent.Axis[i] > 5000) != (m_iAxis[i] < -5000 || m_iAxis[i] > 5000))
            return true;
        }

        for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS; i++)
          if (m_bButton[i] != a_cEvent.JoystickEvent.IsButtonPressed(i))
            return true;

        if (m_iPov != a_cEvent.JoystickEvent.POV)
          return true;
      }

      return false;
    }

    CControllerUi::CControllerUi(irr::gui::IGUIElement* a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType) :
      irr::gui::IGUIElement(a_eType, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()), 
      m_pController(nullptr),
      m_pDrv       (CGlobal::getInstance()->getVideoDriver()),
      m_eMode      (enMode::Display),
      m_eCtrl      (enControl::Keyboard),
      m_bSet       (true),
      m_iJoystick  (255)
    {
    }

    CControllerUi::~CControllerUi() {
      if (m_pController != nullptr) {
        delete m_pController;
        m_pController = nullptr;
      }
    }

    /**
    * Get the configuration of the controller as serialized string
    * @return the configuration of the controller
    */
    std::string CControllerUi::getControllerString() {
      if (m_pController!= nullptr)
        return m_pController->serialize();
      else
        return "";
    }

    /**
    * The "setText" method is abused to pass the configuration serialized string to the UI
    * @param a_pText the new serialized string
    */
    void CControllerUi::setText(const wchar_t* a_pText) {
      irr::gui::IGUIElement::setText(a_pText);
    }

    /**
    * Set the controller which is to be configured
    * @param a_sCtrl the controller string the controller is constructed from
    */
    void CControllerUi::setController(const std::string& a_sCtrl) {
      if (m_pController != nullptr)
        delete m_pController;

      m_pController = new controller::CControllerBase();
      m_pController->deserialize(a_sCtrl);
    }


    bool CControllerUi::OnJoystickEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      // Only listen to joystick events if the mode is set to Joystick
      if (m_eCtrl == enControl::Joystick) {
        if (m_eMode == enMode::Wizard) {
          if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (m_mJoysticks.find(a_cEvent.JoystickEvent.Joystick) == m_mJoysticks.end()) {
              m_mJoysticks[a_cEvent.JoystickEvent.Joystick] = SJoystickState();
              m_mJoysticks[a_cEvent.JoystickEvent.Joystick].update(a_cEvent);

              printf("Joystick %i added.\n", a_cEvent.JoystickEvent.Joystick);
            }
            
            if (m_mJoysticks.find(a_cEvent.JoystickEvent.Joystick) != m_mJoysticks.end()) {
              if (m_iJoystick == 255 && m_mJoysticks[a_cEvent.JoystickEvent.Joystick].hasChanged(a_cEvent))
                m_iJoystick = a_cEvent.JoystickEvent.Joystick;

              if (m_iJoystick == a_cEvent.JoystickEvent.Joystick) {
                if (m_bSet) {
                  if (m_mJoysticks[m_iJoystick].hasChanged(a_cEvent)) {
                    for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_AXES; i++) {
                      if ((m_mJoysticks[m_iJoystick].m_iAxis[i] < -5000 || m_mJoysticks[m_iJoystick].m_iAxis[i] > 5000) != (a_cEvent.JoystickEvent.Axis[i] < -5000 || a_cEvent.JoystickEvent.Axis[i] > 5000)) {
                        m_pController->getInputs()[m_iWizard].m_eType      = controller::CControllerBase::enInputType::JoyAxis;
                        m_pController->getInputs()[m_iWizard].m_iAxis      = i;
                        m_pController->getInputs()[m_iWizard].m_iDirection = a_cEvent.JoystickEvent.Axis[i] > m_mJoysticks[m_iJoystick].m_iAxis[i] ? 1 : -1;
                        m_mJoysticks[m_iJoystick].update(a_cEvent);
                        m_bSet = false;
                        break;
                      }
                    }

                    if (m_bSet) {
                      for (int i = 0; i < irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS; i++) {
                        if (a_cEvent.JoystickEvent.IsButtonPressed(i) != m_mJoysticks[m_iJoystick].m_bButton[i]) {
                          m_pController->getInputs()[m_iWizard].m_eType   = controller::CControllerBase::enInputType::JoyButton;
                          m_pController->getInputs()[m_iWizard].m_iButton = i;
                          m_mJoysticks[m_iJoystick].update(a_cEvent);
                          m_bSet = false;
                          break;
                        }
                      }
                    }

                    if (m_bSet) {
                      if (a_cEvent.JoystickEvent.POV != m_mJoysticks[m_iJoystick].m_iPov) {
                        m_pController->getInputs()[m_iWizard].m_eType = controller::CControllerBase::enInputType::JoyPov;
                        m_pController->getInputs()[m_iWizard].m_iPov  = a_cEvent.JoystickEvent.POV;
                        m_mJoysticks[m_iJoystick].update(a_cEvent);
                        m_bSet = false;
                      }
                    }
                  }
                }
                else {
                  if (
                    (m_pController->getInputs()[m_iWizard].m_eType == controller::CControllerBase::enInputType::JoyAxis   && std::abs(a_cEvent.JoystickEvent.Axis[m_pController->getInputs()[m_iWizard].m_iAxis]) < 1000) || 
                    (m_pController->getInputs()[m_iWizard].m_eType == controller::CControllerBase::enInputType::JoyButton && !a_cEvent.JoystickEvent.IsButtonPressed(m_pController->getInputs()[m_iWizard].m_iButton)) ||
                    (m_pController->getInputs()[m_iWizard].m_eType == controller::CControllerBase::enInputType::JoyPov    && a_cEvent.JoystickEvent.POV == 0xFFFF)
                  ) {
                    m_bSet = true;
                    m_iWizard++;

                    m_mJoysticks[m_iJoystick].update(a_cEvent);

                    if (m_iWizard >= m_pController->getInputs().size())
                      setMode(enMode::Display);
                  }
                }
              }
            }
          }

          l_bRet = true;
        }
      }

      return l_bRet;
    }

    bool CControllerUi::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        // Only listen to keyboard events if the mode is set to keyboard
        if (m_eCtrl == enControl::Keyboard) {
          if (m_eMode == enMode::Wizard) {

          }
        }
      }

      return l_bRet;
    }

    void CControllerUi::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);
    }

    void CControllerUi::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
    }

    void CControllerUi::draw() {
      irr::gui::IGUIElement::draw();

      m_pDrv->draw2DRectangleOutline(AbsoluteClippingRect, irr::video::SColor(0xFF, 0, 0, 0));

      if (m_vGui.size() > 0 && m_eCtrl != enControl::Off) {
        irr::core::dimension2du l_cCtrlSize;

        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator l_itCtrl = m_pController->getInputs().begin(); l_itCtrl != m_pController->getInputs().end(); l_itCtrl++) {
          irr::core::dimension2du d = m_pFont->getDimension((*l_itCtrl).getControlString().c_str());

          if (l_cCtrlSize.Width  < d.Width ) l_cCtrlSize.Width  = d.Width;
          if (l_cCtrlSize.Height < d.Height) l_cCtrlSize.Height = d.Height;
        }

        l_cCtrlSize.Width += (*m_vGui.begin()).m_cPosColn.X - (*m_vGui.begin()).m_cPosBack.X + (*m_vGui.begin()).m_cDimHead.Height;

        std::vector<SGuiElement>::iterator l_itGui = m_vGui.begin();

        int l_iIndex = 0;

        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator l_itCtrl = m_pController->getInputs().begin(); l_itCtrl != m_pController->getInputs().end() && l_itGui != m_vGui.end(); l_itCtrl++) {

          irr::core::dimension2du l_cSize = irr::core::dimension2du(
            m_pFont->getDimension((*l_itCtrl).getControlString().c_str()).Width,
            (*l_itGui).m_cDimHead.Height
          );

          irr::core::recti l_cHead = irr::core::recti((*l_itGui).m_cPosHead, (*l_itGui).m_cDimHead);
          irr::core::recti l_cColn = irr::core::recti((*l_itGui).m_cPosColn, (*l_itGui).m_cDimColn);
          irr::core::recti l_cCtrl = irr::core::recti((*l_itGui).m_cPosCtrl, l_cSize);
          irr::core::recti l_cRect = irr::core::recti((*l_itGui).m_cPosBack, l_cCtrlSize);

          switch (m_eMode) {
            case enMode::Display:
              m_pDrv->draw2DRectangle(irr::video::SColor(128, 192, 192, 192), l_cRect, &AbsoluteClippingRect);

              m_pFont->draw(helpers::s2ws((*l_itCtrl).m_sName).c_str(), l_cHead, irr::video::SColor(0xFF, 0, 0, 0), false, true, &AbsoluteClippingRect);
              m_pFont->draw(L":"                                              , l_cColn, irr::video::SColor(0xFF, 0, 0, 0), false, true, &AbsoluteClippingRect);
              m_pFont->draw((*l_itCtrl).getControlString()            .c_str(), l_cCtrl, irr::video::SColor(0xFF, 0, 0, 0), false, true, &AbsoluteClippingRect);
              break;

            case enMode::Wizard:
              irr::video::SColor l_cBack = irr::video::SColor( 96, 224, 224, 224);
              irr::video::SColor l_cText = irr::video::SColor(128,   0,   0,   0);

              if (l_iIndex == m_iWizard) {
                l_cBack = irr::video::SColor(128, 192, 192, 255);
                l_cText = irr::video::SColor(255,   0,   0,   0);
              }
              else if (l_iIndex < m_iWizard) {
                l_cBack = irr::video::SColor(128, 192, 192, 192);
                l_cText = irr::video::SColor(255,   0,   0,   0);
              }

              m_pDrv->draw2DRectangle(l_cBack, l_cRect, &AbsoluteClippingRect);

              m_pFont->draw(helpers::s2ws((*l_itCtrl).m_sName).c_str(), l_cHead, l_cText, false, true, &AbsoluteClippingRect);
              m_pFont->draw(L":"                                              , l_cColn, l_cText, false, true, &AbsoluteClippingRect);
              m_pFont->draw((*l_itCtrl).getControlString()            .c_str(), l_cCtrl, l_cText, false, true, &AbsoluteClippingRect);
              break;
          }

          l_itGui++;
          l_iIndex++;
        }
      }
    }

    void CControllerUi::calculateGui() {
      if (m_pController != nullptr) {
        std::vector<controller::CControllerBase::SCtrlInput> l_vItems = m_pController->getInputs();

        irr::core::dimension2di l_cHead;
        irr::core::dimension2di l_cColn;

        m_pFont = nullptr;

        enFont l_aFonts[] = {
          enFont::Huge,
          enFont::Big,
          enFont::Regular,
          enFont::Small,
          enFont::Tiny
        };

        irr::core::dimension2du l_cSize;

        for (int i = 0; i < 5; i++) {
          m_pFont = CGlobal::getInstance()->getFont(l_aFonts[i], m_pDrv->getScreenSize());
          l_cSize = m_pFont->getDimension(L"TestString");

          int l_iHeight = (int)((l_vItems.size() * 2) + 1) * l_cSize.Height;

          if (l_iHeight < AbsoluteClippingRect.getHeight())
            break;
        }

        l_cHead.Height = l_cSize.Height;
        l_cColn.Height = l_cSize.Height;

        irr::core::vector2di l_cPos = AbsoluteClippingRect.UpperLeftCorner + irr::core::vector2di(0, l_cSize.Height);

        l_cSize = irr::core::dimension2du();

        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator l_itCtrl = l_vItems.begin(); l_itCtrl != l_vItems.end(); l_itCtrl++) {
          irr::core::dimension2du d = m_pFont->getDimension(helpers::s2ws((*l_itCtrl).m_sName).c_str());

          if (d.Width  > l_cSize.Width ) l_cSize.Width  = d.Width;
          if (d.Height > l_cSize.Height) l_cSize.Height = d.Height;
        }

        m_vGui.clear();

        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator l_itCtrl = l_vItems.begin(); l_itCtrl != l_vItems.end(); l_itCtrl++) {
          m_vGui.push_back(SGuiElement());

          m_vGui.back().m_cDimHead = m_pFont->getDimension(helpers::s2ws((*l_itCtrl).m_sName).c_str());
          m_vGui.back().m_cDimColn = m_pFont->getDimension(L":");
          m_vGui.back().m_cDimColn.Width *= 2;
          m_vGui.back().m_cPosHead = l_cPos + irr::core::vector2di(l_cSize.Height / 2 + l_cSize.Width - m_vGui.back().m_cDimHead.Width, 0);
          m_vGui.back().m_cPosColn = m_vGui.back().m_cPosHead + irr::core::vector2di(m_vGui.back().m_cDimHead.Width, 0);
          m_vGui.back().m_cPosCtrl = m_vGui.back().m_cPosColn + irr::core::vector2di(m_vGui.back().m_cDimColn.Width, 0);
          m_vGui.back().m_cPosBack = l_cPos;

          l_cPos.Y += 2 * l_cSize.Height;
        }
      }
    }

    /**
    * Set the mode of the controller UI
    * @param a_eMode the new mode
    */
    void CControllerUi::setMode(enMode a_eMode) {
      m_eMode = a_eMode;
    }

    /**
    * Set the control type of the UI
    * @param a_eCtrl the new control type
    */
    void CControllerUi::setControlType(enControl a_eCtrl) {
      m_eCtrl = a_eCtrl;
    }

    /**
    * Start the controller configuration wizard
    */
    void CControllerUi::startWizard() {
      m_eMode     = enMode::Wizard;
      m_iWizard   = 0;
      m_bSet      = true;
      m_iJoystick = 255;
    }

    std::string CControllerUi::serialize() {
      if (m_pController != nullptr)
        return m_pController->serialize();
      else
        return "";
    }
  } // namespace controller 
} // namespace dustbin