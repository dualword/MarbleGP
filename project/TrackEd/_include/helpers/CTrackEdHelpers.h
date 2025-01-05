// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace helpers {
    /**
    * Add a message to the log
    * @param a_sMessage the message to add
    * @param a_eLevel the level of the log message
    * @param a_iTimeToLive the time to live in milliseconds
    */
    void addLogMessage(const std::wstring &a_sMessage, irr::ELOG_LEVEL a_eLevel, irr::u32 a_iTimeToLive);

    /**
    * Set the "Edit Mode" message
    * @param a_sMessage the new message
    */
    void setEditModeMessage(const std::wstring &a_sMessage);

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::wstring &a_sFile);

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::string &a_sFile);

    /**
    * Post a user event
    * @param a_iUserData1 the user data
    * @param a_iUserData2 additional user data
    */
    void postUserEvent(size_t a_iUserData1, size_t a_iUserData2);

    /**
    * Find the root node for the decoration elements, add it if not presend
    * @param a_pRoot the root node taken from the scene manager
    * @param a_bAddIfNotFound add the decoration node if no match was found?
    * @return the decoration root node
    */
    irr::scene::ISceneNode *getDecorationRootNode(irr::scene::ISceneNode *a_pRoot, bool a_bAddIfNotFound = true);

    /**
    * Find a scene node by it's name
    * @param a_sName the name to search for
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the scene node if found, "nullptr" if no match was found
    */
    irr::scene::ISceneNode *findNodeByName(const std::string &a_sName, irr::scene::ISceneNode *a_pNode);

    /**
    * Find a scene node by it's name and type
    * @param a_sName the name to search for
    * @param a_eType the type of the requested node
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the scene node if found, "nullptr" if no match was found
    */
    irr::scene::ISceneNode *findNodeByNameAndType(const std::string &a_sName, irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pNode);

    /**
    * Find a scene node by it's type
    * @param a_eType the type of the requested node
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the scene node if found, "nullptr" if no match was found
    */
    irr::scene::ISceneNode *findNodeByType(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pNode);

    /**
    * Draw a plane for debugging purposes
    * @param a_pDrv the video driver
    * @param a_cPoint the plane point
    * @param a_cNormal the plane normal
    */
    void drawPlane3df(irr::video::IVideoDriver *a_pDrv, const irr::core::vector3df &a_cPoint, const irr::core::vector3df &a_cNormal);
  }
}

