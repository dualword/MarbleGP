// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <controller/CControllerBase.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CTextureHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiImageList.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <Defines.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace menu {

    /**
    * @class SPlayerProfileUI
    * @author Christian Keimel
    * This data structure holds references to all UI
    * elements for editing the player profiles
    */
    struct SPlayerProfileUI {
      gui::CMenuBackground *m_pRoot;  /**< The root element for the player profile dialogs */

      irr::gui::IGUITab *m_pDataRoot; /**< The tab that acts as root element for all edits */

      irr::gui::IGUIEditBox *m_pName;   /**< The player name edit box */
      irr::gui::IGUIEditBox *m_pShort;  /**< The player name abbreviation edit box */

      irr::gui::IGUIStaticText *m_pControl;     /**< The label for type of controls */
      irr::gui::IGUIStaticText *m_pTextureType; /**< The label for type of texture */
      irr::gui::IGUIStaticText *m_pLblAiHelp;   /**< The label for the AI help level */

      gui::CMenuButton *m_pDelete;      /**< The "remove profile" button */
      gui::CMenuButton *m_pControls;    /**< The "edit controls" button */
      gui::CMenuButton *m_pTexture;     /**< The "edit texture" button */
      gui::CMenuButton *m_pAddProfile;  /**< The "add profile" button */

      data::SPlayerData m_cData;   /**< The Player Data structure linked to this profile UI */

      SPlayerProfileUI() :
        m_pRoot       (nullptr),
        m_pDataRoot   (nullptr),
        m_pName       (nullptr),
        m_pShort      (nullptr),
        m_pControl    (nullptr),
        m_pTextureType(nullptr),
        m_pLblAiHelp  (nullptr),
        m_pDelete     (nullptr),
        m_pControls   (nullptr),
        m_pTexture    (nullptr),
        m_pAddProfile (nullptr)
      {
      }

      bool isValid() {
        return m_pAddProfile  != nullptr &&
               m_pControls    != nullptr &&
               m_pDataRoot    != nullptr &&
               m_pDelete      != nullptr &&
               m_pName        != nullptr &&
               m_pRoot        != nullptr &&
               m_pTexture     != nullptr &&
               m_pShort       != nullptr;
      }

      void fillUI() {
        if (isValid()) {
          m_pName ->setText(helpers::s2ws(m_cData.m_sName     ).c_str());
          m_pShort->setText(helpers::s2ws(m_cData.m_sShortName).c_str());

          m_pDataRoot->setVisible(true);
          m_pAddProfile->setVisible(false);

          controller::CControllerGame l_cCtrl;
          l_cCtrl.deserialize(m_cData.m_sControls);
          switch ((*l_cCtrl.getInputs().begin()).m_eType) {
            case controller::CControllerBase::enInputType::JoyAxis:
            case controller::CControllerBase::enInputType::JoyButton:
            case controller::CControllerBase::enInputType::JoyPov:
              if (m_pControl != nullptr) m_pControl->setText(L"Joystick");
              break;

            case controller::CControllerBase::enInputType::Key:
              if (m_pControl != nullptr) m_pControl->setText(L"Keyboard");
              break;
          }
          size_t l_iPos = m_cData.m_sTexture.find("://");

          if (l_iPos != std::string::npos && m_pTextureType != nullptr) {
            std::string l_sPrefix  = m_cData.m_sTexture.substr(0, l_iPos );
            if (l_sPrefix == "file" || l_sPrefix == "imported")
              m_pTextureType->setText(L"Imported");
            else if (l_sPrefix == "generate")
              m_pTextureType->setText(L"Generated");
            else
              m_pTextureType->setText(L"Default");
          }

          if (m_pLblAiHelp != nullptr)
            switch (m_cData.m_eAiHelp) {
              case data::SPlayerData::enAiHelp::Off    : m_pLblAiHelp->setText(L"Off"              ); break;
              case data::SPlayerData::enAiHelp::Display: m_pLblAiHelp->setText(L"Display"          ); break;
              case data::SPlayerData::enAiHelp::Low    : m_pLblAiHelp->setText(L"Low"              ); break;
              case data::SPlayerData::enAiHelp::Medium : m_pLblAiHelp->setText(L"Medium"           ); break;
              case data::SPlayerData::enAiHelp::High   : m_pLblAiHelp->setText(L"High"             ); break;
              case data::SPlayerData::enAiHelp::BotMgp : m_pLblAiHelp->setText(L"AI Bot (MarbleGP)"); break;
              case data::SPlayerData::enAiHelp::BotMb2 : m_pLblAiHelp->setText(L"AI Bot (Marble2)" ); break;
              case data::SPlayerData::enAiHelp::BotMb3 : m_pLblAiHelp->setText(L"AI Bot (Marble3)" ); break;
            }
        }
      }
    };


    /**
    * @class CMenuProfiles
    * @author Christian Keimel
    * The handler for the profiles menu
    */
    class CMenuProfiles : public IMenuHandler {
      private:
        SPlayerProfileUI  m_aProfiles[8];   /**< The UI array for modifying the profiles */

        irr::gui::IGUITab *m_pTextureDialog;  /**< The texture root element */
        irr::gui::IGUITab *m_pTextureTabs[2]; /**< Tabs for generated / imported textures */
        irr::gui::IGUITab *m_pColorDialog;    /**< The color selection dialog */
        irr::gui::IGUITab *m_pPatternDialog;  /**< The pattern selection dialog */
        irr::gui::IGUITab *m_pColorDisplay;   /**< The tab showing the color in the choose color dialog*/
        irr::gui::IGUITab *m_pControlDialog;  /**< the tab with the control configuration */
        irr::gui::IGUITab *m_pConfirmDialog;  /**< The error message dialog */
        irr::gui::IGUITab *m_pButtonTab;      /**< The tab with the "edit texture colors" buttons (Android) */

        gui::CMenuButton *m_pMore;    /**< The "more" button in the texture UI (Android) */

        gui::CGuiImageList *m_pPatternList;   /**< The list of texture patterns */

        gui::CSelector          *m_pTextureMode;      /**< The texture type selector (Default, Generated, Imported) */
        gui::CSelector          *m_pAiHelp;           /**< The "AI Help" selector in the user control configuration */
        irr::gui::IGUIImage     *m_pTextureImg;       /**< The texture GUI image showing the current texture */
        irr::gui::IGUIEditBox   *m_pTexturePattern;   /**< The edit field of the texture pattern */
        irr::gui::IGUIListBox   *m_pCustomTexture;    /**< The custom texture list */
        gui::CControllerUi_Game *m_pControllerUI;     /**< The UI for controller configuration */

        irr::scene::ISceneManager* m_pMySmgr;   /**< Alternative scene manager to draw the texture scene */
        irr::video::ITexture* m_pMyRtt;         /**< Render target for the texture scene */
        irr::scene::ISceneNode* m_pMarble;      /**< The marble scene node in the texture scene */

        int m_iMaxIndex;    /**< The highest found index of the profile edit tabs */
        int m_iEditing;     /**< The currently edited profile */
        int m_iPatternPage; /**< The currently active page of the texture pattern dialog */

        std::string m_sTextureEdit;   /**< The edited generate texture pattern */

        std::map<std::string, std::tuple<std::string, irr::gui::IGUITab*>> m_mButtonLinks;    /**< This map links the button names (key) with the edit boxes (value) */
        std::map<std::string, irr::gui::IGUIEditBox* > m_mGeneratedEd;    /**< Link of buttons to their edit fields */
        std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText* > m_mColorLink;      /**< Link of the color scrollbars to their edits */

        std::vector<irr::video::ITexture*> m_vPatterns;   /**< List of the available texture patterns */
        std::vector<gui::CReactiveLabel*> m_vColorPick;  /**< List of the predefined colors to pick */

        std::map<irr::u8, irr::SEvent> m_mJoyStates;    /**< Joystick states to make sure events are only passed when the joystick state changes */

        void updateTextureUI() {
          if (m_aProfiles[m_iEditing].m_cData.m_sTexture == "") {
            m_aProfiles[m_iEditing].m_cData.m_sTexture = "default://number=1";
          }

          if (m_pTextureMode != nullptr) {
            std::string l_sType = "default";

            std::map<std::string, std::string> l_mParamaters = helpers::parseParameters(l_sType, m_aProfiles[m_iEditing].m_cData.m_sTexture);

            if (l_sType == "default")
              m_pTextureMode->setSelected(0);
            else if (l_sType == "generate") {
              m_pTextureMode->setSelected(1);
              if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(true);
              if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(false);

              if (m_pMore != nullptr) m_pMore->setVisible(true);
            }
            else if (l_sType == "imported") {
              m_pTextureMode->setSelected(2);
              if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(false);
              if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(true);

              if (m_pMore != nullptr) m_pMore->setVisible(false);
            }
            else {
              if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(false);
              if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(false);

              if (m_pMore != nullptr) m_pMore->setVisible(false);
            }

            std::map<std::string, std::string> l_mParamMap{
              { "numbercolor" , "texture_fg_nb"       },
              { "numberback"  , "texture_bg_nb"       },
              { "ringcolor"   , "texture_nr"          },
              { "numberborder", "texture_nf"          },
              { "patterncolor", "texture_fg_pt"       },
              { "patternback" , "texture_bg_pt"       },
              { "pattern"     , "texture_pattern"     },
              { "nameback"    , "imported_name_color" },
              { "namecolor"   , "imported_name_back"  }
            };

            for (std::map<std::string, std::string>::iterator it = l_mParamaters.begin(); it != l_mParamaters.end(); it++) {
              std::string l_sKey = l_mParamMap.find(it->first) != l_mParamMap.end() ? l_mParamMap[it->first] : "";
              if (m_mGeneratedEd.find(l_sKey) != m_mGeneratedEd.end() && m_mGeneratedEd[l_sKey] != nullptr) {
                m_mGeneratedEd[l_sKey]->setText(helpers::s2ws(it->second).c_str());
              }
            }

            if (m_pTexturePattern != nullptr && l_mParamaters.find("pattern") != l_mParamaters.end())
              m_pTexturePattern->setText(helpers::s2ws(l_mParamaters["pattern"]).c_str());

            if (m_pCustomTexture != nullptr && l_mParamaters.find("file") != l_mParamaters.end())
              m_pCustomTexture->setSelected(helpers::s2ws(l_mParamaters["file"]).c_str());
          }
        }

        /**
        * Update the texture preview, either in 3d or 2d if the 3d view is not available
        * @param a_sTextureString a string defining the texture that will be generated
        */
        void updateTexture(const std::string &a_sTextureString) {
          if (m_pMyRtt != nullptr && m_pMarble != nullptr) {
            m_pMarble->getMaterial(0).setTexture(0, helpers::createTexture(a_sTextureString, m_pDrv, m_pFs));
          }
          else if (m_pTextureImg != nullptr) {
            if (m_aProfiles[m_iEditing].m_cData.m_sTexture == "") {
              m_aProfiles[m_iEditing].m_cData.m_sTexture = "default://number=1";
            }

            m_pTextureImg->setImage(helpers::createTexture(a_sTextureString, m_pDrv, m_pFs));
          }
        }

        /**
        * Update the pattern images starting with "m_iPatternPage"
        * @see m_iPatternPage
        */
        void updatePatterns() {
          if (m_pPatternList != nullptr) {
            irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/texture_patterns.xml");

            if (l_pXml) {
              std::vector<gui::CGuiImageList::SListImage> l_vPatterns;

              while (l_pXml->read()) {
                if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                  std::string l_sNode = l_pXml->getNodeName();

                  if (l_sNode == "pattern") {
                    std::string l_sPattern = l_pXml->getAttributeValueSafe("file");

                    if (l_sPattern != "") {
                      std::string l_sPath = "data/patterns/" + l_sPattern;

                      if (m_pFs->existFile(l_sPath.c_str())) {
                        l_vPatterns.push_back(gui::CGuiImageList::SListImage(l_sPath, l_sPattern, l_sPattern));
                      }
                    }
                  }
                }
              }

              m_pPatternList->setImageList(l_vPatterns);

              if (m_pTexturePattern != nullptr)
                m_pPatternList->setSelected(helpers::ws2s(m_pTexturePattern->getText()), false);
            }

            l_pXml->drop();
          }
        }

        /**
        * Fill the vector with the available default colors. All children will be checked as well
        * @param a_pElement the UI element to check, children will be iterated
        * @see m_vColorPick
        */
        void fillColorPickVector(irr::gui::IGUIElement *a_pElement) {
          if (a_pElement->getType() == gui::g_ReactiveLabelId) {
            std::string l_sName = a_pElement->getName();
            if (l_sName == "pick_color")
              m_vColorPick.push_back(reinterpret_cast<gui::CReactiveLabel *>(a_pElement));
          }

          for (irr::core::list<irr::gui::IGUIElement *>::ConstIterator it = a_pElement->getChildren().begin(); it != a_pElement->getChildren().end(); it++)
            fillColorPickVector(*it);
        }

        /**
        * Create the texture string depending on the settings
        */
        std::string createTextureString() {
          std::wstring s = L"default://";

          if (m_pTextureMode != nullptr) {
            if (m_pTextureMode->getSelected() == 1) {
              // Generated texture

              s = L"";

              // All parameters for the generated textures are stored
              // in Edit fields. This map uses the parameter name (key)
              // and maps them to the edit field name (value)
              std::map<std::wstring, std::string> l_mParameters{
                { L"numbercolor" , "texture_fg_nb"       },
                { L"numberback"  , "texture_bg_nb"       },
                { L"ringcolor"   , "texture_nr"          },
                { L"numberborder", "texture_nf"          },
                { L"patterncolor", "texture_fg_pt"       },
                { L"patternback" , "texture_bg_pt"       },
                { L"pattern"     , "texture_pattern"     }
              };

              for (std::map<std::wstring, std::string>::iterator it = l_mParameters.begin(); it != l_mParameters.end(); it++) {
                irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(it->second, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                if (p != nullptr) {
                  if (s == L"")
                    s = L"generate://";
                  else
                    s = s + L"&";

                  s += it->first + L"=" + p->getText();
                }
              }
            }
            else if (m_pTextureMode->getSelected() == 2) {
              // Imported texture
              irr::u32 l_iTexture = m_pCustomTexture->getSelected();
              if (l_iTexture >= m_pCustomTexture->getItemCount())
                l_iTexture = 0;

              if (l_iTexture < m_pCustomTexture->getItemCount())
                s = std::wstring(L"imported://file=") + m_pCustomTexture->getListItem(l_iTexture);

              std::map<std::wstring, std::string> l_mParameters{
                { L"patternback" , "imported_name_color" },
                { L"patterncolor", "imported_name_back"  }
              };

              for (std::map<std::wstring, std::string>::iterator it = l_mParameters.begin(); it != l_mParameters.end(); it++) {
                irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(it->second, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                if (p != nullptr) {
                  s += L"&" + it->first + L"=" + p->getText();
                }
              }
            }
          }

          return helpers::ws2s(s);
        }

        /**
        * Handle OK button click of the color selection dialog. Depending on which color is edited
        * the corresponding edit field will be updated, this edit field is in the next step used
        * to create the color string
        * @see m_sTextureEdit
        * @see m_mGeneratedEd
        * @see createTextureString
        * @see changeZLayer
        */
        void buttonColorOkClicked() {
          if (m_sTextureEdit != "" && m_mGeneratedEd.find(m_sTextureEdit) != m_mGeneratedEd.end() && m_mGeneratedEd[m_sTextureEdit] != nullptr) {
            bool l_bHandleNumberFrame = false;

            if (m_sTextureEdit == "texture_bg_nb" && m_mGeneratedEd.find("texture_nf") != m_mGeneratedEd.end()) {
              std::wstring l_sColor1 = m_mGeneratedEd["texture_bg_nb"]->getText(),
                            l_sColor2 = m_mGeneratedEd["texture_nf"   ]->getText();
              
              l_bHandleNumberFrame = l_sColor1 == l_sColor2;
            }

            std::wstring l_sRed = L"", l_sGreen = L"", l_sBlue = L"";

            for (std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText*>::iterator it = m_mColorLink.begin(); it != m_mColorLink.end(); it++) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%0x", it->first->getPos());

              std::string l_sName = it->first->getName();

                    if (l_sName == "scrollbar_red"  ) { l_sRed   = s; while (l_sRed  .size() < 2) l_sRed   = L"0" + l_sRed  ; }
              else if (l_sName == "scrollbar_green") { l_sGreen = s; while (l_sGreen.size() < 2) l_sGreen = L"0" + l_sGreen; }
              else if (l_sName == "scrollbar_blue" ) { l_sBlue  = s; while (l_sBlue .size() < 2) l_sBlue  = L"0" + l_sBlue ; }
            }

            m_mGeneratedEd[m_sTextureEdit]->setText((l_sRed + l_sGreen + l_sBlue).c_str());

            if (l_bHandleNumberFrame) {
              m_mGeneratedEd["texture_nf"]->setText(m_mGeneratedEd["texture_bg_nb"]->getText());
            }

            updateTexture(createTextureString());
          }

          m_sTextureEdit = "";
          if (m_pColorDialog != nullptr)
            m_pColorDialog->setVisible(false);

          changeZLayer(10);
        }

        /**
        * Handle the "cancel" click of the color selection dialog. The dialog will be made
        * invisible and the Z-Layer is changed back to "10" (texture dialog)
        * @see m_pColorDialog
        * @see changeZLayer
        */
        void buttonColorCancelClicked() {
          if (m_pColorDialog != nullptr)
            m_pColorDialog->setVisible(false);

          changeZLayer(10);
        }

        /**
        * A texture selection dialog has been clicked (These are the images in the "select pattern" dialog)
        * @param a_cEvent the Irrlicht event to be handled
        * @return true if the event is a texture selection
        * @see m_pTexturePattern
        * @see m_pPatternDialog
        */
        bool buttonTexturePatternSelectClicked(const irr::SEvent &a_cEvent) {
          if (a_cEvent.EventType == irr::EET_USER_EVENT && a_cEvent.UserEvent.UserData1 == c_iEventImageSelected && a_cEvent.UserEvent.UserData2 == c_iEventImageSelected && m_pPatternList != nullptr) {
            std::string s = m_pPatternList->getSelectedData();

            if (s != "") {
              if (m_pTexturePattern != nullptr)
                m_pTexturePattern->setText(helpers::s2ws(s).c_str());

              m_pPatternDialog->setVisible(false);
              updateTexture(createTextureString());
              changeZLayer(10);

              return true;
            }
          }
          return false;
        }

        /**
        * The OK button of the texture creation dialog was clicked, the texture is updated,
        * the Z Layer is set back to "0" (root dialog)
        * @see m_aProfiles
        * @see m_pTextureDialog
        * @see m_iEditing
        * @see changeZLayer
        */
        void buttonTextureOkClicked() {
          m_pTextureDialog->setVisible(false);
          m_aProfiles[m_iEditing].m_cData.m_sTexture = createTextureString();
          m_aProfiles[m_iEditing].fillUI();
          m_iEditing = -1;
          changeZLayer(1);
        }

        /**
        * The texture dialog was cancelled. This makes the dialog
        * invisible and changes the Z-Layer back to "0" (root)
        * @see m_pTextureDialog
        * @see m_iEditing
        * @see changeZLayer
        */
        void buttonTextureCancelClicked() {
          m_pTextureDialog->setVisible(false);
          m_iEditing = -1;
          changeZLayer(1);
        }

        /**
        * The general OK button has been clicked. The profiles are serialized and stored
        * as string in the "profiles" setting, the last step loads the main menu
        * @see messages::CSerializer64
        */
        void buttonOkClicked() {
          bool l_bClose = true;

          if (m_pManager->peekMenuStack() != "menu_main") {
            std::wstring l_sErrors = L"";

            l_bClose = false;

            for (int i = 0; i < m_iMaxIndex; i++)
              if (m_aProfiles[i].m_cData.m_sName != "") {
                l_bClose = true;
                break;
              }

            if (!l_bClose && m_pConfirmDialog != nullptr) {
              irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("confirmDialog_text", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
              if (p != nullptr)
                l_sErrors += L"You must add at least one profile before you can start a game.";

              m_pConfirmDialog->setVisible(true);
            }
          }

          if (l_bClose) {
            messages::CSerializer64 l_cSerializer;

            l_cSerializer.addS32(c_iProfileHead);
            l_cSerializer.addString(c_sProfileHead);

            for (int i = 0; i <= m_iMaxIndex; i++) {
              if (m_aProfiles[i].m_cData.m_iPlayerId > 0 && m_aProfiles[i].m_cData.m_iPlayerId <= m_iMaxIndex + 1) {
                if (m_aProfiles[i].m_pDataRoot->isVisible()) {
                  printf("Save profile %i (\"%s\")...\n", i, m_aProfiles[i].m_cData.m_sName.c_str());
                  l_cSerializer.addS32(c_iProfileStart);
                  l_cSerializer.addString(m_aProfiles[i].m_cData.serialize());
                  l_cSerializer.addS32(c_iProfileEnd);
                }
              }
            }

            l_cSerializer.addS32(c_iAllProfileEnd);

            m_pState->getGlobal()->setSetting("profiles", l_cSerializer.getMessageAsString());
            m_pControlDialog = nullptr;
            createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
          }
        }

        /**
        * The general cancel button was clicked, nothing is saved
        * and the main menu is loaded
        */
        void buttonCancelClicked() {
          createMenu("menu_main", m_pDevice, m_pManager, m_pState);
        }

        /**
        * The pattern dialog is closed when the "close" button is clicked
        */
        void buttonPatternCloseClicked() {
          if (m_pPatternDialog != nullptr)
            m_pPatternDialog->setVisible(false);

          changeZLayer(10);
        }

        /**
        * The control dialog is updated with the settings of the
        * selected profile
        */
        void updateControlDialog() {
          if (m_pControllerUI != nullptr) {
            if (m_aProfiles[m_iEditing].isValid()) {
              std::string s = m_aProfiles[m_iEditing].m_cData.m_sControls;
              if (s != "")
                m_pControllerUI->setText(helpers::s2ws(s).c_str());
            }
          }

          if (m_pAiHelp != nullptr)
            m_pAiHelp->setSelected((int)m_aProfiles[m_iEditing].m_cData.m_eAiHelp);
        }

        /**
        * Close the control dialog and set the Z-Layer back to root (0)
        * @see changeZLayer
        */
        void buttonControlsCancelClicked() {
          if (m_pControlDialog != nullptr) {
            m_pControlDialog->setVisible(false);
            changeZLayer(1);
          }
        }

        /**
        * The ok button of the control dialog was clicked. This method saves the controller
        * settings, hides the controller dialog and sets the Z-Layer back to root (0)
        * @see changeZLayer
        * @see m_aProfiles
        */
        void buttonControlsOkClicked() {
          if (m_iEditing >= 0 && m_iEditing <= m_iMaxIndex && m_aProfiles[m_iEditing].isValid() && m_pControllerUI != nullptr) {
            m_aProfiles[m_iEditing].m_cData.m_sControls = m_pControllerUI->serialize();
            m_aProfiles[m_iEditing].fillUI();
          }

          if (m_pControlDialog != nullptr) {
            m_pControlDialog->setVisible(false);
            changeZLayer(1);
          }
          m_iEditing = -1;
        }

        /**
        * The "Oh, I see" button of the message dialog has been clicked
        */
        void buttonOhISeeClicked() {
          if (m_pColorDialog != nullptr) {
            m_pConfirmDialog->setVisible(false);
            changeZLayer(1);
          }
        }

        /**
        * Update the scrollbars, the edit fields and the preview tab of the color dialog
        * @param a_sColor color string (RRGGBB), must be of length 6
        * @see m_pColorDisplay
        * @see m_mColorLink
        */
        void updateColorDialog(const std::string &a_sColor) {
          char *p;

          irr::s32 l_iRed   = std::strtol(a_sColor.substr(0, 2).c_str(), &p, 16),
                    l_iGreen = std::strtol(a_sColor.substr(2, 2).c_str(), &p, 16),
                    l_iBlue  = std::strtol(a_sColor.substr(4, 2).c_str(), &p, 16);

          for (std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText*>::iterator it2 = m_mColorLink.begin(); it2 != m_mColorLink.end(); it2++) {
            std::string l_sName = it2->first->getName();

            if (l_sName == "scrollbar_red")
              it2->first->setPos(l_iRed);
            else if (l_sName == "scrollbar_green")
              it2->first->setPos(l_iGreen);
            else if (l_sName == "scrollbar_blue")
              it2->first->setPos(l_iBlue);

            if (m_mColorLink.find(it2->first) != m_mColorLink.end()) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%i", it2->first->getPos());
              it2->second->setText(s);
            }
          }

          if (m_pColorDisplay != nullptr)
            m_pColorDisplay->setBackgroundColor(irr::video::SColor(0xFF, l_iRed, l_iGreen, l_iBlue));
        }

        void switchTextureMode() {
          if (m_pTextureMode != nullptr) {
            switch (m_pTextureMode->getSelected()) {
              case 0:
                if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(false);
                if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(false);

                if (m_pMore != nullptr) m_pMore->setVisible(false);
                break;

              case 1:
                if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(true);
                if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(false);

                if (m_pMore != nullptr) m_pMore->setVisible(true);
                break;

              case 2:
                if (m_pTextureTabs[0] != nullptr) m_pTextureTabs[0]->setVisible(false);
                if (m_pTextureTabs[1] != nullptr) m_pTextureTabs[1]->setVisible(true);

                if (m_pMore != nullptr) m_pMore->setVisible(false);
                break;
            }
          }
          updateTexture(createTextureString());
        }

        public:
          CMenuProfiles(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) : 
            IMenuHandler(a_pDevice, a_pManager, a_pState), 
            m_pTextureDialog (nullptr),
            m_pColorDialog   (nullptr),
            m_pPatternDialog (nullptr),
            m_pColorDisplay  (nullptr),
            m_pControlDialog (nullptr),
            m_pConfirmDialog (nullptr),
            m_pButtonTab     (nullptr),
            m_pMore          (nullptr),
            m_pPatternList   (nullptr),
            m_pTextureMode   (nullptr),
            m_pAiHelp        (nullptr),
            m_pTextureImg    (nullptr),
            m_pTexturePattern(nullptr),
            m_pCustomTexture (nullptr),
            m_pControllerUI  (nullptr),
            m_pMySmgr        (nullptr),
            m_pMyRtt         (nullptr),
            m_pMarble        (nullptr),
            m_iMaxIndex      (-1), 
            m_iEditing       (-1),
            m_iPatternPage   (1),
            m_sTextureEdit   ("")
          {
            m_pState->getGlobal()->clearGui();

            helpers::loadMenuFromXML("data/menu/menu_profiles.xml", m_pGui->getRootGUIElement(), m_pGui);
            m_pSmgr->loadScene("data/scenes/skybox.xml");
            m_pSmgr->addCameraSceneNode();

            m_pMySmgr = m_pSmgr->createNewSceneManager(false);
            m_pMySmgr->loadScene("data/scenes/texture_scene.xml");
            irr::scene::ICameraSceneNode *l_pCam = m_pMySmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
            l_pCam->setAspectRatio(1.0f);

            m_pMarble = m_pMySmgr->getSceneNodeFromName("marble");

            m_pMyRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");

            irr::gui::IGUIElement *l_pRoot = m_pGui->getRootGUIElement();

            m_pTextureDialog  = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("texture_dialog"   , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pColorDialog    = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("color_dialog"     , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pTextureMode    = reinterpret_cast<     gui::CSelector          *>(findElementByNameAndType("texture_mode"     , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId        , l_pRoot));
            m_pAiHelp         = reinterpret_cast<     gui::CSelector          *>(findElementByNameAndType("ai_help"          , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId        , l_pRoot));
            m_pTextureImg     = reinterpret_cast<irr::gui::IGUIImage          *>(findElementByNameAndType("texture_image"    , irr::gui::EGUIET_IMAGE                                , l_pRoot));
            m_pTexturePattern = reinterpret_cast<irr::gui::IGUIEditBox        *>(findElementByNameAndType("texture_pattern"  , irr::gui::EGUIET_EDIT_BOX                             , l_pRoot));
            m_pCustomTexture  = reinterpret_cast<irr::gui::IGUIListBox        *>(findElementByNameAndType("imported_texture" , irr::gui::EGUIET_LIST_BOX                             , l_pRoot));
            m_pTextureTabs[0] = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("texture_generated", irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pTextureTabs[1] = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("texture_imported" , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pPatternDialog  = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("pattern_dialog"   , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pColorDisplay   = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("color_display"    , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pControlDialog  = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("controllerDialog" , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pConfirmDialog  = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("confirmDialog"    , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pButtonTab      = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("ButtonTab"        , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pMore           = reinterpret_cast<     gui::CMenuButton        *>(findElementByNameAndType("btn_more"         , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId      , l_pRoot));
            m_pControllerUI   = reinterpret_cast<     gui::CControllerUi_Game *>(findElementByNameAndType("controller_ui"    , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId, l_pRoot));
            m_pPatternList    = reinterpret_cast<     gui::CGuiImageList      *>(findElementByNameAndType("PatternList"      , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId       , l_pRoot));


            if (m_pTextureImg != nullptr && m_pMyRtt != nullptr)
              m_pTextureImg->setImage(m_pMyRtt);

            if (m_pControllerUI != nullptr) {
              controller::CControllerGame *l_pCtrl = new controller::CControllerGame();
              m_pControllerUI->setText(helpers::s2ws(l_pCtrl->serialize()).c_str());
              m_pControllerUI->setMenuManager(m_pManager);
              delete l_pCtrl;
            }

            std::vector<std::string> l_vKeys = {
              "texture_fg_nb",
              "texture_bg_nb",
              "texture_nr",
              "texture_nf",
              "texture_fg_pt",
              "texture_bg_pt",
              "imported_name_color",
              "imported_name_back"
            };

            for (std::vector<std::string>::iterator it = l_vKeys.begin(); it != l_vKeys.end(); it++) {
              m_mGeneratedEd[*it] = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(*it, irr::gui::EGUIET_EDIT_BOX, l_pRoot));
            }

            l_vKeys = { "_red", "_green", "_blue" };

            for (std::vector<std::string>::iterator it = l_vKeys.begin(); it != l_vKeys.end(); it++) {
              irr::gui::IGUIScrollBar  *l_pKey   = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(std::string("scrollbar") + *it, irr::gui::EGUIET_SCROLL_BAR , l_pRoot));
              irr::gui::IGUIStaticText *l_pValue = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType(std::string("value"    ) + *it, irr::gui::EGUIET_STATIC_TEXT, l_pRoot));

              m_mColorLink[l_pKey] = l_pValue;
            }

            // Link the buttons (key) to the edit fields (value)
            m_mButtonLinks = {
              { "btn_select_fg_nb"     , std::make_tuple("texture_fg_nb"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_bg_nb"     , std::make_tuple("texture_bg_nb"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_nr"        , std::make_tuple("texture_nr"         , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_nf"        , std::make_tuple("texture_nf"         , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_fg_pt"     , std::make_tuple("texture_fg_pt"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_bg_pt"     , std::make_tuple("texture_bg_pt"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_name_color", std::make_tuple("imported_name_color", reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_name_back" , std::make_tuple("imported_name_back" , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) }
            };

            for (int i = 0; i < 8; i++) {
              std::string s = "root_player" + std::to_string(i + 1);

              m_aProfiles[i].m_pRoot = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType(s, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, l_pRoot));

              if (m_aProfiles[i].m_pRoot != nullptr) {
                irr::gui::IGUIElement *l_pRoot = m_aProfiles[i].m_pRoot;

                m_aProfiles[i].m_pDataRoot    = reinterpret_cast<irr::gui::IGUITab        *>(findElementByNameAndType("tab"              , irr::gui::EGUIET_TAB                            , l_pRoot));
                m_aProfiles[i].m_pName        = reinterpret_cast<irr::gui::IGUIEditBox    *>(findElementByNameAndType("edit_name"        , irr::gui::EGUIET_EDIT_BOX                       , l_pRoot));
                m_aProfiles[i].m_pShort       = reinterpret_cast<irr::gui::IGUIEditBox    *>(findElementByNameAndType("edit_abbreviation", irr::gui::EGUIET_EDIT_BOX, l_pRoot));
                m_aProfiles[i].m_pControl     = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_controls"   , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pTextureType = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_texture"    , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pLblAiHelp   = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_aihelp"     , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pDelete      = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_delete"       , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pControls    = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_controls"     , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pTexture     = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_texture"      , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pAddProfile  = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_add"          , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));

                if (m_aProfiles[i].isValid()) {
                  m_aProfiles[i].m_pDataRoot  ->setVisible(false);
                  m_aProfiles[i].m_pAddProfile->setVisible(i == 0);
                  m_iMaxIndex = i;
                }
              }
            }

            std::string l_sProfiles = m_pState->getGlobal()->getSetting("profiles");

            if (l_sProfiles != "") {
              messages::CSerializer64 l_cSerializer = messages::CSerializer64(l_sProfiles.c_str());

              int l_iNum = 0;

              if (l_cSerializer.getS32() == c_iProfileHead) {
                if (l_cSerializer.getString() == c_sProfileHead) {
                  while (l_cSerializer.hasMoreMessages()) {
                    irr::s32 l_iNextTuple = l_cSerializer.getS32();

                    if (l_iNextTuple == c_iAllProfileEnd) {
                      break;
                    }
                    else if (l_iNextTuple == c_iProfileStart) {
                      if (l_iNum >= 0 && l_iNum <= m_iMaxIndex) {
                        m_aProfiles[l_iNum].m_cData.deserialize(l_cSerializer.getString());

                        if (m_aProfiles[l_iNum].isValid())
                          m_aProfiles[l_iNum].m_cData.m_iPlayerId = l_iNum + 1;

                        if (l_cSerializer.getS32() == c_iProfileEnd) {
                          m_aProfiles[l_iNum].fillUI();
                          l_iNum++;

                          if (l_iNum > m_iMaxIndex)
                            break;
                          else 
                            if (m_aProfiles[l_iNum].isValid()) {
                              m_aProfiles[l_iNum].m_pAddProfile->setVisible(true);
                            }
                        }
                        else {
                          printf("Invalid profile ending.\n");
                          break;
                        }
                      }
                    }
                    else {
                      printf("Invalid player profile header.\n");
                      break;
                    }
                  }
                }
                else printf("Invalid player profile identifier.\n");
              }
              else printf("Invalid header for player profiles.\n");
            }

            // We use an XML file to get a list of the available patterns as the
            // Irrlicht file system directory list does not work with archives.
            if (m_pFs->existFile("data/patterns/patterns.xml")) {
              irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/patterns/patterns.xml");
              if (l_pXml) {
                while (l_pXml->read()) {
                  if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                    std::string l_sNode = l_pXml->getNodeName();
                    if (l_sNode == "pattern") {
                      std::string s = l_pXml->getAttributeValueSafe("file");
                      if (s != "") {
                        irr::video::ITexture *l_pPattern = m_pDrv->getTexture((std::string("data/patterns/") + s).c_str());
                        if (l_pPattern != nullptr)
                          m_vPatterns.push_back(l_pPattern);
                      }
                    }
                  }
                }
                l_pXml->drop();
              }

              // Fill the custom texture list
              if (m_pCustomTexture != nullptr) {
                irr::io::path l_sDir = m_pFs->getWorkingDirectory();

                std::wstring l_sTexturePath = platform::portableGetTexturePath();

                if (l_sTexturePath != L"") {
                  m_pFs->changeWorkingDirectoryTo(helpers::ws2s(l_sTexturePath).c_str());
                  irr::io::IFileList *l_pList = m_pFs->createFileList();

                  if (l_pList != nullptr) {
                    for (irr::u32 i = 0; i < l_pList->getFileCount(); i++) {
                      std::string l_sName = l_pList->getFileName(i).c_str();
                      if (l_sName.find(".png") != std::string::npos) {
                        // m_pCustomTexture->addItem(helpers::s2ws(l_sName).c_str());
                      }
                    }

                    l_pList->drop();
                  }
                }


                m_pFs->changeWorkingDirectoryTo(l_sDir);
              }
            }

            fillColorPickVector(m_pGui->getRootGUIElement());

            printf("Loaded (%i).\n", m_iMaxIndex);

            m_pState->setZLayer(1);
          }

          virtual ~CMenuProfiles() {
            if (m_pMySmgr) m_pMySmgr->drop();
          }

          int joyevent = 0;

          virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
            // A guard to make sure joystick events are only handled once per change
            if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
              irr::u8 l_iIndex = a_cEvent.JoystickEvent.Joystick;
              bool    l_bSkip  = false;

              if (m_mJoyStates.find(l_iIndex) != m_mJoyStates.end()) {
                l_bSkip = m_mJoyStates[l_iIndex].JoystickEvent.ButtonStates == a_cEvent.JoystickEvent.ButtonStates &&
                          m_mJoyStates[l_iIndex].JoystickEvent.POV          == a_cEvent.JoystickEvent.POV;

                if (l_bSkip) {
                  for (int i = 0; i < a_cEvent.JoystickEvent.NUMBER_OF_AXES && l_bSkip; i++) {
                    int l_iOld = m_mJoyStates[l_iIndex].JoystickEvent.Axis[i];
                    int l_iNew = a_cEvent              .JoystickEvent.Axis[i];

                    if (abs(l_iOld) < 16000) l_iOld = 0;
                    if (abs(l_iNew) < 16000) l_iNew = 0;

                    l_bSkip &= l_iOld > 0 == l_iNew > 0 && l_iOld < 0 == l_iNew < 0;
                  }
                }
              }

              m_mJoyStates[l_iIndex] = irr::SEvent(a_cEvent);

              if (l_bSkip)
                return true;

              printf("Process joystick event (%i).\n", joyevent++);
            }

            bool l_bRet = false;

            if (m_pControlDialog != nullptr && m_pControllerUI != nullptr && m_pControlDialog->isVisible())
              l_bRet = m_pControllerUI->update(a_cEvent);

            if (!l_bRet) {
              if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
                std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

                if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED) {
                  for (int i = 0; i <= m_iMaxIndex; i++) {
                    if (m_aProfiles[i].m_pName == a_cEvent.GUIEvent.Caller) {
                      bool l_bUpdateShort = m_aProfiles[i].m_cData.m_sName.substr(0, 5) == m_aProfiles[i].m_cData.m_sShortName || m_aProfiles[i].m_cData.m_sShortName == "";

                      m_aProfiles[i].m_cData.m_sName = helpers::ws2s(m_aProfiles[i].m_pName->getText()).c_str();

                      if (l_bUpdateShort) {
                        m_aProfiles[i].m_cData.m_sShortName = m_aProfiles[i].m_cData.m_sName.substr(0, 5);
                        m_aProfiles[i].m_pShort->setText(helpers::s2ws(m_aProfiles[i].m_cData.m_sShortName).c_str());
                      }

                      break;
                    }
                    else if (m_aProfiles[i].m_pShort == a_cEvent.GUIEvent.Caller) {
                      m_aProfiles[i].m_cData.m_sShortName = helpers::ws2s(m_aProfiles[i].m_pShort->getText()).c_str();
                    }
                  }
                }
                else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                  if (l_sSender == "ok") {
                    if (m_pColorDialog != nullptr && m_pColorDialog->isVisible())
                      buttonColorOkClicked();
                    else if (m_pPatternDialog != nullptr && m_pPatternDialog->isVisible())
                      buttonPatternCloseClicked();
                    else if (m_pTextureDialog != nullptr && m_pTextureDialog->isVisible())
                      buttonTextureOkClicked();
                    else if (m_pConfirmDialog != nullptr && m_pConfirmDialog->isVisible())
                      buttonOhISeeClicked();
                    else if (m_pPatternDialog == nullptr || !m_pPatternDialog->isVisible())
                       buttonOkClicked();

                    l_bRet = true;
                  }
                  else if (l_sSender == "cancel") {
                    if (m_pColorDialog != nullptr && m_pColorDialog->isVisible())
                      buttonColorCancelClicked();
                    else if (m_pPatternDialog != nullptr && m_pPatternDialog->isVisible())
                      buttonPatternCloseClicked();
                    else if (m_pButtonTab != nullptr && m_pButtonTab->isVisible()) {
                      m_pButtonTab->setVisible(false);
                      changeZLayer(10);
                    }
                    else if (m_pTextureDialog != nullptr && m_pTextureDialog->isVisible())
                      buttonTextureCancelClicked();
                    else 
                      buttonCancelClicked();
                    l_bRet = true;
                  }
                  else if (l_sSender == "btn_texture") {
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].isValid() && m_aProfiles[i].m_pTexture == a_cEvent.GUIEvent.Caller && m_pTextureDialog != nullptr) {
                        m_iEditing = i;
                        m_pTextureDialog->setVisible(true);
                        updateTextureUI();
                        updateTexture(createTextureString());
                        changeZLayer(10);
                        l_bRet = true;
                        break;
                      }
                    }
                  }
                  else if (l_sSender == "btn_texture_cancel") {
                    buttonTextureCancelClicked();
                    l_bRet = true;
                  }
                  else if (l_sSender == "btn_texture_ok") {
                    buttonTextureOkClicked();
                    l_bRet = true;
                  }
                  else if (l_sSender == "btn_add") {
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].isValid() && m_aProfiles[i].m_pAddProfile == a_cEvent.GUIEvent.Caller) {
                        int l_iNum = 1;
                        std::wstring l_sName = L"Player " + std::to_wstring(l_iNum++);

                        for (int j = 0; j < i; j++) {
                          if (m_aProfiles[j].isValid() && l_sName == m_aProfiles[j].m_pName->getText()) {
                            l_sName = std::wstring(L"Player ") + std::to_wstring(l_iNum++);
                          }
                        }

                        m_aProfiles[i].m_pAddProfile->setVisible(false);
                        m_aProfiles[i].m_pDataRoot  ->setVisible(true);
                        m_aProfiles[i].m_pName      ->setText   (l_sName.c_str());

                        m_aProfiles[i].m_cData.m_iPlayerId = i + 1;
                        m_aProfiles[i].m_cData.m_sName = helpers::ws2s(l_sName);

                        if (i + 1 <= m_iMaxIndex && m_aProfiles[i + 1].isValid()) {
                          m_aProfiles[i + 1].m_pAddProfile->setVisible(true);
                          m_aProfiles[i + 1].m_pDataRoot  ->setVisible(false);
                        }
                      }
                    }
                  }
                  else if (l_sSender == "btn_delete") {
                    int l_iDeleted = -1;
                    bool l_bFirstEmpty = true;

                    // Search for the delete button that was clicked
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      // if found ..
                      if (m_aProfiles[i].m_pDelete == a_cEvent.GUIEvent.Caller && m_aProfiles[i].isValid()) {
                        l_iDeleted = i;
                        // .. we iterate all following profiles and copy
                        // the data of the next profile to the iterated
                        for (int j = i; j < m_iMaxIndex; j++) {
                          if (m_aProfiles[j].isValid() && m_aProfiles[j + 1].isValid()) {
                            m_aProfiles[j].m_cData.copyFrom(m_aProfiles[j + 1].m_cData);

                            // If the profile is used (m_iPlayerId set) the we fill the UI ...
                            if (m_aProfiles[j].m_cData.m_iPlayerId > 0)
                              m_aProfiles[j].fillUI();
                            else {
                              // .. Otherwise we hide the data ..
                              m_aProfiles[j].m_pDataRoot->setVisible(false);
                              if (m_aProfiles[j].m_cData.m_iPlayerId <= 0) {
                                // .. and if it's the first empty profile we show the add button
                                if (l_bFirstEmpty) {
                                  l_bFirstEmpty = false;
                                  m_aProfiles[j].m_pAddProfile->setVisible(true);
                                }
                                else m_aProfiles[j].m_pAddProfile->setVisible(false);
                              }
                            }
                          }
                        }

                        // One of m_iMaxIndex profiles has been deleted, we need 
                        // to set the last add button to visible if all profiles
                        // were used before the deletion
                        if (l_bFirstEmpty) {
                          if (m_aProfiles[m_iMaxIndex].isValid()) {
                            m_aProfiles[m_iMaxIndex].m_pAddProfile->setVisible(true);
                            m_aProfiles[m_iMaxIndex].m_pDataRoot->setVisible(false);
                            m_aProfiles[m_iMaxIndex].m_cData.m_iPlayerId = -1;
                            m_aProfiles[m_iMaxIndex].m_cData.m_sName     = "";
                            m_aProfiles[m_iMaxIndex].m_cData.m_sControls = "";
                            m_aProfiles[m_iMaxIndex].m_cData.m_sTexture  = "";
                          }
                        }
                        else m_aProfiles[m_iMaxIndex].m_pAddProfile->setVisible(false);
                      }
                    }

                    for (int i = 0; i <= m_iMaxIndex; i++)
                      if (m_aProfiles[i].m_cData.m_iPlayerId > 0)
                        m_aProfiles[i].m_cData.m_iPlayerId = i + 1;
                  }
                  else if (l_sSender == "btn_color_ok") {
                    buttonColorOkClicked();
                  }
                  else if (l_sSender == "btn_color_cancel") {
                    buttonColorCancelClicked();
                  }
                  else if (l_sSender == "btn_select_pattern") {
                    if (m_pPatternDialog != nullptr) {
                      m_pPatternList->setSelected("", true);
                      m_pPatternDialog->setVisible(true);
                      changeZLayer(46);
                      updatePatterns();
                    }

                    if (m_pButtonTab != nullptr)
                      m_pButtonTab->setVisible(false);
                  }
                  else if (l_sSender == "btn_pattern_close") {
                    buttonPatternCloseClicked();
                  }
                  else if (l_sSender  == "btn_pattern_left") {
                    m_iPatternPage -= 3;
                    updatePatterns();
                  }
                  else if (l_sSender == "btn_pattern_right") {
                    m_iPatternPage += 3;
                    updatePatterns();
                  }
                  else if (l_sSender == "btn_ohisee") {
                    buttonOhISeeClicked();
                  } 
                  else if (l_sSender == "btn_controls") {
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].isValid() && m_aProfiles[i].m_pControls == a_cEvent.GUIEvent.Caller && m_pControlDialog != nullptr) {
                        m_iEditing = i;
                        m_pControlDialog->setVisible(true);
                        updateControlDialog();
                        changeZLayer(42);
                        l_bRet = true;
                        break;
                      }
                    }
                  }
                  else if (l_sSender == "btn_ctrl_ok") {
                    buttonControlsOkClicked();
                  }
                  else if (l_sSender == "btn_ctrl_cancel") {
                    buttonControlsCancelClicked();
                  }
                  else if (l_sSender == "btn_more") {
                    if (m_pButtonTab != nullptr) {
                      m_pButtonTab->setVisible(!m_pButtonTab->isVisible());
                      changeZLayer(m_pButtonTab->isVisible() ? 69 : 10);
                    }
                  }
                  else if (l_sSender == "btn_pattern_select") {
                  irr::SEvent l_cEvent{};
                    l_cEvent.EventType = irr::EET_USER_EVENT;
                    l_cEvent.UserEvent.UserData1 = c_iEventImageSelected;
                    l_cEvent.UserEvent.UserData2 = c_iEventImageSelected;
                    buttonTexturePatternSelectClicked(l_cEvent);
                  }
                  else {
                    for (std::map<std::string, std::tuple<std::string, irr::gui::IGUITab *>>::iterator it = m_mButtonLinks.begin(); it != m_mButtonLinks.end(); it++) {
                      if (l_sSender == it->first && std::get<1>(it->second) != nullptr && m_pColorDialog != nullptr) {
                        m_sTextureEdit = std::get<0>(it->second);
                        m_pColorDialog->setVisible(true);

                        if (m_sTextureEdit != "" && m_mGeneratedEd.find(m_sTextureEdit) != m_mGeneratedEd.end() && m_mGeneratedEd[m_sTextureEdit] != nullptr) {
                          std::string l_sColor = helpers::ws2s(m_mGeneratedEd[m_sTextureEdit]->getText());
                      
                          while (l_sColor.size() < 6)
                            l_sColor = "0" + l_sColor;

                          updateColorDialog(l_sColor);
                        }

                        if (m_pButtonTab != nullptr)
                          m_pButtonTab->setVisible(false);

                        changeZLayer(23);
                        l_bRet = true;
                      }
                    }

                    if (!l_bRet) {
                      for (std::vector<gui::CReactiveLabel*>::iterator it = m_vColorPick.begin(); it != m_vColorPick.end(); it++) {
                        if (*it == a_cEvent.GUIEvent.Caller) {
                          std::string l_sColor = helpers::ws2s((*it)->getText());

                          while (l_sColor.size() < 6)
                            l_sColor = "0" + l_sColor;

                          updateColorDialog(l_sColor);
                          l_bRet = true;
                        }
                      }
                    }

                    if (!l_bRet)
                      printf("Button clicked (%s, %i, CMenuProfiles).\n", l_sSender.c_str(), a_cEvent.GUIEvent.Caller->getID());            
                  }
                }
                else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
                  if (l_sSender == "texture_mode") {
                    switchTextureMode();
                    l_bRet = true;
                  }
                  else if (l_sSender == "ai_help") {
                    gui::CSelector *p = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("ai_help", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
                    if (p) {
                      m_aProfiles[m_iEditing].m_cData.m_eAiHelp = (data::SPlayerData::enAiHelp)p->getSelected();

                      l_bRet = true;
                    }
                  }
                  else {
                    for (std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText*>::iterator it = m_mColorLink.begin(); it != m_mColorLink.end(); it++) {
                      if (a_cEvent.GUIEvent.Caller == it->first) {
                        if (it->second != nullptr) {
                          it->second->setText(std::to_wstring(it->first->getPos()).c_str());
                        }

                        irr::s32 l_iRed   = 0,
                                 l_iGreen = 0,
                                 l_iBlue  = 0;

                        for (std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText*>::iterator it2 = m_mColorLink.begin(); it2 != m_mColorLink.end(); it2++) {
                          std::string l_sName = it2->first->getName();

                          if (l_sName == "scrollbar_red")
                            l_iRed = it2->first->getPos();
                          else if (l_sName == "scrollbar_green")
                            l_iGreen = it2->first->getPos();
                          else if (l_sName == "scrollbar_blue")
                            l_iBlue = it2->first->getPos();
                        }

                        if (m_pColorDisplay != nullptr)
                          m_pColorDisplay->setBackgroundColor(irr::video::SColor(0xFF, l_iRed, l_iGreen, l_iBlue));

                        l_bRet = true;
                      }
                    }

                    if (!l_bRet) printf("Scrollbar \"%s\" changed.\n", l_sSender.c_str());
                  }
                }
                else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_LISTBOX_CHANGED || a_cEvent.GUIEvent.EventType == irr::gui::EGET_LISTBOX_SELECTED_AGAIN) {
                  if (a_cEvent.GUIEvent.Caller == m_pCustomTexture) {
                    updateTexture(createTextureString());
                  }
                }
              }
              else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
                l_bRet = buttonTexturePatternSelectClicked(a_cEvent);
              }
            }

            return l_bRet;
          }

          /**
          * This method is called every frame after "scenemanager::drawall" is called
          */
          virtual bool run() override {
            m_pDrv->setRenderTarget(m_pMyRtt, true, true);
            m_pMySmgr->drawAll();
            m_pDrv->setRenderTarget(nullptr, false, false);

            return false;
          }
    };

    IMenuHandler* createMenuProfiles(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuProfiles(a_pDevice, a_pManager, a_pState);
    }
  }
}