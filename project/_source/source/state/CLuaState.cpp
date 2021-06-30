// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaScript_dialog.h>
#include <gui_freetype_font.h>
#include <lua/CLuaHelpers.h>
#include <state/CLuaState.h>
#include <gui/CDialog.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace state {
    CLuaState::CLuaState() : 
      m_pDevice (CGlobal::getInstance()->getIrrlichtDevice()), 
      m_pSmgr   (CGlobal::getInstance()->getSceneManager  ()), 
      m_pDrv    (CGlobal::getInstance()->getVideoDriver   ()), 
      m_pGui    (CGlobal::getInstance()->getGuiEnvironment()),
      m_pGlobal (CGlobal::getInstance()),
      m_pScript (nullptr),
      m_pTimer  (nullptr),
      m_pDialog (nullptr)
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
      std::string l_sScriptName = m_pGlobal->popScript(),
                  l_sScript = lua::loadLuaScript(l_sScriptName);

      m_pScript = new lua::CLuaScript_dialog(l_sScript);
      m_pScript->initialize();

      m_pDialog = new gui::CDialog();
      m_pDialog->loadDialog("data/menu/menu_main.xml");
      m_pDialog->loadDialog("data/menu/button_ok.xml");
      m_pDialog->loadDialog("data/menu/button_cancel.xml");

      irr::core::dimension2du l_cDim = m_pDrv->getScreenSize();
      onResize(l_cDim);
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

      delete m_pDialog;
      m_pDialog = nullptr;
    }

    /**
    * Return the state's ID
    */
    enState CLuaState::getId() {
      return enState::LuaState;
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

      // if (m_pScript != nullptr)
      //   m_pScript->windowresized();
      printf("==> %i, %i\n", a_cDim.Width, a_cDim.Height);

      CGUITTFace* l_pFace = new CGUITTFace();
      l_pFace->load("data/fonts/adventpro-regular.ttf");

      CGUIFreetypeFont* l_pFont = new CGUIFreetypeFont(m_pDrv);
      l_pFont->AntiAlias = true;

      l_pFont->attach(l_pFace, a_cDim.Height / 100);  // Tiny: Height / 100,  Small: Height / 80, Regular: Height / 60, Big = Height / 45, Huge = Height / 30

      m_pGui->clear();

      /*for (int y = -20; y <= 20; y++) {
        for (int x = -30; x <= 30; x++) {
          std::wstring s = L"";

          if (x == 0) {
            s = std::to_wstring(y);
          }
          else if (y == 0) {
            s = std::to_wstring(x);
          }
          else if (x == 0 && y == 0) {
            s = L"X";
          }

          for (int i = 0; i < 9; i++) {
            bool l_bAdd = false;
            irr::core::recti l_cRect;

            if (i == 0) {
              l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Center);
              l_bAdd = true;
            }
            else if (i == 1) {
              if (x >= 0 && x < 5 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperLeft);
                l_bAdd = true;
              }
            }
            else if (i == 2) {
              if (x >= -20 && x <= 20 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperMiddle);
                l_bAdd = true;
              }
            }
            else if (i == 3) {
              if (x > -5 && x <= 0 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperRight);
                l_bAdd = true;
              }
            }
            else if (i == 4) {
              if (x >= 0 && x < 5 && y >= -10 && y <= 10) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Left);
                l_bAdd = true;
              }
            }
            else if (i == 5) {
              if (x > -5 && x <= 0 && y >= -10 && y <= 10) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Right);
                l_bAdd = true;
              }
            }
            else if (i == 6) {
              if (x >= 0 && x < 5 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerLeft);
                l_bAdd = true;
              }
            }
            else if (i == 7) {
              if (x >= -10 && x <= 10 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerMiddle);
                l_bAdd = true;
              }
            }
            else if (i == 8) {
              if (x > -5 && x <= 0 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerRight);
                l_bAdd = true;
              }
            }

            if (l_bAdd) {
              irr::gui::IGUIStaticText* p = m_pGui->addStaticText(s.c_str(), l_cRect, true, true, nullptr, -1, true);
              p->setOverrideFont(l_pFont);
              p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            }
          }
        }
      }*/

      l_pFont->drop();
      l_pFace->drop();

      if (m_pDialog != nullptr)
        m_pDialog->createUi();
    }

    /**
    * Event handling method. The main class passes all Irrlicht events to this method
    */
    bool CLuaState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        m_bButtons[0] = a_cEvent.MouseInput.isLeftPressed  ();
        m_bButtons[1] = a_cEvent.MouseInput.isMiddlePressed();
        m_bButtons[2] = a_cEvent.MouseInput.isRightPressed ();
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
        case enMouseButton::RIght : return m_bButtons[2];
        default: return false;
      }
    }
  }
}