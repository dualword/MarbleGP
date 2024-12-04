// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>

namespace dustbin {
  class CMainClass;   /**< Forward declaration of the main class */

  namespace platform {
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen);
    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen);

#ifndef _TRACKED
    void preventScreenSaver();
    void saveSettings();
    void consumeBackEvent(bool a_bConsume);
#endif

    const std::wstring portableGetDataPath   ();
    const std::wstring portableGetTexturePath();
  }
}
