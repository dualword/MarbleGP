// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuIntroduction
    * @author Christian Keimel
    * This menu is shown when the game is strted for the first time.
    * It creates a random profile (AI help High, Keyboard control on
    * Windows or Touch on android and starts the tutorial
    */
    class CMenuIntroduction : public IMenuHandler {
      public:
        CMenuIntroduction(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_intro.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          std::string l_sHeadline = m_pState->getGlobal()->getGlobal("message_headline");
          if (l_sHeadline != "") {
            irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(l_sHeadline).c_str());

            m_pState->getGlobal()->setGlobal("message_headline", "");
          }

          std::string l_sText = m_pState->getGlobal()->getGlobal("message_text");
          if (l_sText != "") {
            irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_message", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(l_sText).c_str());

            m_pState->getGlobal()->setGlobal("message_text", "");
          }
        }

        virtual ~CMenuIntroduction() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                l_bRet = true;



                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }


          return l_bRet;
        }
    };

    IMenuHandler *createMenuIntroduction(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuIntroduction(a_pDevice, a_pManager, a_pState);
    }
  }
}