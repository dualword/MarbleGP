// (w) 2021 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <lua/CLuaScript_dialog.h>
#include <platform/CPlatform.h>
#include <gui_freetype_font.h>
#include <lua/CLuaHelpers.h>
#include <state/CLuaState.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace state {
    CLuaState::CLuaState() : 
      m_pDevice  (CGlobal::getInstance()->getIrrlichtDevice()), 
      m_pSmgr    (CGlobal::getInstance()->getSceneManager  ()), 
      m_pDrv     (CGlobal::getInstance()->getVideoDriver   ()), 
      m_pGui     (CGlobal::getInstance()->getGuiEnvironment()),
      m_pGlobal  (CGlobal::getInstance()),
      m_pMenuCtrl(nullptr),
      m_pScript  (nullptr),
      m_pTimer   (nullptr),
      m_iZLayer  (-1)
    {
      for (int i = 0; i < 3; i++)
        m_bButtons[i] = false;

      m_pTimer = m_pDevice->getTimer();
    }

    CLuaState::~CLuaState() {
    }

    /**
    * This method is called when the state is activated
    */
    void CLuaState::activate() {
      m_iZLayer = -1;

      std::string l_sScriptName = m_pGlobal->popScript(),
                  l_sScript = lua::loadLuaScript(l_sScriptName);

      irr::core::dimension2du l_cDim = m_pDrv->getScreenSize();
      onResize(l_cDim);

      m_pScript = new lua::CLuaScript_dialog(l_sScript);
      m_pScript->initialize();

      if (m_pGlobal->getSettings().m_misc_usemenuctrl)
        m_pMenuCtrl = new controller::CControllerMenu(m_iZLayer);

      m_bDefCanc[0] = false;
      m_bDefCanc[1] = false;
    }

    /**
    * This method is called when the state is deactivated
    */
    void CLuaState::deactivate() {
      if (m_pScript != nullptr) {
        m_pScript->cleanup();
        delete m_pScript;
        m_pScript = nullptr;
      }

      if (m_pMenuCtrl != nullptr) {
        delete m_pMenuCtrl;
        m_pMenuCtrl = nullptr;
      }
    }

    /**
    * Return the state's ID
    */
    enState CLuaState::getId() {
      return enState::LuaState;
    }

    /**
    * This method is called before the UI is cleared on window resize. It can be
    * used to save all necessary data to re-build the UI
    */
    void CLuaState::beforeResize() {
      if (m_pScript != nullptr)
        m_pScript->beforeResize();
    }

    /**
    * Change the Z-Layer for the Menu Controller
    * @param a_iZLayer the new Z-Layer
    */
    void CLuaState::setZLayer(int a_iZLayer) {
      m_iZLayer = a_iZLayer;
      if (m_pMenuCtrl != nullptr)
        m_pMenuCtrl->setZLayer(a_iZLayer);
    }

    /**
    * This is a callback method that gets invoked when the window is resized
    * @param a_cDim the new dimension of the window
    */
    void CLuaState::onResize(const irr::core::dimension2du& a_cDim) {
      irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->getActiveCamera();
      if (l_pCam != nullptr) {
        irr::f32 l_fRatio = (irr::f32)a_cDim.Width / (irr::f32)a_cDim.Height;
        l_pCam->setAspectRatio(l_fRatio);
      }

      if (m_pScript != nullptr)
        m_pScript->windowresized();
    }

    /**
    * Event handling method. The main class passes all Irrlicht events to this method
    */
    bool CLuaState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (m_pMenuCtrl != nullptr)
        m_pMenuCtrl->update(a_cEvent);

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          m_pScript->uiButtonClicked(a_cEvent.GUIEvent.Caller->getID(), a_cEvent.GUIEvent.Caller->getName());
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
          if (a_cEvent.GUIEvent.Caller->getType() == gui::g_SelectorId) {
            m_pScript->uiValueChanged(a_cEvent.GUIEvent.Caller->getID(), a_cEvent.GUIEvent.Caller->getName(), (float)reinterpret_cast<gui::CSelector*>(a_cEvent.GUIEvent.Caller)->getSelected());
          }
          else {
            m_pScript->uiValueChanged(a_cEvent.GUIEvent.Caller->getID(), a_cEvent.GUIEvent.Caller->getName(), (float)reinterpret_cast<irr::gui::IGUIScrollBar*>(a_cEvent.GUIEvent.Caller)->getPos());
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED) {
          m_pScript->uiTextChanged(a_cEvent.GUIEvent.Caller->getID(), a_cEvent.GUIEvent.Caller->getName(), platform::ws2s(a_cEvent.GUIEvent.Caller->getText()));
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_LISTBOX_CHANGED || a_cEvent.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN) {
          m_pScript->uiValueChanged(a_cEvent.GUIEvent.Caller->getID(), a_cEvent.GUIEvent.Caller->getName(), (float)reinterpret_cast<irr::gui::IGUIListBox*>(a_cEvent.GUIEvent.Caller)->getSelected());
        }
      }
      else if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        // If a key event was passed we need to handle "Return" and "Escape" for default OK and cancel buttons
        if (a_cEvent.KeyInput.PressedDown) {
          if (a_cEvent.KeyInput.Key == irr::KEY_RETURN) m_bDefCanc[0] = true;
          if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) m_bDefCanc[1] = true;
        }
        else {
          if (a_cEvent.KeyInput.Key == irr::KEY_RETURN && m_bDefCanc[0]) {  
            if (m_pScript != nullptr && isDefaultEnabled()) {
              irr::gui::IGUIElement* p = m_pScript->getDefaultElement(false);
              if (p != nullptr)
                m_pScript->uiButtonClicked(p->getID(), p->getName());
            }
          }
          else if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE && m_bDefCanc[1]) {
            if (m_pScript != nullptr && isDefaultEnabled()) {
              irr::gui::IGUIElement* p = m_pScript->getDefaultElement(true);
              if (p != nullptr)
                m_pScript->uiButtonClicked(p->getID(), p->getName());
            }
          }
          m_bDefCanc[0] = false;
          m_bDefCanc[1] = false;
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        m_bButtons[0] = a_cEvent.MouseInput.isLeftPressed  ();
        m_bButtons[1] = a_cEvent.MouseInput.isMiddlePressed();
        m_bButtons[2] = a_cEvent.MouseInput.isRightPressed ();
      }
      else if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        for (std::vector<state::IJoystickEventHandler*>::iterator it = m_vJoystickHandlers.begin(); it != m_vJoystickHandlers.end(); it++)
          (*it)->OnJoystickEvent(a_cEvent);
      }

      return l_bRet;
    }


    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CLuaState::run() {
      if (m_pScript != nullptr)
        m_pScript->step(m_pTimer->getTime());

      return m_pGlobal->getStateChange();
    }

    void CLuaState::onUievent(const std::string& a_type, irr::s32 a_id, const std::string& a_name, const std::string& a_data) {
      if (m_pScript != nullptr) {
        if (a_type == "uielementhovered")
          m_pScript->uiElementHovered(a_id, a_name);
        else if (a_type == "uielementleft")
          m_pScript->uiElementLeft(a_id, a_name);
        else if (a_type == "uibuttonclicked")
          m_pScript->uiButtonClicked(a_id, a_name);
        else if (a_type == "uivaluechanged")
          m_pScript->uiValueChanged(a_id, a_name, (float)std::atof(a_data.c_str()));
      }
    }

    bool CLuaState::isMouseDown(enMouseButton a_eButton) {
      switch (a_eButton) {
        case enMouseButton::Left  : return m_bButtons[0];
        case enMouseButton::Middle: return m_bButtons[1];
        case enMouseButton::Right : return m_bButtons[2];
        default: return false;
      }
    }
  }
}