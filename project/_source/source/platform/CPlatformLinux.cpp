/*
(w) 2016 - 2018 by Christian Keimel / https://www.bulletbyte.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#include <platform/CPlatform.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>

namespace dustbin {
  namespace platform {
    
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen) {
      sprintf(a_sBuffer, "%d", a_iValue);
    }

    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen) {
      std::tm *ptm;
      time_t l_iDummy = a_iTime;
      ptm = std::localtime(&l_iDummy);
      std::strftime(a_sBuffer, a_iLen, "%d %b %Y", ptm);
    }

    void portableOpenUrl(const char *a_sUrl) {
    }

    void portableFocusWindow(irr::video::IVideoDriver *a_pDrv) {
    }

    void portableGetDataPaths(char *a_sRoot, char *a_sGhosts, char *a_sReplay) {
      sprintf(a_sRoot  , "."); //~/.DustbinGames/marbles3");
      sprintf(a_sGhosts, "."); //"~/.DustbinGames/marbles3/ghosts");
      sprintf(a_sReplay, "."); //"~/.DustbinGames/marbles3/replay");
    }
  }
}