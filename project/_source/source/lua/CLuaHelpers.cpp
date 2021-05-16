// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaHelpers.h>

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

    /**
    * Convert a SVector3 lua structure to an irrlicht vector
    * @param a_cIn the lua table
    * @return the Irrlicht vector
    */
    irr::core::vector3df convertVectorToIrr(const SVector3d& a_cIn) {
      return irr::core::vector3df(a_cIn.m_x, a_cIn.m_y, a_cIn.m_z);
    }

    /**
    * Convert an Irrlicht vector to a lua SVector3
    * @param a_cIn the Irrlicht vector
    * @return the lua table
    */
    SVector3d convertVectorToLua(const irr::core::vector3df& a_cIn) {
      SVector3d l_cRet;
      l_cRet.m_x = a_cIn.X;
      l_cRet.m_y = a_cIn.Y;
      l_cRet.m_z = a_cIn.Z;
      return l_cRet;
    }

    /**
    * Convert a SColor lua structor to an irrlicht color object
    * @param a_cIn the lua table
    * @return the irrlicht color
    */
    irr::video::SColor convertColorToIrr(const SColor& a_cIn) {
      return irr::video::SColor(a_cIn.m_a, a_cIn.m_r, a_cIn.m_g, a_cIn.m_b);
    }

    /**
    * Convert an Irrlicht color to a lua SColor
    * @param a_cIn the irrlicht color
    * @return a SColor lua structure
    */
    SColor convertColorToLua(const irr::video::SColor& a_cIn) {
      SColor l_cRet;
      l_cRet.m_a = a_cIn.getAlpha();
      l_cRet.m_r = a_cIn.getRed  ();
      l_cRet.m_g = a_cIn.getGreen();
      l_cRet.m_b = a_cIn.getBlue ();
      return l_cRet;
    }
  }
}