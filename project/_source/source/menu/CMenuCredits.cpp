// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <gui/CGuiTextField.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuMessage
    * @author Christian Keimel
    * This menu displays the credits for the libraries in use
    */
    class CMenuCredits : public IMenuHandler {
      private:
        std::map<std::string, irr::gui::IGUIElement *> m_mElements;

      public:
        CMenuCredits(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_credits.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_mElements["headline" ] = helpers::findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT                   , m_pGui->getRootGUIElement());
          m_mElements["link"     ] = helpers::findElementByNameAndType("label_link"    , irr::gui::EGUIET_STATIC_TEXT                   , m_pGui->getRootGUIElement());
          m_mElements["main"     ] = helpers::findElementByNameAndType("textfield"     , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_TextFieldId, m_pGui->getRootGUIElement());
        }

        virtual ~CMenuCredits() {
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
              else {
                if (l_sSender.substr(0, 8) == "license_") {
                  std::string l_sBase = "data/doc/" + l_sSender;

                  std::vector<std::tuple<std::string, std::string>> l_mAppendices;
                  
                  l_mAppendices.push_back(std::make_tuple(".txt"         , "main"    ));
                  l_mAppendices.push_back(std::make_tuple("_headline.txt", "headline"));
                  l_mAppendices.push_back(std::make_tuple("_link.txt"    , "link"    ));

                  for (std::vector<std::tuple<std::string, std::string>>::iterator it = l_mAppendices.begin(); it != l_mAppendices.end(); it++) {
                    if (m_mElements.find(std::get<1>(*it)) != m_mElements.end() && m_mElements[std::get<1>(*it)] != nullptr) {
                      irr::gui::IGUIElement *l_pElement = m_mElements[std::get<1>(*it)];

                      std::string l_sFile = l_sBase + std::get<0>(*it);

                      if (m_pFs->existFile(l_sFile.c_str())) {
                        irr::io::IReadFile *f = m_pFs->createAndOpenFile(l_sFile.c_str());

                        if (f != nullptr) {
                          char *p = new char[f->getSize() + 1];
                          memset(p, 0, f->getSize() + 1);

                          f->read(p, f->getSize());

                          if (l_pElement->getType() == irr::gui::EGUIET_STATIC_TEXT) {
                            (reinterpret_cast<irr::gui::IGUIStaticText *>(l_pElement))->setText(helpers::s2ws(p).c_str());
                          }
                          else if (l_pElement->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_TextFieldId) {
                            (reinterpret_cast<gui::CGuiTextField *>(l_pElement))->setNewText(p);
                          }

                          delete[] p;
                          f->drop();
                        }
                      }
                    }
                  }
                }
                else printf("Button %s clicked.\n", l_sSender.c_str());
              }
            }
          }


          return l_bRet;
        }
    };

    IMenuHandler *createMenuCredits(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuCredits(a_pDevice, a_pManager, a_pState);
    }
  }
}