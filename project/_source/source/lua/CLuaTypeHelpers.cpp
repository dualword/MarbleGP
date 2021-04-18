// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaTypeHelpers.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    
    SVector3d irrVectorToLua(const irr::core::vector3df& a_cInput) {
      SVector3d l_cRet;
      l_cRet.m_x = a_cInput.X;
      l_cRet.m_y = a_cInput.Y;
      l_cRet.m_z = a_cInput.Z;
      return l_cRet;
    }

    irr::core::vector3df luaVectorToIrr(const SVector3d& a_cInput) {
      irr::core::vector3df l_cRet;
      l_cRet.X = a_cInput.m_x;
      l_cRet.Y = a_cInput.m_y;
      l_cRet.Z = a_cInput.m_z;
      return l_cRet;
    }

    /**
    * Convert Irrlicht color information to LUA
    * @param a_cInput Irrlicht color information
    * @return LUA color information
    */
    SColor irrColorToLua(const irr::video::SColor& a_cInput) {
      SColor l_cRet;
      l_cRet.m_a = a_cInput.getAlpha();
      l_cRet.m_r = a_cInput.getRed  ();
      l_cRet.m_g = a_cInput.getGreen();
      l_cRet.m_b = a_cInput.getBlue ();
      return l_cRet;
    }

    /**
    * Convert LUA color information to Irrlicht
    * @param a_cInput Irrlicht color information
    * @return LUA color information
    */
    irr::video::SColor luaColorToIrr(const SColor a_cInput) {
      return irr::video::SColor(a_cInput.m_a, a_cInput.m_r, a_cInput.m_g, a_cInput.m_b);
    }

    /**
    * Convert a string to LUA color information
    * @param a_sInput hex color string (AARRGGBB). If the string is less than 
    * eight characters long alpha will be FF, all colors will be filled with 0
    */
    SColor colorStringToLua(const std::string& a_sColor) {
      SColor l_cRet;

      std::string l_sColor = a_sColor;
      while (l_sColor.size() < 6) l_sColor += "0";
      while (l_sColor.size() < 8) l_sColor = "F" + l_sColor;

      char *p;

      l_cRet.m_a = std::strtol(l_sColor.substr(0, 2).c_str(), &p, 16);
      l_cRet.m_r = std::strtol(l_sColor.substr(2, 2).c_str(), &p, 16);
      l_cRet.m_g = std::strtol(l_sColor.substr(4, 2).c_str(), &p, 16);
      l_cRet.m_b = std::strtol(l_sColor.substr(6, 2).c_str(), &p, 16);

      return l_cRet;
    }

    /**
    * Convert a lua color to a string (AARRGGBB)
    * @param a_cInput the lua color to encode
    * @return the color encoded as a string
    */
    std::string luaColorToString(const SColor a_cInput) {
      std::string l_sRet = "";

      auto encodeMember = [&](int a_iMember) {
        char s[0xFF];
        sprintf_s(s, 0xFF, "%2X", a_iMember);
        std::string l_sAdd = s;
        while (l_sAdd.size() < 2) l_sAdd = "0" + l_sAdd;
        l_sRet += l_sAdd;
      };

      encodeMember(a_cInput.m_a);
      encodeMember(a_cInput.m_r);
      encodeMember(a_cInput.m_g);
      encodeMember(a_cInput.m_b);

      return l_sRet;
    }

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

          std::memset(l_sData, 0, l_pLUA->getSize() + 1);
          l_pLUA->read(l_sData, l_pLUA->getSize());
          std::string l_sScript = l_sData;
          l_pLUA->drop();
          delete []l_sData;

          return l_sScript;
        }
      }
      else return "";
    }
  }
}
