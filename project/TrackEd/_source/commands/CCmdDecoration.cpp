// (w) 2024 by Dustbin::Games / Christian Keimel
#include <platform/CTrackEdPlatform.h>
#include <commands/CCmdDecoration.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    CCmdDecoration::CCmdDecoration(irr::scene::ISceneManager *a_pSmgr, irr::gui::IGUIEnvironment *a_pGui) :
      ICommand   ("CmdDecoration"),
      m_bFinished(false),
      m_pSmgr    (a_pSmgr),
      m_pGui     (a_pGui),
      m_pBack    (nullptr),
      m_pParent  (nullptr),
      m_pList    (nullptr),
      m_pNode    (nullptr)
    {
      m_pNode = getDecorationRoot(m_pSmgr->getRootSceneNode());

      if (m_pNode == nullptr) {
        m_pNode = m_pSmgr->addEmptySceneNode(m_pSmgr->getRootSceneNode());
        m_pNode->setName("__DecorationRootNode");
      }

      irr::core::dimension2du l_cSize   = m_pGui->getSkin()->getFont()->getDimension(L"ThisTextShouldReallyBeLongEnoughInMyOpinionButIMayBeWrongAsWell");
      irr::core::dimension2du l_cScreen = m_pSmgr->getVideoDriver()->getScreenSize();

      m_pBack = m_pGui->addTab(irr::core::recti(irr::core::vector2di(0), l_cScreen));
      m_pBack->setDrawBackground(true);
      m_pBack->setBackgroundColor(irr::video::SColor(128, 0xFF, 0xFF, 0xFF));

      irr::core::recti l_cDialog = irr::core::recti(
        irr::core::vector2di(l_cScreen.Width / 2 - l_cSize.Width / 2, l_cScreen.Height / 2 - 20 * l_cSize.Height / 2),
        irr::core::dimension2du(l_cSize.Width, 20 * l_cSize.Height)
      );

      m_pParent = m_pGui->addTab(l_cDialog, m_pBack);
      m_pParent->setDrawBackground(true);
      m_pParent->setBackgroundColor(irr::video::SColor(255, 255, 255, 255));

      irr::u32 l_iBtnHeight = 5 * l_cSize.Height / 4;

      irr::gui::IGUIStaticText *l_pHead = m_pGui->addStaticText(
        L"Manage Decoration",
        irr::core::recti(
          0,
          0,
          l_cDialog.getWidth(),
          l_iBtnHeight
        ),
        false,
        false,
        m_pParent
      );

      l_pHead->setBackgroundColor(irr::video::SColor(255, 224, 224, 224));
      l_pHead->setDrawBackground(true);
      l_pHead->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);

      m_pList = m_pGui->addListBox(
        irr::core::recti(
          0, 
          l_iBtnHeight,
          l_cDialog.getWidth (),
          l_cDialog.getHeight() - 2 * l_iBtnHeight
        ),
        m_pParent
      );

      m_pGui->addButton(
        irr::core::recti(
          l_cDialog.getWidth () - l_cSize.Width / 2,
          l_cDialog.getHeight() - l_iBtnHeight,
          l_cDialog.getWidth (),
          l_cDialog.getHeight()
        ),
        m_pParent,
        (irr::s32)enButtonIDs::Close,
        L"Close"
      );

      m_pGui->addButton(
        irr::core::recti(
          0,
          l_cDialog.getHeight() - 2 * l_iBtnHeight,
          l_cDialog.getWidth () / 2,
          l_cDialog.getHeight() - l_iBtnHeight
        ),
        m_pParent,
        (irr::s32)enButtonIDs::Add,
        L"Add Decoration"
      );

      m_pGui->addButton(
        irr::core::recti(
          l_cDialog.getWidth () / 2,
          l_cDialog.getHeight() - 2 * l_iBtnHeight,
          l_cDialog.getWidth (),
          l_cDialog.getHeight() - l_iBtnHeight
        ),
        m_pParent,
        (irr::s32)enButtonIDs::Delete,
        L"Remove Decoration"
      );
    }

    CCmdDecoration::~CCmdDecoration() {
      if (m_pBack != nullptr) {
        m_pBack->setVisible(false);
        m_pGui->addToDeletionQueue(m_pBack);
      }
    }

    /**
    * Find the decoration scene node
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the decoration scene node
    */
    irr::scene::ISceneNode *CCmdDecoration::getDecorationRoot(irr::scene::ISceneNode *a_pNode) {
      if (std::string("__DecorationRootNode") == a_pNode->getName()) {
        return a_pNode;
      }

      for (auto l_pChild: a_pNode->getChildren()) {
        irr::scene::ISceneNode *p = getDecorationRoot(l_pChild);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdDecoration::isFinished() {
      return m_bFinished;
    }

    /**
    * Update the list of decoration objects
    */
    void CCmdDecoration::updateDecorationList() {
      m_pList->clear();

      for (auto l_pChild: m_pNode->getChildren()) {
        std::wstring l_sName = helpers::s2ws(l_pChild->getName());

        wchar_t l_sDelimiter[] = { L'\\', L'/', L'\0' };
        for (int i = 0; l_sDelimiter[i] != L'\0'; i++) {
          if (l_sName.find_last_of(l_sDelimiter[i]) != std::string::npos) {
            l_sName = l_sName.substr(l_sName.find_last_of(l_sDelimiter[i]) + 1);
            break;
          }
        }

        m_pList->addItem(l_sName.c_str());
      }
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdDecoration::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (a_cEvent.GUIEvent.Caller->getID() == (irr::s32)enButtonIDs::Close) {
            m_bFinished = true;
          }
          else if (a_cEvent.GUIEvent.Caller->getID() == (irr::s32)enButtonIDs::Add) {
            tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();


            std::string l_sFile = platform::portableSelectFileOpen(L"Irrlicht Scene File\0*.xml", helpers::s2ws(l_pGlobal->getSetting("load_decoration_folder")).c_str(), L"");

            if (l_sFile != "") {
              std::string l_sName   = l_sFile;
              std::string l_sFolder = l_sFile;

              char l_sDelimiter[] = { '\\', '/', '\0' };

              for (int i = 0; l_sDelimiter[i] != '\0'; i++) {
                if (l_sName.find_last_of(l_sDelimiter[i]) != std::string::npos) {
                  l_sName = l_sName.substr(l_sName.find_last_of(l_sDelimiter[i]) + 1);
                }

                if (l_sFolder.find_last_of(l_sDelimiter[i]) != std::string::npos) {
                  l_sFolder = l_sFolder.substr(0, l_sFolder.find_last_of(l_sDelimiter[i]));
                }
              }

              if (l_sName.find_last_of('.') != std::string::npos)
                l_sName = l_sName.substr(0, l_sName.find_last_of('.'));

              l_pGlobal->setSetting("load_decoration_folder", l_sFolder);
 
              irr::scene::ISceneManager *l_pSmgr = m_pSmgr->createNewSceneManager(false);
              l_pSmgr->loadScene(l_sFile.c_str());

              if (l_pSmgr->getRootSceneNode()->getChildren().size() > 0) {
                irr::scene::ISceneNode *l_pNode  = *l_pSmgr->getRootSceneNode()->getChildren().begin();
                irr::scene::ISceneNode *l_pClone = l_pNode->clone(m_pNode, m_pSmgr);
                l_pClone->setName(l_sFile.c_str());
              }

              helpers::postUserEvent(c_iUpdateShader     , c_iUpdateShader);
              helpers::postUserEvent(c_iUpdateDecorations, c_iUpdateDecorations);
              l_pSmgr->drop();

              updateDecorationList();
            }
          }
        }
      }

      return l_bRet;
    }
  }
}