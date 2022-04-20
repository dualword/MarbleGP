// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <menu/IMenuManager.h>
#include <data/CDataStructs.h>
#include <gui/CControllerUi.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <Defines.h>
#include <CGlobal.h>
#include <map>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSettings
    * @author Christian Keimel
    * The handler for the settings menu
    */
    class CMenuSettings : public IMenuHandler {
      private:
        gui::CSelector       *m_pResolution,
                             *m_pSplitScreen;
        gui::CMenuBackground *m_pGfxG,
                             *m_pGfxM,
                             *m_pSfx,
                             *m_pMisc;
        gui::CControllerUi   *m_pController;

        std::map<std::string, bool[4]> m_mPageVisibility;

        std::vector<gui::CMenuBackground *> m_vPages;

        int m_iSplitIdx;

        data::SSettings m_cSettings;

        void updateSoundSetting(float& a_fSetting, int a_iLabelId, irr::gui::IGUIScrollBar *a_pScrollbar) {
          if (a_pScrollbar != nullptr) {
            a_fSetting = (float)a_pScrollbar->getPos() / 1000.0f; 
            irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByIdAndType(a_iLabelId, irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

            if (l_pLabel != nullptr) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%.0f%%", 100.0 * a_fSetting);
              l_pLabel->setText(s);
            }
          }
        }

        void updateSoundUI(int a_iScrollbarId, int a_iLabelId, float a_fValue) {
          irr::gui::IGUIScrollBar  *l_pScroll = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByIdAndType(a_iScrollbarId, irr::gui::EGUIET_SCROLL_BAR , m_pGui->getRootGUIElement()));
          irr::gui::IGUIStaticText *l_pLabel  = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByIdAndType(a_iLabelId    , irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pScroll != nullptr) {
            l_pScroll->setPos((irr::s32)(1000.0f * a_fValue));
            if (l_pLabel != nullptr) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%.0f%%", 100.0 * a_fValue);
              l_pLabel->setText(s);
            }
          }
        }

        void updateSelectorSettings(int &a_iSetting, gui::CSelector *a_pSelector) { if (a_pSelector != nullptr) a_iSetting = a_pSelector->getSelected(); else printf("Selector is null.\n"); }

        void updateSelectorUI(int a_iSetting, gui::CSelector *a_pSelector) { if (a_pSelector != nullptr) a_pSelector->setSelected(a_iSetting); else printf("Selector is null.\n"); }

        void updateCheckboxSettings(bool &a_bSetting, gui::CDustbinCheckbox *a_pCheckbox) { if (a_pCheckbox != nullptr) a_bSetting = a_pCheckbox->isChecked(); else printf("Checkbox is null.\n"); }

        void updateCheckboxUI(bool a_bSetting, gui::CDustbinCheckbox* a_pCheckbox) { if (a_pCheckbox != nullptr) a_pCheckbox->setChecked(a_bSetting); else printf("Checkbox is null.\n"); }

        void updateSplitscreenUI() {
          int l_iIndex = 0;

          if (m_pSplitScreen != nullptr) {
            l_iIndex = m_pSplitScreen->getSelected();
          }

          if (l_iIndex >= 0 && l_iIndex < 8) {
            irr::gui::EGUI_ELEMENT_TYPE l_eType = (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId;

            updateCheckboxUI(m_cSettings.m_aGameGFX[l_iIndex].m_bHightlight  , reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23014, l_eType, m_pGui->getRootGUIElement())));
            updateCheckboxUI(m_cSettings.m_aGameGFX[l_iIndex].m_bShowControls, reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23015, l_eType, m_pGui->getRootGUIElement())));
            updateCheckboxUI(m_cSettings.m_aGameGFX[l_iIndex].m_bShowRanking , reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23016, l_eType, m_pGui->getRootGUIElement())));

            m_iSplitIdx = l_iIndex;
          }
        }

        void updateSplitscreenSettings() {
          int l_iIndex = 0;

          if (m_pSplitScreen != nullptr) {
            l_iIndex = m_pSplitScreen->getSelected();
          }

          irr::gui::EGUI_ELEMENT_TYPE l_eType = (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId;

          for (int i = 7; i >= l_iIndex; i--) {
            if (m_cSettings.m_aGameGFX[i].m_bHightlight == m_cSettings.m_aGameGFX[l_iIndex].m_bHightlight)
              updateCheckboxSettings(m_cSettings.m_aGameGFX[i].m_bHightlight, reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23014, l_eType, m_pGui->getRootGUIElement())));

            if (m_cSettings.m_aGameGFX[i].m_bShowControls == m_cSettings.m_aGameGFX[l_iIndex].m_bShowControls)
              updateCheckboxSettings(m_cSettings.m_aGameGFX[i].m_bShowControls, reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23015, l_eType, m_pGui->getRootGUIElement())));

            if (m_cSettings.m_aGameGFX[i].m_bShowRanking == m_cSettings.m_aGameGFX[l_iIndex].m_bShowRanking)
              updateCheckboxSettings(m_cSettings.m_aGameGFX[i].m_bShowRanking, reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23016, l_eType, m_pGui->getRootGUIElement())));
          }
        }

      public:
        CMenuSettings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_pResolution (nullptr),
          m_pSplitScreen(nullptr),
          m_pController (nullptr),
          m_iSplitIdx   (-1)
        {
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_settings.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_cSettings.copyFrom(m_pState->getGlobal()->getSettingData());

          m_pResolution  = reinterpret_cast<gui::CSelector *>(findElementByIdAndType(23010, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          m_pSplitScreen = reinterpret_cast<gui::CSelector *>(findElementByIdAndType(23042, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

          m_pGfxG = reinterpret_cast<gui::CMenuBackground *>(findElementByIdAndType(23050, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
          m_pGfxM = reinterpret_cast<gui::CMenuBackground *>(findElementByIdAndType(23051, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
          m_pSfx  = reinterpret_cast<gui::CMenuBackground *>(findElementByIdAndType(23052, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
          m_pMisc = reinterpret_cast<gui::CMenuBackground *>(findElementByIdAndType(23053, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));

          std::string l_sCtrl = m_cSettings.m_sController;

          if (l_sCtrl == "") {
            controller::CControllerMenu *p = new controller::CControllerMenu(-1);
            l_sCtrl = p->serialize();
            delete p;
          }

          m_pController = reinterpret_cast<gui::CControllerUi *>(findElementByNameAndType("controller_ui", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiId, m_pGui->getRootGUIElement()));

          if (m_pController != nullptr) {
            m_pController->setFont(m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize()));
            m_pController->setText(helpers::s2ws(l_sCtrl).c_str());
            m_pController->setMenuManager(m_pManager);
          }

          // Fill the video resolution list
          if (m_pResolution != nullptr) {
            irr::core::dimension2du l_cScreenSize = m_pDrv->getScreenSize();

            irr::video::IVideoModeList *l_pList = m_pDevice->getVideoModeList();

            std::wstring l_sLast = L"";
            for (irr::s32 i = 0; i < l_pList->getVideoModeCount(); i++) {

              std::wstring s = std::to_wstring(l_pList->getVideoModeResolution(i).Width) + L"x" + std::to_wstring(l_pList->getVideoModeResolution(i).Height);
              if (s != l_sLast) {
                l_sLast = s;
                m_pResolution->addItem(l_sLast);

                if (l_pList->getVideoModeResolution(i) == l_cScreenSize)
                  m_pResolution->setSelected(i);
              }
            }
          }

          updateCheckboxUI(m_cSettings.m_bFullscreen , reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23011, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement())));
          updateCheckboxUI(m_cSettings.m_bUseMenuCtrl, reinterpret_cast<gui::CDustbinCheckbox *>(findElementByIdAndType(23026, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement())));

          updateSplitscreenUI();

          updateSelectorUI(m_cSettings.m_iShadows, reinterpret_cast<gui::CSelector *>(findElementByIdAndType(23012, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement())));
          updateSelectorUI(m_cSettings.m_iAmbient, reinterpret_cast<gui::CSelector *>(findElementByIdAndType(23013, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement())));

          updateSoundUI(23018, 30000, m_cSettings.m_fSfxMaster );
          updateSoundUI(23020, 30001, m_cSettings.m_fSoundTrack);
          updateSoundUI(23022, 30002, m_cSettings.m_fSfxMenu   );
          updateSoundUI(23024, 30003, m_cSettings.m_fSfxGame   );

          updateSplitscreenUI();

          m_pState->setZLayer(1);
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (m_pController != nullptr)
            m_pController->update(a_cEvent);

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "gfx") {
                m_pState->setZLayer(1);

                if (m_pGfxG != nullptr) m_pGfxG->setVisible(true);
                if (m_pGfxM != nullptr) m_pGfxM->setVisible(true);
                if (m_pSfx  != nullptr) m_pSfx ->setVisible(false);
                if (m_pMisc != nullptr) m_pMisc->setVisible(false);

                l_bRet = true;
              }
              else if (l_sSender == "sfx") {
                m_pState->setZLayer(2);

                if (m_pGfxG != nullptr) m_pGfxG->setVisible(false);
                if (m_pGfxM != nullptr) m_pGfxM->setVisible(false);
                if (m_pSfx  != nullptr) m_pSfx ->setVisible(true);
                if (m_pMisc != nullptr) m_pMisc->setVisible(false);

                l_bRet = true;
              }
              else if (l_sSender == "misc") {
                m_pState->setZLayer(4);

                if (m_pGfxG != nullptr) m_pGfxG->setVisible(false);
                if (m_pGfxM != nullptr) m_pGfxM->setVisible(false);
                if (m_pSfx  != nullptr) m_pSfx ->setVisible(false);
                if (m_pMisc != nullptr) m_pMisc->setVisible(true);

                l_bRet = true;
              }
              else if (l_sSender == "ok") {
                if (m_pController != nullptr)
                  m_cSettings.m_sController = m_pController->serialize();

                m_pState->getGlobal()->getSettingData().copyFrom(m_cSettings);

                irr::SEvent l_cEvent;
                l_cEvent.EventType = irr::EET_USER_EVENT;
                l_cEvent.UserEvent.UserData1 = c_iEventSettingsChanged;
                CGlobal::getInstance()->getIrrlichtDevice()->postEventFromUser(l_cEvent);

                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
              else if (l_sSender == "cancel") {
                m_pState->getGlobal()->getSoundInterface()->setMasterVolume    (m_pState->getGlobal()->getSettingData().m_fSfxMaster );
                m_pState->getGlobal()->getSoundInterface()->setSoundtrackVolume(m_pState->getGlobal()->getSettingData().m_fSoundTrack);
                m_pState->getGlobal()->getSoundInterface()->setSfxVolumeMenu   (m_pState->getGlobal()->getSettingData().m_fSfxMenu   );
                m_pState->getGlobal()->getSoundInterface()->setSfxVolumeGame   (m_pState->getGlobal()->getSettingData().m_fSfxGame   );

                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
              else printf("Button clicked (%s, %i, CMenuMain).\n", l_sSender.c_str(), a_cEvent.GUIEvent.Caller->getID());
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
              if (l_sSender == "selector_splitscreen") {
                updateSplitscreenUI();
                l_bRet = true;
              }
              else if ((l_sSender == "Shadows" || l_sSender == "Ambient") && a_cEvent.GUIEvent.Caller->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId) {
                int *a_pSetting = nullptr;

                if (a_cEvent.GUIEvent.Caller->getID() == 23012)
                  a_pSetting = &m_cSettings.m_iShadows;
                else if (a_cEvent.GUIEvent.Caller->getID() == 23013)
                  a_pSetting = &m_cSettings.m_iAmbient;

                if (a_pSetting != nullptr)
                  updateSelectorSettings(*a_pSetting, reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller));
              }
              else if (l_sSender == "Resolution") {
                if (m_pResolution != nullptr) {
                  std::string s = helpers::ws2s(m_pResolution->getItem(m_pResolution->getSelected()));
                  std::vector<std::string> l_vArray = helpers::splitString(s, 'x');

                  if (l_vArray.size() == 2) {
                    m_cSettings.m_iResolutionW = std::atoi(l_vArray[0].c_str());
                    m_cSettings.m_iResolutionH = std::stoi(l_vArray[1].c_str());
                  }
                }
              }
              else {
                irr::s32 l_iId = a_cEvent.GUIEvent.Caller->getID();

                if ((l_iId == 23018 || l_iId == 23020 || l_iId == 23022 || l_iId == 23024) && a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_SCROLL_BAR) {
                  int    l_iLabelId = -1;
                  float *l_fSetting = nullptr;

                  irr::gui::IGUIScrollBar *p = reinterpret_cast<irr::gui::IGUIScrollBar *>(a_cEvent.GUIEvent.Caller);

                  irr::f32 l_fNewValue = ((irr::f32)p->getPos()) / 1000.0f;

                  switch (l_iId) {
                    case 23018: l_iLabelId = 30000; l_fSetting = &m_cSettings.m_fSfxMaster ; CGlobal::getInstance()->getSoundInterface()->setMasterVolume    (l_fNewValue); break;
                    case 23020: l_iLabelId = 30001; l_fSetting = &m_cSettings.m_fSoundTrack; CGlobal::getInstance()->getSoundInterface()->setSoundtrackVolume(l_fNewValue); break;
                    case 23022: l_iLabelId = 30002; l_fSetting = &m_cSettings.m_fSfxMenu   ; CGlobal::getInstance()->getSoundInterface()->setSfxVolumeMenu   (l_fNewValue); break;
                    case 23024: l_iLabelId = 30003; l_fSetting = &m_cSettings.m_fSfxGame   ; CGlobal::getInstance()->getSoundInterface()->setSfxVolumeGame   (l_fNewValue); break;
                  }

                  if (l_fSetting != nullptr) {
                    updateSoundSetting(*l_fSetting, l_iLabelId, p);
                  }
                }
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
              if (a_cEvent.GUIEvent.Caller->getID() == 23011) {
                m_cSettings.m_bFullscreen = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked();
              }
              else if (a_cEvent.GUIEvent.Caller->getID() == 23026) {
                m_cSettings.m_bUseMenuCtrl = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked();
              }
              else if (a_cEvent.GUIEvent.Caller->getID() == 23014 || 
                       a_cEvent.GUIEvent.Caller->getID() == 23015 ||
                       a_cEvent.GUIEvent.Caller->getID() == 23016) 
              {
                updateSplitscreenSettings();
              }
            }
          }

          return l_bRet;
        }
    };

    IMenuHandler *createMenuSettings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSettings(a_pDevice, a_pManager, a_pState);
    }
  }
}