// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <vector>
#include <string>

namespace dustbin {
  namespace platform {
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen);
    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen);
    void preventScreenSaver();

    const std::wstring portableGetDataPath   ();
    const std::string  portableGetFontPath   ();
    const std::wstring portableGetTexturePath();

    std::wstring s2ws(const std::string& str);
    std::string ws2s(const std::wstring& wstr);

    std::vector<std::string> splitString(const std::string a_sInput, const char a_sDelimiter);
  }
}
