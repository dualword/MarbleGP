// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <gameclasses/SPlayer.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
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

                std::string l_sName  = "";
                std::string l_sShort = "";

                helpers::createRandomProfile(l_sName, l_sShort);

                gameclasses::STournament *l_pTournament = m_pState->getGlobal()->startTournament();

                gameclasses::SPlayer* l_pPlayer = new gameclasses::SPlayer(
                  1,
                  1,
                  l_sName,
                  helpers::createRandomTexture(),
                  helpers::getDefaultGameCtrl_Keyboard(),
                  l_sShort,
                  data::SPlayerData::enAiHelp::High,
                  nullptr,
                  data::enPlayerType::Local
                );

                l_pTournament->m_vPlayers.push_back(l_pPlayer);
                
                data::SPlayerData l_cPlayer;
                l_cPlayer.m_sName      = l_pPlayer->m_sName;
                l_cPlayer.m_sShortName = l_pPlayer->m_sShortName;
                l_cPlayer.m_sTexture   = l_pPlayer->m_sTexture;
                l_cPlayer.m_eAiHelp    = l_pPlayer->m_eAiHelp;
                l_cPlayer.m_sControls  = l_pPlayer->m_sController;
                l_cPlayer.m_eType      = l_pPlayer->m_eType;
                
                std::vector<data::SPlayerData> l_vProfiles = {
                  l_cPlayer
                };

                helpers::saveProfiles(l_vProfiles);

                gameclasses::SRace* l_pRace = new gameclasses::SRace(
                  "tutorial",
                  "Tutorial Track",
                  "This track will show you all you need to know to play MarbleGP",
                  1,
                  l_pTournament
                );
                l_pTournament->m_vRaces.push_back(l_pRace);
                l_pTournament->startRace();

                l_cPlayer.m_iViewPort = 1;
                l_cPlayer.m_iGridPos  = 1;
                l_cPlayer.m_iPlayerId = 1;

                CGlobal::getInstance()->setSetting("selectedplayers", messages::urlEncode(l_cPlayer.m_sName));
                platform::saveSettings();

                m_pManager->pushToMenuStack("menu_selecttrack");
                m_pManager->pushToMenuStack("menu_newgamewizard");

                m_pState->setState(state::enState::Game);
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