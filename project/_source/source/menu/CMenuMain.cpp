// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CMenuLoader.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuMain
    * @author Christian Keimel
    * The handler for the main menu
    */
    class CMenuMain : public IMenuHandler {
      public:
        CMenuMain(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_main.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pManager->clearMenuStack();

          m_pState->setZLayer(0);
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

            if (l_sButton == "settings") {
              createMenu("menu_settings", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "profiles") {
              createMenu("menu_profiles", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "free_racing") {
              std::vector<data::SPlayerData> l_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));
              m_pManager->pushToMenuStack("menu_selecttrack");

              if (l_vProfiles.size() == 0) {
                m_pManager->pushToMenuStack("menu_setupgame");
                m_pManager->pushToMenuStack("menu_profiles");
                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
              }
              else createMenu("menu_setupgame", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "view_track") {
              data::SRacePlayers l_cPlayers;

              for (int i = 0; i < 16; i++) {
                data::SPlayerData l_cData;
                l_cData.m_eType     = data::enPlayerType::Ai;
                l_cData.m_iGridPos  = i;
                l_cData.m_iPlayerId = i + 1;
                l_cData.m_sName     = "AI Demo Player #" + std::to_string(i + 1);
                l_cData.m_sControls = "ai_player";
                l_cData.m_eAiHelp   = data::SPlayerData::enAiHelp::Off;
                l_cData.m_sTexture  = "default://number=" + std::to_string(i + 1);

                l_cPlayers.m_vPlayers.push_back(l_cData);
              }

              m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "exit") {
              m_pState->setState(state::enState::Quit);
            }
            else printf("Button clicked (%s, %i, CMenuMain).\n", l_sButton.c_str(), a_cEvent.GUIEvent.Caller->getID());
          }

          return l_bRet;
        }
    };



    IMenuHandler* createMenuMain(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuMain(a_pDevice, a_pManager, a_pState);
    }
  }
}