// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameClient.h>
#include <menu/IMenuHandler.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuNetworkLobby
    * @author Christian Keimel
    * In this menu the clients wait for the server to start a race
    */
    class CMenuNetworkLobby : public IMenuHandler {
      private:
        network::CGameClient *m_pClient;

        threads::CInputQueue *m_pQueue;

      public:
        CMenuNetworkLobby(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pClient    (a_pState->getGlobal()->getGameClient()),
          m_pQueue     (new threads::CInputQueue())
        {
          if (m_pClient != nullptr)
            m_pClient->getOutputQueue()->addListener(m_pQueue);

          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_lobby.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr)
            p->setText(L"MarbleGP Network Lobby");

          p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_message", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr)
            p->setText(L"Waiting for the server to select a race.");

          m_pManager->pushToMenuStack("menu_raceresult");
        }

        virtual ~CMenuNetworkLobby() {
          if (m_pClient != nullptr)
            m_pClient->getOutputQueue()->removeListener(m_pQueue);

          if (m_pQueue != nullptr)
            delete m_pQueue;
        }

        virtual bool run() override {
          if (m_pClient != nullptr) {
            messages::IMessage *l_pMsg = m_pQueue->popMessage();

            if (l_pMsg != nullptr) {
              printf("Got message %i\n", (int)l_pMsg->getMessageId());
              if (l_pMsg->getMessageId() == messages::enMessageIDs::UpdateRaceInfo) {
                messages::CUpdateRaceInfo *l_pUpdate = reinterpret_cast<messages::CUpdateRaceInfo *>(l_pMsg);

                printf("Got race information: \"%s\", \"%s\"\n", l_pUpdate->gettrack().c_str(), l_pUpdate->getinfo().c_str());

                irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_details", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                if (p != nullptr) {
                  p->setText(helpers::s2ws(l_pUpdate->getinfo()).c_str());
                }

                p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_race", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                if (p != nullptr) {
                  std::map<std::string, std::string> l_mTrackNames = helpers::getTrackNameMap();
                  std::string l_sTrack = l_pUpdate->gettrack();
                  if (l_mTrackNames.find(l_sTrack) != l_mTrackNames.end())
                    l_sTrack = l_mTrackNames[l_sTrack];

                  p->setText(helpers::s2ws(l_sTrack).c_str());
                }

                irr::gui::IGUIImage *l_pThumbnail = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("Thumbnail", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
                if (l_pThumbnail != nullptr) {
                  std::string l_sImg = "data/levels/" + l_pUpdate->gettrack() + "/thumbnail.png";
                  if (m_pFs->existFile(l_sImg.c_str()))
                    l_pThumbnail->setImage(m_pDrv->getTexture(l_sImg.c_str()));
                  else
                    l_pThumbnail->setImage(m_pDrv->getTexture("data/images/no_image.png"));
                }
              }
            }
          }
          return false;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                if (m_pClient != nullptr) {
                  m_pState->getGlobal()->stopGameClient();
                }

                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }


          return l_bRet;
        }
      };

    IMenuHandler *createMenuNetLobby(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNetworkLobby(a_pDevice, a_pManager, a_pState);
    }
}
}