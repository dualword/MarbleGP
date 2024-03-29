// (w) 2021 by Dustbin::Games / Christian Keimel

#ifdef _LINUX

#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <stdlib.h>
#include <cstring>
#include <codecvt>
#include <stdio.h>
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
      return L"./";
    }

    const std::wstring portableGetTexturePath() {
      std::wstring l_sPath = portableGetDataPath();
      l_sPath += L"/data/textures/";

      std::string l_sCmd = std::string("mkdir -p ") + helpers::ws2s(l_sPath);
      system(l_sCmd.c_str());

      return l_sPath;
    }

    void preventScreenSaver() {
    }
  }
}

#endif