// (w) 2021 by Dustbin::Games / Christian Keimel

#ifdef _RASPI

#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <filesystem>
#include <CGlobal.h>
#include <stdlib.h>
#include <cstring>
#include <codecvt>
#include <stdio.h>
#include <fstream>
#include <locale>
#include <ctime>

namespace dustbin {
  namespace platform {

    void portableItoa(int a_iValue, char* a_sBuffer, int a_iLen) {
      sprintf(a_sBuffer, "%d", a_iValue);
    }

    void portableDateStr(long a_iTime, char* a_sBuffer, int a_iLen) {
      std::tm* ptm;
      time_t l_iDummy = a_iTime;
      ptm = std::localtime(&l_iDummy);
      std::strftime(a_sBuffer, a_iLen, "%d %b %Y", ptm);
    }

    const std::string portableGetFontPath() {
      return "/usr/share/fonts/truetype/freefont/";
    }

    const std::wstring portableGetDataPath() {
      std::string l_sPath = getenv("HOME");
      l_sPath += "/.local/share/DustbinGames/MarbleGP/";
      std::filesystem::create_directories(l_sPath.c_str());
      
      return helpers::s2ws(l_sPath.c_str());
    }

    const std::wstring portableGetTexturePath() {
      std::wstring l_sPath = portableGetDataPath();
      l_sPath += L"data/textures/";

      std::filesystem::create_directories(l_sPath.c_str());

      return l_sPath;
    }

    void preventScreenSaver() {
    }
    
    const std::wstring portableGetTrackEdPath() {
      std::wstring l_sPath = portableGetDataPath() + L"TrackEd/";
      std::filesystem::create_directories(l_sPath.c_str());
      
      return l_sPath;
    }
    
    const std::wstring portableGetTrackPackPath() {
      std::wstring l_sPath = portableGetDataPath() + L"TrackPack/";
      std::filesystem::create_directories(l_sPath.c_str());
      
      return l_sPath;
    }

    const std::string portableGetDownloadPath() {
      std::string l_sPath = getenv("HOME");
      l_sPath += "/Downloads/";
      
      return l_sPath;
    }
    
    void saveSettings() {
      std::string l_sFile = helpers::ws2s(portableGetDataPath()) + "MarbleGP_Setup.xml";
      
      std::ofstream l_cFile(l_sFile);
      l_cFile << CGlobal::getInstance()->getSettings();
      l_cFile.close();
    }
    
    void consumeBackEvent(bool a_bConsume) {
    }
  }
}

#endif
