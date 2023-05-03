// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>

namespace dustbin {
  class CMainClass;   /**< Forward declaration of the main class */

  namespace platform {
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen);
    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen);
    void preventScreenSaver();
    void saveSettings();
    void consumeBackEvent(bool a_bConsume);

    const std::wstring portableGetDataPath   ();
    const std::wstring portableGetTexturePath();
  }
}
