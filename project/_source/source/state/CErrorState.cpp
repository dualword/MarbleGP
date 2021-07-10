// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CMenuBackground.h>
#include <platform/CPlatform.h>
#include <state/CErrorState.h>
#include <gui/CMenuButton.h>
#include <CGlobal.h>

namespace dustbin {
  namespace state {
    CErrorState::CErrorState() : m_bBackToLua(false) {

    }

    CErrorState::~CErrorState() {

    }

    /**
      * This method is called when the state is activated
      */
    void CErrorState::activate() {
      createUi();
      m_bBackToLua = false;
    }

    /**
    * This method is called when the state is deactivated
    */
    void CErrorState::deactivate() {
      CGlobal::getInstance()->getGuiEnvironment()->clear();
      CGlobal::getInstance()->getSceneManager()->clear();
    }

    /**
      * This is a callback method that gets invoked when the window is resized
      * @param a_cDim the new dimension of the window
      */
    void CErrorState::onResize(const irr::core::dimension2du& a_cDim) {
      createUi();
    }

    /**
    * This method is called before the UI is cleared on window resize. It can be
    * used to save all necessary data to re-build the UI
    */
    void CErrorState::beforeResize() {
      // Nothing to do here
    }

    /**
      * Return the state's ID
      */
    enState CErrorState::getId() {
      return enState::ErrorState;
    }

    /**
      * Get the state of the mouse buttons. As the cursor control Irrlicht Object does not
      * report the state of the button I decided to hack it this way
      * @param a_iButton The mouse button
      */
    bool CErrorState::isMouseDown(enMouseButton a_iButton) {
      return false;
    }

    /**
      * Event handling method. The main class passes all Irrlicht events to this method
      */
    bool CErrorState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          m_bBackToLua = true;
        }
      }

      return l_bRet;
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CErrorState::run() {
      return m_bBackToLua ? enState::LuaState : enState::None;
    }

    void CErrorState::createUi() {
      CGlobal* l_pGlobal = CGlobal::getInstance();

      l_pGlobal->getSceneManager()->loadScene("data/menu3d/skybox.xml");
      irr::gui::IGUIEnvironment* l_pGui = l_pGlobal->getGuiEnvironment();
      l_pGui->clear();

      irr::gui::IGUIElement* l_pBack = l_pGui->addGUIElement("MenuBackground");
      l_pBack->setRelativePosition(l_pGlobal->getRect(-40, -25, 40, 25, enLayout::Center));

      irr::gui::IGUIStaticText* p = l_pGui->addStaticText(L"Oops .. an error occured.", l_pGlobal->getRect(-39, -23, 39, -18, enLayout::Center));
      p->setOverrideFont(l_pGlobal->getFont(enFont::Huge, l_pGlobal->getVideoDriver()->getScreenSize()));
      p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);

      p = l_pGui->addStaticText(platform::s2ws(l_pGlobal->getGlobal("ERROR_HEAD")).c_str(), l_pGlobal->getRect(-39, -16, 39, -10, enLayout::Center));
      p->setOverrideFont(l_pGlobal->getFont(enFont::Big, l_pGlobal->getVideoDriver()->getScreenSize()));
      p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);

      l_pGui->addStaticText(platform::s2ws(l_pGlobal->getGlobal("ERROR_MESSAGE")).c_str(), l_pGlobal->getRect(-20, -10, 20, 20, enLayout::Center));

      gui::CMenuButton* l_pBtn = new gui::CMenuButton(l_pGui->getRootGUIElement());
      irr::io::IAttributes* l_pAttr = l_pGlobal->getFileSystem()->createEmptyAttributes();
      l_pBtn->serializeAttributes(l_pAttr);
      l_pAttr->setAttribute("ImagePath", "data/images/btn_ok.png");
      l_pBtn->deserializeAttributes(l_pAttr);
      l_pAttr->drop();

      l_pBtn->setRelativePosition(l_pGlobal->getRect(1, -4, 5, 0, enLayout::LowerMiddle));
    }
  } // namespace state
} // namespace dustbin