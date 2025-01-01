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
  }
}

