// (w) 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdMainMenu.h>
#include <IGlobal.h>

namespace dustbin {
  namespace helpers {
    /**
    * Add a message to the log
    * @param a_sMessage the message to add
    * @param a_eLevel the level of the log message
    * @param a_iTimeToLive the time to live in milliseconds
    */
    void addLogMessage(const std::wstring &a_sMessage, irr::ELOG_LEVEL a_eLevel, irr::u32 a_iTimeToLive) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        commands::CCmdEditTrack *l_pEdit = l_pGlobal->getEditTrackCommand();
        if (l_pEdit != nullptr) {
          commands::CCmdMainMenu *l_pMain = l_pEdit->getMainMenu();
          if (l_pMain != nullptr) {
            l_pMain->addLogMessage(a_sMessage, a_eLevel, a_iTimeToLive);
          }
        }
      }
    }

    /**
    * Set the "Edit Mode" message
    * @param a_sMessage the new message
    */
    void setEditModeMessage(const std::wstring &a_sMessage) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        commands::CCmdEditTrack *l_pEdit = l_pGlobal->getEditTrackCommand();
        if (l_pEdit != nullptr) {
          commands::CCmdMainMenu *l_pMain = l_pEdit->getMainMenu();
          if (l_pMain != nullptr) {
            l_pMain->setEditMode(a_sMessage);
          }
        }
      }
    }

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::wstring &a_sFile) {
      wchar_t l_sDelimiter[] = { L'/', L'\\', L'\0' };

      for (int i = 0; l_sDelimiter[i] != L'\0'; i++) {
        if (a_sFile.find_last_of(l_sDelimiter[i]) != std::string::npos) {
          return a_sFile.substr(a_sFile.find_last_of(l_sDelimiter[i]) + 1);
        }
      }

      return a_sFile;
    }

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::string &a_sFile) {
      char l_sDelimiter[] = { '/', '\\', '\0' };

      for (int i = 0; l_sDelimiter[i] != L'\0'; i++) {
        if (a_sFile.find_last_of(l_sDelimiter[i]) != std::string::npos) {
          return helpers::s2ws(a_sFile.substr(a_sFile.find_last_of(l_sDelimiter[i]) + 1));
        }
      }

      return helpers::s2ws(a_sFile);
    }

    /**
    * Post a user event
    * @param a_iUserData the user data
    */
    void postUserEvent(size_t a_iUserData1, size_t a_iUserData2) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_USER_EVENT;
        l_cEvent.UserEvent.UserData1 = a_iUserData1;
        l_cEvent.UserEvent.UserData2 = a_iUserData2;
        l_pGlobal->getDevice()->postEventFromUser(l_cEvent);
      }
    }

    irr::scene::ISceneNode *findDecoration(irr::scene::ISceneNode *a_pNode) {
      if (std::string(a_pNode->getName()) == "__DecorationRootNode")
        return a_pNode;

      for (auto l_pChild: a_pNode->getChildren()) {
        irr::scene::ISceneNode *p = findDecoration(l_pChild);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
    * Find the root node for the decoration elements, add it if not presend
    * @param a_pRoot the root node taken from the scene manager
    * @return the decoration root node
    */
    irr::scene::ISceneNode *getDecorationRootNode(irr::scene::ISceneNode *a_pRoot, bool a_bAddIfNotFound) {
      irr::scene::ISceneNode *l_pRet = findDecoration(a_pRoot);

      if (l_pRet == nullptr && a_bAddIfNotFound) {
        l_pRet = a_pRoot->getSceneManager()->addEmptySceneNode();
        l_pRet->setName("__DecorationRootNode");
      }

      return l_pRet;
    }

    /**
    * Find a scene node by it's name
    * @param a_sName the name to search for
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the scene node if found, "nullptr" if no match was found
    */
    irr::scene::ISceneNode *findNodeByName(const std::string &a_sName, irr::scene::ISceneNode *a_pNode) {
      if (a_sName == a_pNode->getName())
        return a_pNode;

      for (auto l_pChild: a_pNode->getChildren()) {
        irr::scene::ISceneNode *p = findNodeByName(a_sName, l_pChild);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
    * Find a scene node by it's name
    * @param a_sName the name to search for
    * @param a_eType the type of the requested node
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the scene node if found, "nullptr" if no match was found
    */
    irr::scene::ISceneNode *findNodeByNameAndType(const std::string &a_sName, irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pNode) {
      if (a_sName == a_pNode->getName() && a_eType == a_pNode->getType())
        return a_pNode;

      for (auto l_pChild: a_pNode->getChildren()) {
        irr::scene::ISceneNode *p = findNodeByNameAndType(a_sName, a_eType, l_pChild);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
    * Draw a plane for debugging purposes
    * @param a_pDrv the video driver
    * @param a_cPoint the plane point
    * @param a_cNormal the plane normal
    */
    void drawPlane3df(irr::video::IVideoDriver *a_pDrv, const irr::core::vector3df &a_cPoint, const irr::core::vector3df &a_cNormal) {
      irr::core::vector3df v1;
      irr::core::vector3df v2;

      if (a_cNormal.Y == 1.0f) {
        v1 = irr::core::vector3df(1.0f, 0.0f, 0.0f);
        v2 = irr::core::vector3df(0.0f, 0.0f, 1.0f);
      }
      else if (a_cNormal.X == 1.0f) {
        v1 = irr::core::vector3df(0.0f, 1.0f, 0.0f);
        v2 = irr::core::vector3df(0.0f, 0.0f, 1.0f);
      }
      else if (a_cNormal.Z == 1.0f) {
        v1 = irr::core::vector3df(0.0f, 1.0f, 0.0f);
        v2 = irr::core::vector3df(1.0f, 0.0f, 0.0f);
      }
      else {
        irr::f32 s = 1.0f / (a_cNormal.X * a_cNormal.X + a_cNormal.Y * a_cNormal.Y + a_cNormal.Z * a_cNormal.Z);
        irr::f32 v3x = s * a_cNormal.X;
        irr::f32 v3y = s * a_cNormal.Y;
        irr::f32 v3z = s * a_cNormal.Z;

        // Calculate v1.
        s = 1.0f / (v3x * v3x + v3z * v3z);
        irr::f32 v1x = s * v3z;
        irr::f32 v1y = 0.0f;
        irr::f32 v1z = s * -v3x;

        // Calculate v2 as cross product of v3 and v1.
        // Since v1y is 0, it could be removed from the following calculations. Keeping it for consistency.
        irr::f32 v2x = v3y * v1z - v3z * v1y;
        irr::f32 v2y = v3z * v1x - v3x * v1z;
        irr::f32 v2z = v3x * v1y - v3y * v1x;

        v1 = irr::core::vector3df(0.0f);
        v2 = irr::core::vector3df(1.0f);

        for (irr::f32 f = 0.0f; f < 180.0f; f += 0.5f) {
          irr::core::vector3df l_cDummy = irr::core::vector3df(
            (irr::f32)(v1x * cos(f) + v2x * sin(f)),
            (irr::f32)(v1y * cos(f) + v2y * sin(f)),
            (irr::f32)(v1z * cos(f) + v2z * sin(f))          
          );

          if (l_cDummy.Y > v1.Y)
            v1 = l_cDummy;

          if (l_cDummy.Y >= 0.0f && l_cDummy.Y < v2.Y)
            v2 = l_cDummy;
        }
      }

      v1 = v1.normalize();
      v2 = v2.normalize();

      for (irr::f32 y = -50.0f; y <= 50.0f; y += 5.0f) {
        irr::core::vector3df p1 = a_cPoint + y * v1 - 50.0f * v2;
        irr::core::vector3df p2 = a_cPoint + y * v1 + 50.0f * v2;

        a_pDrv->draw3DLine(p1, p2, irr::video::SColor(0xFF, 0, 0, 0));
      }

      for (irr::f32 x = -50.0f; x <= 50.0f; x += 5.0f) {
        irr::core::vector3df p1 = a_cPoint + x * v2 - 50.0f * v1;
        irr::core::vector3df p2 = a_cPoint + x * v2 + 50.0f * v1;

        a_pDrv->draw3DLine(p1, p2, irr::video::SColor(0xFF, 0, 0, 0));
      }    
    }
  }
}

