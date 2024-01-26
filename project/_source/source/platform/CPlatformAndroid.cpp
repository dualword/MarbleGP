// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <irrlicht.h>
#include <CGlobal.h>
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
      std::wstring l_sPath = helpers::s2ws(CGlobal::getInstance()->getAndroidApp()->activity->internalDataPath) + L"/";
      return l_sPath;
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