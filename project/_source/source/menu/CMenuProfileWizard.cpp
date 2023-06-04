// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/IGuiControllerUiCallback.h>
#include <controller/CControllerGame.h>
#include <helpers/CTextureHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <helpers/CDataHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gui/CReactiveLabel.h>
#include <menu/IMenuHandler.h>
#include <gui/CGuiImageList.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <Defines.h>
#include <random>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuProfileWizard
    * @author Christian Keimel
    * This class handles the menu that acts as a profile
    * for creating / editing a player profile
    */
    class CMenuProfileWizard : public IMenuHandler, public gui::IGuiControllerUiCallback {
      private:
        enum class enMenuStep {
          Initialize,   /**< Initial step: only used when entering the wizard */
          Name,         /**< First step: define the player's name */
          Abbreviation, /**< Second step: pick an abbreviation for the ranking display during race */
          AiHelp,       /**< Third step: define the AI help for the player */
          Controls,     /**< Fourth step: define the profile's controls */
          Texture,      /**< Fifth step: create a custom texture (if wanted) */
          Tutorial      /**< Sixth step: Do you want to play the tutorial? (only available if the wizard was started because no profiles have been found) */
        };

        enMenuStep   m_eStep;         /**< The current step of the wizard */
        CGlobal     *m_pGlobal;       /**< Instance of the global object */
        std::string  m_sColorEdit;    /**< The currently edited color (Step 5: Texture) */
        std::string  m_sProfile;      /**< Number of the currently edited profile */
        std::string  m_sNameOriginal; /**< The original name set when the "Name" step is started. Only used to determine whether or not the abbreviation needs an update */

        data::SPlayerData m_cPlayer;  /**< The player we edit */

        gui::CControllerUi_Game   *m_pCtrl;     /**< The controller UI (Step 4/5) */
        irr::scene::ISceneManager *m_pMySmgr;   /**< Own scene manager for rendering the texture of the marble */
        irr::video::ITexture      *m_pMyRtt;    /**< Render target texture for the marble with it's custom texture */
        irr::scene::ISceneNode    *m_pMarble;   /**< The marble for the texture preview */

        irr::gui::IGUIElement *m_pBtnBack;          /**< The back button */
        irr::gui::IGUIElement *m_pBtnCancel;        /**< The cancel button */
        irr::gui::IGUIElement *m_pBtnNext;          /**< The next button */
        irr::gui::IGUIElement *m_pBtnSave;          /**< The save button */
        irr::gui::IGUIImage   *m_pWarning;          /**< The warning image for the name steps */
        irr::gui::IGUITab     *m_pPatternDialog;    /**< The pattern selection dialog */
        gui::CGuiImageList    *m_pPatternList;      /**< The list of texture patterns */

        void createRandomName() {
          std::vector<std::tuple<std::string, std::string, std::string>> l_vNames = {
            { "Octavia Minor"        , "Minor", "generate://numberback=ECF8BA&numberborder=ECF8BA&numbercolor=77EFBD&pattern=texture_hexagon.png&patternback=ECF8BA&patterncolor=59C4EB&ringcolor=5ADFDF" },
            { "Marcus Aurelius"      , "Rgbg" , "generate://numberback=ffffff&numberborder=ffffff&numbercolor=ff0000&pattern=texture_rgbg.png&patternback=ff0000&patterncolor=ffffff&ringcolor=ff7f7f"        },
            { "Lucius Commodus"      , "Luciu", "generate://numberback=C7395F&numberborder=C7395F&numbercolor=DED4E8&pattern=texture_atomic.png&patternback=C7395F&patterncolor=E8BA40&ringcolor=DED4E8" },
            { "Bruttia Crispina"     , "Brtia", "generate://numberback=A95EA3&numberborder=A95EA3&numbercolor=B6E696&pattern=texture_hammer.png&patternback=A95EA3&patterncolor=DC3A79&ringcolor=1686CD" },
            { "Flavia Iulia"         , "Iulia", "generate://numberback=FAF3DE&numberborder=FAF3DE&numbercolor=78FFC4&pattern=texture_stars.png&patternback=FAF3DE&patterncolor=DCAAE4&ringcolor=FDC2E4" },
            { "Valerius Constantinus", "Const", "generate://numberback=020202&numberborder=020202&numbercolor=5351A2&pattern=texture_gun.png&patternback=020202&patterncolor=F6C845&ringcolor=A254A1" },
            { "Aelius Galenus"       , "Doctr", "generate://numberback=576dc1&numberborder=576dc1&numbercolor=f2d328&pattern=texture_stethoscope.png&patternback=576dc1&patterncolor=f2d328&ringcolor=000000" },
            { "Livia Drusilla"       , "Drusi", "generate://numberback=59C4EB&numberborder=59C4EB&numbercolor=77EFBD&pattern=texture_franconia.png&patternback=59C4EB&patterncolor=ECF8BA&ringcolor=5ADFDF" },
            { "Publius Vergilius"    , "Publi", "generate://numberback=DD671E&numberborder=DD671E&numbercolor=144058&pattern=texture_bowling.png&patternback=DD671E&patterncolor=4D181C&ringcolor=E58D2E" },
            { "Claudius Germanicus"  , "Nero" , "generate://numberback=640000&numberborder=640000&numbercolor=ff6400&pattern=texture_flames.png&patternback=ff0000&patterncolor=ff6900&ringcolor=ff9696" },
            { "Aelius Hadrianus"     , "Hadri", "generate://numberback=E87A5C&numberborder=E87A5C&numbercolor=3B5BA5&pattern=texture_spqr.png&patternback=E87A5C&patterncolor=DE418E&ringcolor=469E48" },
            { "Magnus Pius"          , "Pius" , "generate://numberback=6A513C&numberborder=6A513C&numbercolor=A4998E&pattern=texture_nuclear.png&patternback=6A513C&patterncolor=507B6A&ringcolor=4B1816" },
            { "Aurelia Cotta"        , "Cotta", "generate://numberback=E6C17A&numberborder=E6C17A&numbercolor=404041&pattern=texture_lion.png&patternback=E6C17A&patterncolor=F6EDE3&ringcolor=404041" },
            { "Pompeia Sulla"        , "Sulla", "generate://numberback=E26274&numberborder=E26274&numbercolor=F9EC7E&pattern=texture_rollin.png&patternback=E26274&patterncolor=E3CCB2&ringcolor=F9EC7E" },
            { "Pompeius Rufus"       , "Rufus", "generate://numberback=3B5BA5&numberborder=3B5BA5&numbercolor=F3B941&pattern=texture_stars.png&patternback=3B5BA5&patterncolor=E87A5D&ringcolor=F3B941" },
            { "Marcus Caelius"       , "Caeli", "generate://numberback=E3856B&numberborder=E3856B&numbercolor=80C4B7&pattern=texture_bass.png&patternback=E3856B&patterncolor=EDCBD2&ringcolor=80C4B7" },
            { "Lucius Atratinus"     , "Atrat", "generate://numberback=656E77&numberborder=656E77&numbercolor=CAD4DF&pattern=texture_bomb.png&patternback=656E77&patterncolor=DDDBDE&ringcolor=3B373B" },
            { "Tullius Cicero"       , "Cicro", "generate://numberback=00246B&numberborder=00246B&numbercolor=CADCFC&pattern=texture_arrow.png&patternback=00246B&patterncolor=8AB6F9&ringcolor=CADCFC" },
            { "Caecilius Pomponianus", "Pompo", "generate://numberback=1686CD&numberborder=1686CD&numbercolor=DC3A79&pattern=texture_dustbin.png&patternback=1686CD&patterncolor=B6E696&ringcolor=A95EA3" },
            { "Pomponia Metella"     , "Metel", "generate://numberback=D0944D&numberborder=D0944D&numbercolor=67C2D4&pattern=texture_atomic.png&patternback=D0944D&patterncolor=3988A4&ringcolor=CB625F" },
            { "Vipsania Agrippina"   , "Agrip", "generate://numberback=E7A339&numberborder=E7A339&numbercolor=91B187&pattern=texture_hearts.png&patternback=E7A339&patterncolor=4AAFD5&ringcolor=91B187" },
            { "Ludovicus Secundus"   , "Kini" , "generate://numberback=ffffff&numberborder=ffffff&numbercolor=000000&pattern=texture_diamond.png&patternback=ffffff&patterncolor=0000ff&ringcolor=000000" },
            { "Carolus Magnus"       , "Frnko", "generate://numberback=ff0000&numberborder=ff0000&numbercolor=ffffff&pattern=texture_franconia.png&patternback=ffffff&patterncolor=ff0000&ringcolor=000000" },
            { "Avidius Cassius"      , "AvCas", "generate://numberback=F2EC9B&numberborder=F2EC9B&numbercolor=96FFBD&pattern=texture_hippo.png&patternback=F2EC9B&patterncolor=1803A5&ringcolor=96FFBD" },
            { "Septimius Severus"    , "SepSe", "generate://numberback=866C69&numberborder=866C69&numbercolor=CD8C8C&pattern=texture_explosion.png&patternback=866C69&patterncolor=D4B8B1&ringcolor=53331F" },
            { "Licinius Varus"       , "Varus", "generate://numberback=FEFAAE&numberborder=FEFAAE&numbercolor=7EE05F&pattern=texture_pommesgabel.png&patternback=FEFAAE&patterncolor=2249AE&ringcolor=223E8B" },
            { "Aurelius Theo"        , "Theo" , "generate://numberback=7EE05F&numberborder=7EE05F&numbercolor=223E8B&pattern=texture_samurai.png&patternback=7EE05F&patterncolor=2249AE&ringcolor=FEFAAE" },
            { "Cassius Longinus"     , "CasLo", "generate://numberback=ffffff&numberborder=ffffff&numbercolor=000000&pattern=texture_skull.png&patternback=000000&patterncolor=ffffff&ringcolor=ff3200" },
            { "Marcus Iunius"        , "MaIun", "generate://numberback=646464&numberborder=646464&numbercolor=ffffff&pattern=texture_jollyroger.png&patternback=000000&patterncolor=ffffff&ringcolor=ffffff" },
            { "Decimus Albinus"      , "Brtus", "generate://numberback=ae5439&numberborder=ae5439&numbercolor=000000&pattern=texture_gun.png&patternback=eee951&patterncolor=ae5439&ringcolor=000000" }                      
          };

          {
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };
            std::shuffle(l_vNames.begin(), l_vNames.end(), l_cRe);
          }

          std::tuple<std::string, std::string, std::string> l_tName = *l_vNames.begin();

          m_cPlayer.m_sName      = std::get<0>(l_tName);
          m_cPlayer.m_sShortName = std::get<1>(l_tName);
          m_cPlayer.m_sTexture   = std::get<2>(l_tName);

          m_sNameOriginal = m_cPlayer.m_sName;
        }

        /**
        * Update the pattern images starting with "m_iPatternPage"
        * @see m_iPatternPage
        */
        void updatePatterns() {
          if (m_pPatternList != nullptr) {
            irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/patterns/patterns.xml");

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
                        l_vPatterns.push_back(gui::CGuiImageList::SListImage(l_sPath, l_sPattern, l_sPattern, -1));
                      }
                    }
                  }
                }
              }

              m_pPatternList->setImageList(l_vPatterns);

              irr::gui::IGUIEditBox *l_pPattern = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("texture_pattern", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

              if (l_pPattern != nullptr)
                m_pPatternList->setSelected(helpers::ws2s(l_pPattern->getText()), false);

              l_pXml->drop();
            }
          }
        }

        /**
        * Update the texture of the preview marble in the texture wizard step
        * @param a_sTexture the texture string of the marble
        */
        void updateMarbleTexture(const std::string &a_sTexture) {
          if (m_pMyRtt != nullptr && m_pMarble != nullptr) {
            m_pMarble->getMaterial(0).setTexture(0, helpers::createTexture(a_sTexture != "" ? a_sTexture : "default://number=1", m_pDrv, m_pFs));
          }
        }

        /**
        * Get the texture string from the texture dialog
        * @return the texture string from the texture dialog
        */
        std::wstring getTextureString() {
          gui::CSelector *l_pMode = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("texture_mode", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          std::wstring l_sTexture = L"default://";

          if (l_pMode != nullptr) {
            if (l_pMode->getSelected() == 1) {
              l_sTexture = L"";

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
                  if (l_sTexture == L"")
                    l_sTexture = L"generate://";
                  else
                    l_sTexture = l_sTexture + L"&";

                  l_sTexture += it->first + L"=" + p->getText();
                }
              }
            }
          }

          printf("\n%s\n", helpers::ws2s(l_sTexture).c_str());
          return l_sTexture;
        }

        /**
        * Initialize the scrollbars of the color dialog
        * @param a_sColor the color to initialize with
        */
        void initializeColorDialog(const std::wstring& a_sColor) {
          if (a_sColor.size() >= 6) {
            std::map<int, std::string> l_mMap = {
              { 0, "scrollbar_red"   },
              { 2, "scrollbar_green" },
              { 4, "scrollbar_blue"  }
            };

            for (std::map<int, std::string>::iterator l_itColor = l_mMap.begin(); l_itColor != l_mMap.end(); l_itColor++) {
              irr::gui::IGUIScrollBar  *l_pColor = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(l_itColor->second, irr::gui::EGUIET_SCROLL_BAR , m_pGui->getRootGUIElement()));
              if (l_pColor != nullptr) {
                char *p = nullptr;
                std::string l_sSub = helpers::ws2s(a_sColor.substr(l_itColor->first, 2));
                l_pColor->setPos(std::strtol(l_sSub.c_str(), &p, 16));
              }
            }

            irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_display", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

            if (l_pTab != nullptr)
              l_pTab->setBackgroundColor(getColorFromColorDialog());
          }
        }

        /**
        * Translate a color to a HTML style string (RRGGBB)
        * @param a_cColor the color to translate
        * @return the translated color as string
        */
        std::string colorToString(const irr::video::SColor &a_cColor) {
          std::string l_sColor = "";
          char s[0xFF]; 

          sprintf(s,"%x", a_cColor.getRed  ()); std::string l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor  = l_sDummy;
          sprintf(s,"%x", a_cColor.getGreen());             l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor += l_sDummy;
          sprintf(s,"%x", a_cColor.getBlue ());             l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor += l_sDummy;

          return l_sColor;
        }

        /**
        * Get the selected color from the color dialog
        * @return the selected color from the color dialog
        */
        irr::video::SColor getColorFromColorDialog() {
          std::map<std::string, std::string> l_mColorParts = {
            { "scrollbar_red"  , "value_red"   },
            { "scrollbar_green", "value_green" },
            { "scrollbar_blue" , "value_blue"  }
          };

          irr::video::SColor l_cColor = irr::video::SColor(0xFF, 0, 0, 0);

          for (std::map<std::string, std::string>::iterator l_itColor = l_mColorParts.begin(); l_itColor != l_mColorParts.end(); l_itColor++) {
            irr::gui::IGUIScrollBar  *l_pColor = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(l_itColor->first , irr::gui::EGUIET_SCROLL_BAR , m_pGui->getRootGUIElement()));
            irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType(l_itColor->second, irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));


            if (l_pColor != nullptr && l_pLabel != nullptr) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%i", l_pColor->getPos());
              l_pLabel->setText(s);

              if (l_itColor->first == "scrollbar_red"  ) l_cColor.setRed  ((irr::u32)l_pColor->getPos());
              if (l_itColor->first == "scrollbar_green") l_cColor.setGreen((irr::u32)l_pColor->getPos());
              if (l_itColor->first == "scrollbar_blue" ) l_cColor.setBlue ((irr::u32)l_pColor->getPos());
            }
          }

          return l_cColor;
        }

        /**
        * Update the generated texture of the preview marble
        */
        void updateGeneratedTexture() {
          gui::CSelector *l_pMode = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("texture_mode", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (l_pMode != nullptr) {
            irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("texture_generated", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));
            gui::CMenuButton  *l_pBtn = reinterpret_cast<gui::CMenuButton  *>(findElementByNameAndType("btn_texture_params", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

            if (l_pMode->getSelected() == 0) {
              if (l_pTab != nullptr) l_pTab->setVisible(false);
              if (l_pBtn != nullptr) l_pBtn->setVisible(false);

              updateMarbleTexture(m_cPlayer.m_sTexture);
            }
            else {
              if (l_pTab != nullptr) l_pTab->setVisible(true);
              if (l_pBtn != nullptr) l_pBtn->setVisible(true);
            }
          }
        }

        /**
        * Update the info text for the AI help
        */
        void updateAiHelpInfo() {
          irr::gui::IGUIStaticText *l_pInfo  = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("aihelp_info", irr::gui::EGUIET_STATIC_TEXT                  , m_pGui->getRootGUIElement()));
          gui::CSelector           *l_pLevel = reinterpret_cast<gui::CSelector           *>(findElementByNameAndType("ai_help"    , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

          if (l_pLevel != nullptr && l_pInfo != nullptr) {
            switch (l_pLevel->getSelected()) {
              case 0:
                l_pInfo->setText(L"The AI does not help you, you are on your own.");
                break;

              case 1:
                l_pInfo->setText(L"You get some hints by the AI showing you the way it would control the marble at any time in the race.");
                break;

              case 2:
                l_pInfo->setText(L"You get some hints by the AI showing you the way it would control the marble at any time in the race, and it will take over speed control when you approach a jump.");
                break;

              case 3:
                l_pInfo->setText(L"The AI takes over speed control so you can learn a good (probably not the best possible) way to race. You can however override the speed control by using the brake.");
                break;

              case 4:
                l_pInfo->setText(L"The AI adjusts your steering if necessary and speed is controlled by the AI, and when you approach a jump the AI takes over complete control. But you have the option to override the speed control by using the brake.");
                break;

              case 5:
                l_pInfo->setText(L"The MarbleGP AI controls your marble. MarbleGP is the best option AI can offer, use this mode to watch the AI and learn a good way to race.");
                break;

              case 6:
                l_pInfo->setText(L"Marble2 AI takes control of the marble. Marble2 is a good option to race, you can use this mode to watch racing and possibly learn a reasonable way to race.");
                break;

              case 7:
                l_pInfo->setText(L"Have your marble controlled by Marble3 AI. Note however that this AI level is not really good, so to learn how to race a better AI level would be more sufficient.");
                break;
              }
          }
        }

        /**
        * We are changing the current wizard step, so we need to save the data
        */
        void saveDataOfWizardStep() {
          // Save the data of the old step
          switch (m_eStep) {
            case enMenuStep::Initialize:
              // Nothing to save here, this is the initial step
              break;

            case enMenuStep::Name: {
              // Copy the name field to the m_sName member
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr) {
                m_cPlayer.m_sName = helpers::ws2s(l_pEdit->getText());
                if (m_cPlayer.m_sName.substr(0, 5) != m_sNameOriginal.substr(0, 5))
                  m_cPlayer.m_sShortName = m_cPlayer.m_sName.substr(0, 5);
              }
              break;
            }

            case enMenuStep::Abbreviation: {
              // Copy the short name to the m_sShort member
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr)
                m_cPlayer.m_sShortName = helpers::ws2s(l_pEdit->getText());
              break;
            }

            case enMenuStep::AiHelp: {
              // Save the selected AI help to the member m_eAiHelp
              gui::CSelector *l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("ai_help", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
              if (l_pSelector != nullptr) {
                switch (l_pSelector->getSelected()) {
                  case 0: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Off    ; break;
                  case 1: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Display; break;
                  case 2: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Low    ; break;
                  case 3: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Medium ; break;
                  case 4: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::High   ; break;
                  case 5: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMgp ; break;
                  case 6: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMb2 ; break;
                  case 7: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMb3 ; break;
                }
              }
              break;
            }

            case enMenuStep::Controls: {
              gui::CSelector      *l_pType  = reinterpret_cast<gui::CSelector      *>(findElementByNameAndType("controller_type", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
              irr::gui::IGUIImage *l_pImage = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("controller_img" , irr::gui::EGUIET_IMAGE                        , m_pGui->getRootGUIElement()));

              bool l_bSaved = false;

              if (l_pType != nullptr && l_pImage != nullptr) {
                unsigned     l_iSelected = l_pType->getSelected();
                std::wstring l_sSelected = l_iSelected >= 0 && l_iSelected < l_pType->getItemCount() ? l_pType->getItem(l_iSelected) : L"";

                l_pImage->setVisible(false);

                std::map<std::wstring, std::string> l_mItemMap = {
                  { L"Touch Control"                , "DustbinTouchControl"    },
                  { L"Gyroscope"                    , "DustbinGyroscope"       }
                };

                for (std::map<std::wstring, std::string>::iterator l_itCtrl = l_mItemMap.begin(); l_itCtrl != l_mItemMap.end(); l_itCtrl++) {
                  if (l_itCtrl->first == l_sSelected) {
                    l_bSaved = true;
                    m_cPlayer.m_sControls = l_itCtrl->second;
                  }
                }
              }

              gui::CDustbinCheckbox *l_pAuto = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("auto_throttle", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement()));
              if (l_pAuto != nullptr && !l_bSaved) {
                m_cPlayer.m_bAutoThrottle = l_pAuto->isChecked();
              }

              // Serialize the controls and save it to m_sControls
              if (m_pCtrl != nullptr && !l_bSaved) {
                m_cPlayer.m_sControls = m_pCtrl->serialize();
              }
              m_pCtrl = nullptr;
              break;
            }

            case enMenuStep::Texture: {
              // Serialize the texture and save it to m_sTexture
              break;
            }

            case enMenuStep::Tutorial:
              break;
          }
        }

        /**
        * Fill the edit fields with the member data
        */
        void restoreDataOfWizardStep() {
          // Fill the wizard edit fields with the available data
          switch (m_eStep) {
            case enMenuStep::Initialize:
              // Nothing to do here, we never reach this state again
              break;

            case enMenuStep::Name: {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr) {
                l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sName).c_str());

                m_sNameOriginal = m_cPlayer.m_sName;

                irr::SEvent l_cEvent {};
                l_cEvent.EventType = irr::EET_KEY_INPUT_EVENT;
                l_cEvent.KeyInput.Shift       = false;
                l_cEvent.KeyInput.Control     = false;
                l_cEvent.KeyInput.Char        = 0;
                l_cEvent.KeyInput.Key         = irr::KEY_END;
                l_cEvent.KeyInput.PressedDown = true;
                l_pEdit->OnEvent(l_cEvent);
              }
              break;
            }

            case enMenuStep::Abbreviation: {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr) {
                l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sShortName).c_str());

                irr::SEvent l_cEvent{};
                l_cEvent.EventType = irr::EET_KEY_INPUT_EVENT;
                l_cEvent.KeyInput.Shift       = false;
                l_cEvent.KeyInput.Control     = false;
                l_cEvent.KeyInput.Char        = 0;
                l_cEvent.KeyInput.Key         = irr::KEY_END;
                l_cEvent.KeyInput.PressedDown = true;
                l_pEdit->OnEvent(l_cEvent);
              }
              break;
            }

            case enMenuStep::AiHelp: {
              gui::CSelector *l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("ai_help", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

              if (l_pSelector != nullptr) {
                switch (m_cPlayer.m_eAiHelp) {
                  case data::SPlayerData::enAiHelp::Off    : l_pSelector->setSelected(0); break;
                  case data::SPlayerData::enAiHelp::Display: l_pSelector->setSelected(1); break;
                  case data::SPlayerData::enAiHelp::Low    : l_pSelector->setSelected(2); break;
                  case data::SPlayerData::enAiHelp::Medium : l_pSelector->setSelected(3); break;
                  case data::SPlayerData::enAiHelp::High   : l_pSelector->setSelected(4); break;
                  case data::SPlayerData::enAiHelp::BotMgp : l_pSelector->setSelected(5); break;
                  case data::SPlayerData::enAiHelp::BotMb2 : l_pSelector->setSelected(6); break;
                  case data::SPlayerData::enAiHelp::BotMb3 : l_pSelector->setSelected(7); break;
                }

                m_pGui->setFocus(l_pSelector);
              }

              updateAiHelpInfo();
              break;
            }

            case enMenuStep::Controls: {
              bool l_bTouchGyro = false;

              m_pCtrl = reinterpret_cast<gui::CControllerUi_Game *>(findElementByNameAndType("controller_ui", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId, m_pGui->getRootGUIElement()));

              if (m_pCtrl != nullptr) {
                if (m_cPlayer.m_sControls == "") {
#ifdef _ANDROID
                  m_cPlayer.m_sControls = "DustbinTouchControl";
#else
                  controller::CControllerGame l_cCtrl;
                  m_cPlayer.m_sControls = l_cCtrl.serialize();
#endif
                }

                if (m_cPlayer.m_sControls.substr(0, std::string("DustbinController").size()) == "DustbinController") {
                  m_pCtrl->setController(m_cPlayer.m_sControls);
                  gui::CSelector *l_pType  = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("controller_type", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                  if (l_pType != nullptr) {
                    controller::CControllerGame l_cCtrl;
                    l_cCtrl.deserialize(m_cPlayer.m_sControls);

                    if (l_cCtrl.getInputs().size() > 0) {
                      if (l_cCtrl.getInputs()[0].m_eType == controller::CControllerBase::enInputType::Key) {
                        l_pType->setSelectedItem(L"Keyboard");
                      }
                      else {
                        l_pType->setSelectedItem(L"Gamepad");
                      }
                    }
                  }
                }
                else {
                  gui::CSelector      *l_pType  = reinterpret_cast<gui::CSelector      *>(findElementByNameAndType("controller_type", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
                  irr::gui::IGUIImage *l_pImage = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("controller_img" , irr::gui::EGUIET_IMAGE                        , m_pGui->getRootGUIElement()));

                  if (l_pType != nullptr && l_pImage != nullptr) {
                    std::vector<std::tuple<std::string, std::string, std::wstring>> l_mItemMap = {
                      { "DustbinTouchControl"   , "data/images/ctrl_config_touch.png"      , L"Touch Control"                 },
                      { "DustbinGyroscope"      , "data/images/ctrl_config_gyro.png"       , L"Gyroscope"                     }
                    };

                    for (std::vector<std::tuple<std::string, std::string, std::wstring>>::iterator l_itCtrl = l_mItemMap.begin(); l_itCtrl != l_mItemMap.end(); l_itCtrl++) {
                      if (m_cPlayer.m_sControls == std::get<0>(*l_itCtrl)) {
                        m_pCtrl ->setVisible(false);
                        l_pImage->setVisible(true );
                        l_pImage->setImage(m_pDrv->getTexture(std::get<1>(*l_itCtrl).c_str()));

                        l_bTouchGyro = true;

                        for (unsigned i = 0; i < l_pType->getItemCount(); i++) {
                          if (std::get<2>(*l_itCtrl) == l_pType->getItem(i)) {
                            l_pType->setSelected(i);
                            break;
                          }
                        }
                        break;
                      }
                    }
                  }
                  gui::CMenuButton    *l_pEdit  = reinterpret_cast<gui::CMenuButton    *>(findElementByNameAndType("editGameCtrl"  , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
                  gui::CMenuButton    *l_pTest  = reinterpret_cast<gui::CMenuButton    *>(findElementByNameAndType("testGameCtrl"  , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

                  if (l_pEdit != nullptr) l_pEdit->setVisible(false);
                  if (l_pTest != nullptr) l_pTest->setVisible(false);
                }
              }

              gui::CDustbinCheckbox *l_pAuto = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("auto_throttle", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement()));

              if (l_pAuto != nullptr) {
                l_pAuto->setChecked(m_cPlayer.m_bAutoThrottle || l_bTouchGyro);
                l_pAuto->setEnabled(!l_bTouchGyro);
              }

              break;
            }

            case enMenuStep::Texture: {
              std::string l_sTexture = m_cPlayer.m_sTexture;
              if (l_sTexture == "")
                l_sTexture = "default://number=1";

              size_t l_iPos = l_sTexture.find("://");
              if (l_iPos != std::string::npos) {
                std::string l_sPrefix  = l_sTexture.substr(0, l_iPos);

                printf("Prefix: %s\n", l_sPrefix.c_str());
                gui::CSelector    *l_pMode = reinterpret_cast<gui::CSelector    *>(findElementByNameAndType("texture_mode"      , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId  , m_pGui->getRootGUIElement()));
                gui::CMenuButton  *l_pBtn  = reinterpret_cast<gui::CMenuButton  *>(findElementByNameAndType("btn_texture_params", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
                irr::gui::IGUITab *l_pTab  = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("texture_generated" , irr::gui::EGUIET_TAB                            , m_pGui->getRootGUIElement()));

                if (l_pMode != nullptr) {
                  if (l_pTab != nullptr) {
                    l_pTab->setVisible(l_sPrefix == "generate");
                  }

                  if (l_pBtn != nullptr) {
                    l_pBtn->setVisible(l_sPrefix == "generate");
                  }

                  if (l_sPrefix == "generate") {
                    l_pMode->setSelected(1);

                    l_sTexture = m_cPlayer.m_sTexture.substr(l_iPos + 3);

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

                    std::vector<std::string> l_vParts = helpers::splitString(l_sTexture, '&');

                    for (std::vector<std::string>::iterator l_itPart = l_vParts.begin(); l_itPart != l_vParts.end(); l_itPart++) {
                      std::vector<std::string> l_vParam = helpers::splitString(*l_itPart, '=');

                      printf("%s ==> %s\n", l_vParam[0].c_str(), l_vParam[1].c_str());

                      if (l_vParam.size() == 2 && l_mParamMap.find(l_vParam[0]) != l_mParamMap.end()) {
                        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(l_mParamMap[l_vParam[0]], irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                        if (l_pEdit != nullptr)
                          l_pEdit->setText(helpers::s2ws(l_vParam[1]).c_str());
                      }
                    }
                  }
                  else {
                    l_pMode->setSelected(0);
                  }
                }
              }
              break;
            }

            case enMenuStep::Tutorial:
              break;
          }
        }

        /**
        * Toggle the button visibility depending on state and input fields. Also
        * update the background color of the edit fields for (short) name
        */
        void toggleButtonVisibility() {
          if (m_pBtnBack != nullptr)
            m_pBtnBack->setVisible(m_eStep != enMenuStep::Initialize && m_eStep != enMenuStep::Name);

          if (m_pBtnNext != nullptr) {
            bool l_bVisible = m_eStep != enMenuStep::Texture;

            if (m_eStep == enMenuStep::Name) {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

              if (l_pEdit != nullptr) {
                std::wstring s = l_pEdit->getText();
                l_bVisible = s.size() >= 3;

                if (m_pWarning != nullptr)
                  m_pWarning->setVisible(!l_bVisible);
              }
            }
            else if (m_eStep == enMenuStep::Abbreviation) {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

              if (l_pEdit != nullptr) {
                std::wstring s = l_pEdit->getText();
                l_bVisible = s.size() >= 2;

                if (m_pWarning != nullptr)
                  m_pWarning->setVisible(!l_bVisible);
              }
            }

            m_pBtnNext->setVisible(l_bVisible);
          }

          if (m_pBtnCancel != nullptr)
            m_pBtnCancel->setVisible(m_sProfile != "commit_profile" || m_eStep == enMenuStep::Tutorial);

          if (m_pBtnSave != nullptr)
            m_pBtnSave->setVisible(m_eStep == enMenuStep::Texture || m_eStep == enMenuStep::Tutorial);
        }

        /**
        * Change the current wizard step
        * @para a_eStep the new step
        */
        void changeStep(enMenuStep a_eStep) {
          saveDataOfWizardStep();

          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML(
            a_eStep == enMenuStep::Name         ? "data/menu/profilewizard_name.xml"      :
                   a_eStep == enMenuStep::Abbreviation ? "data/menu/profilewizard_shortname.xml" :
                   a_eStep == enMenuStep::AiHelp       ? "data/menu/profilewizard_aihelp.xml"    :
                   a_eStep == enMenuStep::Controls     ? "data/menu/profilewizard_controls.xml"  :
                   a_eStep == enMenuStep::Texture      ? "data/menu/profilewizard_texture.xml"   :
                   a_eStep == enMenuStep::Tutorial     ? "data/menu/profilewizard_tutorial.xml"  : "",
            m_pGui->getRootGUIElement(), m_pGui
          );

          if (a_eStep == enMenuStep::Name) {
            irr::gui::IGUIElement *l_pBack = findElementByName("back", m_pGui->getRootGUIElement());

            if (l_pBack != nullptr)
              l_pBack->setVisible(false);
          }
          else if (a_eStep == enMenuStep::Controls) {
            gui::CSelector      *l_pType  = reinterpret_cast<gui::CSelector      *>(findElementByNameAndType("controller_type", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
            irr::gui::IGUIImage *l_pImage = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("controller_img" , irr::gui::EGUIET_IMAGE                        , m_pGui->getRootGUIElement()));

            // If we find the selector for the controller type
            // we assume that we are on Android
            if (l_pType != nullptr && l_pImage != nullptr) {
              l_pType->addItem(L"Gamepad"); 
              l_pType->addItem(L"Touch Control");
              l_pType->addItem(L"Gyroscope");
            }
          }
          else if (a_eStep == enMenuStep::Texture) {
            // In the texture dialog we hide the "next" button ...
            irr::gui::IGUIElement *l_pNext = findElementByName("next", m_pGui->getRootGUIElement());

            if (l_pNext != nullptr)
              l_pNext->setVisible(false);

            // ... and we shot the "save" button
            irr::gui::IGUIElement *l_pSave = findElementByName("save", m_pGui->getRootGUIElement());

            if (l_pSave != nullptr)
              l_pSave->setVisible(true);

            if (m_pMySmgr == nullptr) {
              m_pMySmgr = m_pSmgr->createNewSceneManager(false);
              m_pMySmgr->loadScene("data/scenes/texture_scene.xml");
              irr::scene::ICameraSceneNode *l_pCam = m_pMySmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
              l_pCam->setAspectRatio(1.0f);
            }

            if (m_pMyRtt == nullptr) {
              m_pMyRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");
            }

            irr::gui::IGUIImage *l_pTexture = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("texture_image", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));

            if (l_pTexture != nullptr && m_pMyRtt != nullptr)
              l_pTexture->setImage(m_pMyRtt);

            if (m_pMySmgr != nullptr) {
              m_pMarble = m_pMySmgr->getSceneNodeFromName("marble");
              updateMarbleTexture(m_cPlayer.m_sTexture);
            }
          }

          m_eStep = a_eStep;

          m_pBtnBack   = findElementByName("back"  , m_pGui->getRootGUIElement());
          m_pBtnCancel = findElementByName("cancel", m_pGui->getRootGUIElement());
          m_pBtnNext   = findElementByName("next"  , m_pGui->getRootGUIElement());
          m_pBtnSave   = findElementByName("save"  , m_pGui->getRootGUIElement());

          m_pWarning   = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("warning", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));

          if (m_pWarning != nullptr) {
            m_pWarning->setImage(m_pDrv->getTexture("data/images/edit_warning.png"));
          }

          restoreDataOfWizardStep();
          toggleButtonVisibility();
        }

        void prepareSetupGame() {
          m_pState->getGlobal()->setSetting("track"          , "noob_oval");
          m_pState->getGlobal()->setSetting("selectedplayers", m_cPlayer.serialize());
        }

      public:
        CMenuProfileWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler    (a_pDevice, a_pManager, a_pState),
          m_eStep         (enMenuStep::Initialize),
          m_pGlobal       (CGlobal::getInstance()),
          m_sColorEdit    (""),
          m_sProfile      (""),
          m_pCtrl         (nullptr),
          m_pMySmgr       (nullptr),
          m_pMyRtt        (nullptr),
          m_pMarble       (nullptr),
          m_pBtnBack      (nullptr),
          m_pBtnCancel    (nullptr),
          m_pBtnNext      (nullptr),
          m_pBtnSave      (nullptr),
          m_pWarning      (nullptr),
          m_pPatternDialog(nullptr),
          m_pPatternList  (nullptr)
        {
          m_sProfile = m_pGlobal->getGlobal("edit_profile");
          m_pGlobal->setGlobal("edit_profile", "");

          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_cPlayer = data::SPlayerData();
          m_cPlayer.m_iPlayerId = 1;

          if (m_sProfile == "commit_profile") {
            createRandomName();
          }
          else if (m_sProfile != "") {  
            m_cPlayer.deserialize(m_sProfile);
          }
          else {
            std::vector<data::SPlayerData> l_vPlayers = data::SPlayerData::createPlayerVector(m_pGlobal->getSetting("profiles"));
            int l_iIndex = 1;

            while (true) {
              bool l_bFound = false;

              std::string l_sName = "Player " + std::to_string(l_iIndex);

              for (auto& l_cPlayer : l_vPlayers) {
                if (l_cPlayer.m_sName == l_sName) {
                  l_bFound = true;
                  break;
                }
              }

              if (l_bFound) {
                l_iIndex++;
              }
              else {
                createRandomName();
                break;
              }
            }
          }

          changeStep(enMenuStep::Name);
        }

        virtual ~CMenuProfileWizard() {
          if (m_pMySmgr != nullptr)
            m_pMySmgr->drop();
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (m_pCtrl != nullptr && m_eStep == enMenuStep::Controls) {
            l_bRet = m_pCtrl->OnEvent(a_cEvent);
          }

          if (!l_bRet) {
            if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
              if (a_cEvent.KeyInput.Key == irr::KEY_RETURN && !a_cEvent.KeyInput.PressedDown) {
                if ((m_pBtnNext != nullptr && m_pBtnNext->isVisible()) || (m_pBtnSave != nullptr && m_pBtnSave->isVisible())) {
                  irr::SEvent l_cEvent {};

                  l_cEvent.EventType = irr::EET_GUI_EVENT;
                  l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
                  l_cEvent.GUIEvent.Caller    = (m_pBtnNext != nullptr && m_pBtnNext->isVisible()) ? m_pBtnNext : m_pBtnSave;
                  l_cEvent.GUIEvent.Element   = m_pBtnNext;
 
                  OnEvent(l_cEvent);

                  l_bRet = true;
                }
              }
            }
            else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
              if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

                if (l_sButton == "cancel") {
                  if (m_pBtnCancel != nullptr && m_pBtnCancel->isVisible()) {
                    if (m_eStep != enMenuStep::Tutorial)
                      m_pManager->changeMenu(createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState));
                    else {
                      prepareSetupGame();
                      m_pManager->pushToMenuStack("menu_selecttrack");
                      m_pManager->changeMenu(createMenu("menu_setupgame", m_pDevice, m_pManager, m_pState));
                    }
                  }
                }
                else if (l_sButton == "save") {
                  if (m_eStep != enMenuStep::Tutorial) {
                    std::string l_sEditProfileNo = m_pState->getGlobal()->getGlobal("edit_profileno");
                    int l_iEditProfileNo = std::atoi(l_sEditProfileNo.c_str());

                    std::vector<data::SPlayerData> l_vPlayers = data::SPlayerData::createPlayerVector(m_pGlobal->getSetting("profiles"));

                    if (l_sEditProfileNo != "" && l_iEditProfileNo >= 0 && l_iEditProfileNo < l_vPlayers.size()) {
                      l_vPlayers[l_iEditProfileNo] = m_cPlayer;
                    }
                    else {
                      l_vPlayers.push_back(m_cPlayer);
                    }
                    helpers::saveProfiles(l_vPlayers);

                    if (m_sProfile == "commit_profile" && m_pBtnSave != nullptr && m_pBtnSave->isVisible()) {
                      changeStep(enMenuStep::Tutorial);
                    }
                    else {
                      createMenu(m_pManager->popMenuStack(), m_pDevice,m_pManager, m_pState);
                    }
                  }
                  else {
                    data::SGameData l_cData;

                    l_cData.m_eType       = data::SGameData::enType::Local;
                    l_cData.m_iClass      = 0;
                    l_cData.m_iLaps       = 1;
                    l_cData.m_sTrack      = "tutorial";
                    l_cData.m_bIsTutorial = true;

                    CGlobal::getInstance()->setGlobal("gamedata", l_cData.serialize());

                    data::SGameSettings l_cSettings;

                    m_cPlayer.m_iViewPort = 1;
                    m_cPlayer.m_iGridPos  = 1;
                    m_cPlayer.m_iPlayerId = 1;

                    data::SRacePlayers l_cPlayers;
                    l_cPlayers.m_vPlayers.push_back(m_cPlayer);

                    prepareSetupGame();
                    m_pManager->pushToMenuStack("menu_selecttrack");
                    m_pManager->pushToMenuStack("menu_setupgame");
                    CGlobal::getInstance()->setGlobal("raceplayers", l_cPlayers.serialize());
                    m_pState->setState(state::enState::Game);
                  }
                }
                else if (l_sButton == "clear_name") {
                  if (m_eStep == enMenuStep::Name) {
                    irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                    if (l_pEdit != nullptr) {
                      l_pEdit->setText(L"");
                      toggleButtonVisibility();
                    }
                  }
                  else if (m_eStep == enMenuStep::Abbreviation) {
                    irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                    if (l_pEdit != nullptr) {
                      l_pEdit->setText(L"");
                      toggleButtonVisibility();
                    }
                  }
                }
                else if (l_sButton == "next") {
                  switch (m_eStep) {
                    case enMenuStep::Name        : changeStep(enMenuStep::Abbreviation); break;
                    case enMenuStep::Abbreviation: changeStep(enMenuStep::AiHelp      ); break;
                    case enMenuStep::AiHelp      : changeStep(enMenuStep::Controls    ); break;
                    case enMenuStep::Controls    : changeStep(enMenuStep::Texture     ); break;
                    case enMenuStep::Texture     :
                      break;

                    case enMenuStep::Initialize  :
                      // This button is not visible here, so let's ignore it
                      break;
                    case enMenuStep::Tutorial:
                      break;
                  }
                }
                else if (l_sButton == "back") {
                  switch (m_eStep) {
                    case enMenuStep::Name      :
                    case enMenuStep::Initialize:
                      // This button isn't visible here, so we ignore it
                      break;

                    case enMenuStep::Abbreviation: changeStep(enMenuStep::Name        ); break;
                    case enMenuStep::AiHelp      : changeStep(enMenuStep::Abbreviation); break;
                    case enMenuStep::Controls    : changeStep(enMenuStep::AiHelp      ); break;
                    case enMenuStep::Texture     : changeStep(enMenuStep::Controls    );  break;
                    case enMenuStep::Tutorial    : break;
                  }
                }
                else if (l_sButton == "editGameCtrl") {
                  if (m_pCtrl != nullptr)
                    m_pCtrl->setMode(gui::CControllerUi::enMode::Wizard);
                }
                else if (l_sButton == "testGameCtrl") {
                  if (m_pCtrl != nullptr) {
                    if (m_pCtrl->getMode() == gui::CControllerUi::enMode::Display)
                      m_pCtrl->startTest();
                    else if (m_pCtrl->getMode() == gui::CControllerUi::enMode::Test)
                      m_pCtrl->setMode(gui::CControllerUi::enMode::Display);
                  }
                }
                else if (l_sButton == "random_name") {
                  createRandomName();
                  irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                  if (l_pEdit != nullptr)
                    l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sName).c_str());

                  toggleButtonVisibility();
                  m_pGui->setFocus(m_pGui->getRootGUIElement());
                }
                else {
                  if (m_eStep == enMenuStep::Texture) {
                    if (l_sButton == "btn_color_ok") {
                      irr::gui::IGUITab *l_pColor = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                      if (l_pColor != nullptr) {
                        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(m_sColorEdit, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                        if (l_pEdit != nullptr) {
                          std::wstring l_sNewColor = helpers::s2ws(colorToString(getColorFromColorDialog()));
                          std::wstring l_sOldColor = l_pEdit->getText();

                          l_pEdit->setText(l_sNewColor.c_str());

                          if (m_sColorEdit == "texture_bg_nb") {
                            irr::gui::IGUIEditBox *l_pFrame = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("texture_nf", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                            if (l_pFrame != nullptr && l_sOldColor == l_pFrame->getText()) {
                              l_pFrame->setText(l_sNewColor.c_str());
                            }
                          }
                        }

                        m_cPlayer.m_sTexture = helpers::ws2s(getTextureString());
                        updateMarbleTexture(m_cPlayer.m_sTexture);
                      
                        l_pColor->setVisible(false);
                      }

                      m_sColorEdit = "";
                    }
                    else if (l_sButton == "btn_color_cancel") {
                      irr::gui::IGUITab *l_pColor = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                      if (l_pColor != nullptr) {
                        l_pColor->setVisible(false);
                      }
                    }
                    else if (l_sButton == "pick_color") {
                      gui::CReactiveLabel *p = reinterpret_cast<gui::CReactiveLabel *>(a_cEvent.GUIEvent.Caller);
                      irr::video::SColor   c = p->getBackgroundColor();

                      initializeColorDialog(helpers::s2ws(colorToString(c)));
                    }
                    else if (l_sButton == "btn_select_pattern") {
                      m_pPatternDialog = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("pattern_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                      if (m_pPatternDialog != nullptr) {
                        m_pPatternList = reinterpret_cast<gui::CGuiImageList *>(findElementByNameAndType("PatternList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId, m_pGui->getRootGUIElement()));

                        if (m_pPatternList != nullptr) {
                          updatePatterns();
                          m_pPatternDialog->setVisible(true);
                        }
                        else m_pPatternDialog = nullptr;
                      }
                    }
                    else if (l_sButton == "btn_pattern_close") {
                      if (m_pPatternDialog != nullptr) {
                        m_pPatternDialog->setVisible(false);
                        m_pPatternDialog = nullptr;
                      }
                    }
                    else if (l_sButton == "btn_texture_params") {
                      irr::gui::IGUIElement *l_pTab = findElementByName("texturegen_android", m_pGui->getRootGUIElement());
                      if (l_pTab != nullptr)
                        l_pTab->setVisible(true);
                    }
                    else if (l_sButton == "btn_close_texture") {
                      irr::gui::IGUIElement *l_pTab = findElementByName("texturegen_android", m_pGui->getRootGUIElement());
                      if (l_pTab != nullptr)
                        l_pTab->setVisible(false);
                    }
                    else if (l_sButton == "btn_random_texture") {
                      gui::CSelector *l_pMode = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("texture_mode", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                      l_pMode->setSelected(1);

                      irr::SEvent l_cEvent {};
                      l_cEvent.EventType = irr::EET_GUI_EVENT;
                      l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
                      l_cEvent.GUIEvent.Caller    = l_pMode;
                      l_cEvent.GUIEvent.Element   = l_pMode;
                      OnEvent(l_cEvent);

                      if (l_pMode != nullptr) {
                        // Four colors
                        std::vector<std::tuple<std::string, std::string, std::string, std::string>> l_vColors = {
                          std::make_tuple("DED4E8", "E8BA40", "C7395F", ""),
                          std::make_tuple("80C4B7", "EDCBD2", "E3856B", ""),
                          std::make_tuple("E87A5D", "F3B941", "3B5BA5", ""),
                          std::make_tuple("D49BAE", "BBCB50", "678CEC", ""),
                          std::make_tuple("4AAFD5", "91B187", "E7A339", ""),
                          std::make_tuple("E3CCB2", "F9EC7E", "E26274", ""),
                          std::make_tuple("FBEAE7", "B2456E", "552619", ""),
                          std::make_tuple("EDF4F2", "31473A", "7C8363", ""),
                          std::make_tuple("CADCFC", "00246B", "8AB6F9", ""),
                          std::make_tuple("E6C17A", "404041", "F6EDE3", ""),
                          std::make_tuple("E1E5EB", "E59462", "D5CAE4", ""),
                          std::make_tuple("81CAD6", "EDCD44", "DC3E26", ""),
                          std::make_tuple("1803A5", "F2EC9B", "96FFBD", ""),
                          std::make_tuple("D9DAD9", "68A4A5", "4C8055", ""),
                          std::make_tuple("6F9BD1", "DF3C5F", "224193", ""),
                          std::make_tuple("E17888", "AE3B8B", "1C5789", "341514"),
                          std::make_tuple("D0944D", "3988A4", "CB625F", "67C2D4"),
                          std::make_tuple("71B379", "B25690", "EDC400", "1D71BA"),
                          std::make_tuple("D4B8B1", "866C69", "CD8C8C", "53331F"),
                          std::make_tuple("72C2C9", "9FA65A", "2963A2", "4CAABC"),
                          std::make_tuple("D8D0CD", "DF5587", "B46543", "B46543"),
                          std::make_tuple("E58D2E", "4D181C", "144058", "DD671E"),
                          std::make_tuple("9EE8E1", "D2385A", "DE9DC2", "573C33"),
                          std::make_tuple("D1B5A3", "E36858", "D1B5A3", "0C0D0D"),
                          std::make_tuple("CEE6F2", "E9B796", "E3867D", "962E2A"),
                          std::make_tuple("EEC95C", "EECCD3", "E3856B", "80C4B7"),
                          std::make_tuple("57BBBC", "B6818B", "B8912E", "802621"),
                          std::make_tuple("B6E696", "CD7E2A", "6C3622", "6FA1BB"),
                          std::make_tuple("B6E696", "A95EA3", "DC3A79", "1686CD"),
                          std::make_tuple("A4998E", "507B6A", "6A513C", "4B1816"),
                          std::make_tuple("DDDBDE", "CAD4DF", "656E77", "3B373B"),
                          std::make_tuple("6FC7E1", "EABDCF", "EFD557", "CE6EA3"),
                          std::make_tuple("BD5598", "82BB42", "BFCF6E", "DF3C5F"),
                          std::make_tuple("E88659", "D8BF58", "D1BAA2", "56C1E1"),
                          std::make_tuple("3B5BA5", "E87A5C", "469E48", "DE418E"),
                          std::make_tuple("FAEF7C", "E3CCB2", "E26274", "78589F"),
                          std::make_tuple("355952", "EAB63E", "FAF6E7", "E37769"),
                          std::make_tuple("6061A8", "CE8F30", "F4F7F7", "ED3224"),
                          std::make_tuple("3A6D80", "F3CD53", "D56729", "9D402D"),
                          std::make_tuple("735DA5", "D3C5E5", "8EC9BC", "FBF5AA"),
                          std::make_tuple("22235F", "7A4D9F", "EB68A0", "A8DACD"),
                          std::make_tuple("223E8B", "2249AE", "7EE05F", "FEFAAE"),
                          std::make_tuple("F4B0F7", "9CFAD4", "EDF9A2", "F8B0B3"),
                          std::make_tuple("78FFC4", "DCAAE4", "FDC2E4", "FAF3DE"),
                          std::make_tuple("E8338B", "C13979", "5C2C90", "2A2E74"),
                          std::make_tuple("020202", "5351A2", "A254A1", "F6C845"),
                          std::make_tuple("EC6D67", "F2AE7F", "FBF5AE", "CEE4B3"),
                          std::make_tuple("EFC6D4", "D950AE", "AAE847", "EEEDEE"),
                          std::make_tuple("59C4EB", "5ADFDF", "77EFBD", "ECF8BA")
                        };

                        {
                          std::random_device l_cRd { };
                          std::default_random_engine l_cRe { l_cRd() };
                          std::shuffle(l_vColors.begin(), l_vColors.end(), l_cRe);
                        }

                        std::tuple<std::string, std::string, std::string, std::string> l_tColor = *l_vColors.begin();

                        std::vector<std::string> l_vPatterns = {
                          "texture_ant.png",
                          "texture_arrow.png",
                          "texture_atomic.png",
                          "texture_bass.png",
                          "texture_bomb.png",
                          "texture_bowling.png",
                          "texture_circle.png",
                          "texture_diamond.png",
                          "texture_dustbin.png",
                          "texture_explosion.png",
                          "texture_flames.png",
                          "texture_franconia.png",
                          "texture_gun.png",
                          "texture_hammer.png",
                          "texture_hearts.png",
                          "texture_hexagon.png",
                          "texture_hippo.png",
                          "texture_jollyroger.png",
                          "texture_lion.png",
                          "texture_nuclear.png",
                          "texture_plane.png",
                          "texture_pommesgabel.png",
                          "texture_rollin.png",
                          "texture_samurai.png",
                          "texture_skull.png",
                          "texture_spqr.png",
                          "texture_stars.png",
                          "texture_stethoscope.png",
                          "texture_vulture.png"
                        };

                        {
                          std::random_device l_cRd { };
                          std::default_random_engine l_cRe { l_cRd() };
                          std::shuffle(l_vPatterns.begin(), l_vPatterns.end(), l_cRe);
                        }

                        std::string l_sPattern = *l_vPatterns.begin();

                        std::vector<int> l_vIndex = { 0, 1, 2 };

                        std::vector<std::vector<std::string>> l_vElements;

                        if (std::get<3>(l_tColor) == "") {
                          printf("Three Colors!\n");

                          l_vElements.push_back({ "texture_fg_nb", "texture_nr" });
                          l_vElements.push_back({ "texture_fg_pt" });
                          l_vElements.push_back({ "texture_bg_nb", "texture_bg_pt", "texture_nf" });
                        }
                        else {
                          printf("Four Colors!\n");
                          l_vIndex.push_back(3);

                          l_vElements.push_back({ "texture_fg_nb" });
                          l_vElements.push_back({ "texture_nr" });
                          l_vElements.push_back({ "texture_fg_pt" });
                          l_vElements.push_back({ "texture_bg_nb", "texture_bg_pt", "texture_nf" });
                        }

                        {
                          std::random_device l_cRd { };
                          std::default_random_engine l_cRe { l_cRd() };
                          std::shuffle(l_vIndex.begin(), l_vIndex.end(), l_cRe);
                        }

                        std::vector<std::vector<std::string>>::iterator l_itElement = l_vElements.begin();

                        for (std::vector<int>::iterator l_itIndex = l_vIndex.begin(); l_itIndex != l_vIndex.end() && l_itElement != l_vElements.end(); l_itIndex++) {
                          for (std::vector<std::string>::iterator l_itEdit = (*l_itElement).begin(); l_itEdit != (*l_itElement).end(); l_itEdit++) {
                            irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(*l_itEdit, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                            if (l_pEdit != nullptr) {
                              switch (*l_itIndex) {
                                case 0: l_pEdit->setText(helpers::s2ws(std::get<0>(l_tColor)).c_str()); break;
                                case 1: l_pEdit->setText(helpers::s2ws(std::get<1>(l_tColor)).c_str()); break;
                                case 2: l_pEdit->setText(helpers::s2ws(std::get<2>(l_tColor)).c_str()); break;
                                case 3: l_pEdit->setText(helpers::s2ws(std::get<3>(l_tColor)).c_str()); break;
                              }
                            }
                            else printf("%s not found.\n", (*l_itEdit).c_str());
                          }

                          l_itElement++;
                        }

                        irr::gui::IGUIEditBox *l_pPattern = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("texture_pattern", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                        if (l_pPattern != nullptr)
                          l_pPattern->setText(helpers::s2ws(l_sPattern).c_str());

                        m_cPlayer.m_sTexture = helpers::ws2s(getTextureString());
                        updateMarbleTexture(m_cPlayer.m_sTexture);
                      }
                    }
                    else {
                      std::map<std::string, std::string> l_mButtonLinks = {
                        { "btn_select_fg_nb"     , "texture_fg_nb"       },
                        { "btn_select_bg_nb"     , "texture_bg_nb"       },
                        { "btn_select_nr"        , "texture_nr"          },
                        { "btn_select_nf"        , "texture_nf"          },
                        { "btn_select_fg_pt"     , "texture_fg_pt"       },
                        { "btn_select_bg_pt"     , "texture_bg_pt"       },
                        { "btn_select_name_color", "imported_name_color" },
                        { "btn_select_name_back" , "imported_name_back"  }
                      };

                      printf("Button: %s\n", l_sButton.c_str());

                      if (l_mButtonLinks.find(l_sButton) != l_mButtonLinks.end()) {
                        irr::gui::IGUITab     *l_pColor = reinterpret_cast<irr::gui::IGUITab     *>(findElementByNameAndType("color_dialog"           , irr::gui::EGUIET_TAB     , m_pGui->getRootGUIElement()));
                        irr::gui::IGUIEditBox *l_pEdit  = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(l_mButtonLinks[l_sButton], irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                        if (l_pColor != nullptr && l_pEdit != nullptr) {
                          m_sColorEdit = l_mButtonLinks[l_sButton];
                          l_pColor->setVisible(true);
                          initializeColorDialog(l_pEdit->getText());
                        }
                      }
                    }
                  }
                }
              }
              else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
                std::string l_sScrollbar = a_cEvent.GUIEvent.Caller->getName();
                if (l_sScrollbar == "ai_help")
                  updateAiHelpInfo();
                else if (l_sScrollbar == "texture_mode") {
                  updateGeneratedTexture();
                }
                else if (l_sScrollbar == "controller_type") {
                  gui::CSelector      *l_pType  = reinterpret_cast<gui::CSelector      *>(findElementByNameAndType("controller_type", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId  , m_pGui->getRootGUIElement()));
                  irr::gui::IGUIImage *l_pImage = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("controller_img" , irr::gui::EGUIET_IMAGE                          , m_pGui->getRootGUIElement()));
                  gui::CMenuButton    *l_pEdit  = reinterpret_cast<gui::CMenuButton    *>(findElementByNameAndType("editGameCtrl"  , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
                  gui::CMenuButton    *l_pTest  = reinterpret_cast<gui::CMenuButton    *>(findElementByNameAndType("testGameCtrl"  , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

                  if (l_pType != nullptr) {
                    if (l_pImage != nullptr) {
                      unsigned     l_iSelected = l_pType->getSelected();
                      std::wstring l_sSelected = l_iSelected >= 0 && l_iSelected < l_pType->getItemCount() ? l_pType->getItem((int)l_iSelected) : L"";

                      l_pImage->setVisible(false);

                      std::map<std::wstring, std::string> l_mItemMap = {
                        { L"Touch Control"                , "data/images/ctrl_config_touch.png"       },
                        { L"Gyroscope"                    , "data/images/ctrl_config_gyro.png"        }
                      };
                    
                      bool l_bFound = false;

                      for (std::map<std::wstring, std::string>::iterator l_itCtrl = l_mItemMap.begin(); l_itCtrl != l_mItemMap.end(); l_itCtrl++) {
                        if (l_itCtrl->first == l_sSelected) {
                          l_bFound = true;
                          l_pImage->setImage(m_pDrv->getTexture(l_itCtrl->second.c_str()));
                          l_pImage->setVisible(true);
                        }
                      }

                      if (m_pCtrl != nullptr) {
                        m_pCtrl->setVisible(!l_bFound);

                        if (m_cPlayer.m_sControls.substr(0, std::string("DustbinController;").size()) != "DustbinController;") {
                          m_cPlayer.m_sControls = data::c_sDefaultControls;
                          m_pCtrl->setController(m_cPlayer.m_sControls);
                        }
                      }
                      if (l_pEdit != nullptr) l_pEdit->setVisible(!l_bFound);
                      if (l_pTest != nullptr) l_pTest->setVisible(!l_bFound);

                      gui::CDustbinCheckbox *l_pAuto = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("auto_throttle", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement()));

                      if (l_pAuto != nullptr) {
                        l_pAuto->setChecked(m_cPlayer.m_bAutoThrottle || l_bFound);
                        l_pAuto->setEnabled(!l_bFound);
                      }
                    }
                    else {
                      // The control image is only available on Android, and the keyboard controls are only available on Windows
                      // so we assume we are now on Windows
                      std::string l_sType = helpers::ws2s(l_pType->getSelectedItem());

                      if (l_sType == "Keyboard") {
                        m_pCtrl->setControlType(gui::CControllerUi::enControl::Keyboard);
                      }
                      else if (l_sType == "Gamepad") {
                        m_pCtrl->setControlType(gui::CControllerUi::enControl::Joystick);
                      }
                    }
                  }
                }
                else if (m_eStep == enMenuStep::Texture) {
                  if (m_sColorEdit != "") {
                    irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_display", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                    if (l_pTab != nullptr)
                      l_pTab->setBackgroundColor(getColorFromColorDialog());
                  }
                }
              }
              else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED) {
                toggleButtonVisibility();
              }
            }
            else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
              if (a_cEvent.UserEvent.UserData1 == c_iEventImageSelected && a_cEvent.UserEvent.UserData2 == c_iEventImageSelected && m_pPatternList != nullptr) {
                std::string s = m_pPatternList->getSelectedData();

                if (s != "") {
                  irr::gui::IGUIEditBox *l_pPattern = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("texture_pattern", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                  if (l_pPattern != nullptr)
                    l_pPattern ->setText(helpers::s2ws(s).c_str());

                  m_cPlayer.m_sTexture = helpers::ws2s(getTextureString());
                  updateMarbleTexture(m_cPlayer.m_sTexture);

                  m_pPatternDialog->setVisible(false);
                  m_pPatternDialog = nullptr;

                  l_bRet = true;
                }
              }
            }
            else if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
              if (m_pCtrl != nullptr)
                l_bRet = m_pCtrl->OnJoystickEvent(a_cEvent);
            }
          }

          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        bool run() override {
          if (m_eStep == enMenuStep::Texture && m_pMyRtt != nullptr && m_pMySmgr != nullptr) {
            m_pDrv->setRenderTarget(m_pMyRtt, true, true);
            m_pMySmgr->drawAll();
            m_pDrv->setRenderTarget(nullptr, false, false);
          }

          return false;
        }
        /**
        * The callback notified when the controller configuration changes
        * @param a_bEditing is the change because editing is started or not
        */
        virtual void editingController(bool a_bEditing) override {
          printf("Editing: %s\n", a_bEditing ? "true" : "false");
          m_pState->enableMenuController(!a_bEditing);
        }
   };

    IMenuHandler* createMenuProfileWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuProfileWizard(a_pDevice, a_pManager, a_pState);
    }
  }
}