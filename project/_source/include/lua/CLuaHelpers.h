// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace lua {
    /**
    * Load a LUA script from the file system and return the content as string
    * @param a_sFileName filename of the LUA script
    * @return the content of the script file
    */
    std::string loadLuaScript(const std::string& a_sFileName) {
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

      if (l_pFs->existFile(a_sFileName.c_str())) {
        irr::io::IReadFile *l_pLUA = l_pFs->createAndOpenFile(a_sFileName.c_str());
        if (l_pLUA != nullptr) {
          char *l_sData = new char[l_pLUA->getSize() + 1];

    #ifdef _LINUX
          memset(l_sData, 0, l_pLUA->getSize() + 1);
    #else
          std::memset(l_sData, 0, l_pLUA->getSize() + 1);
    #endif
          l_pLUA->read(l_sData, l_pLUA->getSize());
          std::string l_sScript = l_sData;
          l_pLUA->drop();
          delete []l_sData;

          return l_sScript;
        }
      }

      return "";
    }
  }
}
