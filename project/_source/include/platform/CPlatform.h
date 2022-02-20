// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>

namespace dustbin {
  namespace platform {
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen);
    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen);
    void preventScreenSaver();

    const std::wstring portableGetDataPath   ();
    const std::wstring portableGetTexturePath();
  }
}
