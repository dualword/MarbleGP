// (w) 2021 by Dustbin::Games / Christian Keimel
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
      std::wstring l_sPath = L"/usr/local/games/DustbinGames/MarbleGP";

      std::string l_sCmd = std::string("mkdir -p ") + platform::ws2s(l_sPath);
      system(l_sCmd.c_str());

      return l_sPath;
    }

    const std::wstring portableGetTexturePath() {
      std::wstring l_sPath = portableGetDataPath();
      l_sPath += L"/data/textures/";

      std::string l_sCmd = std::string("mkdir -p ") + platform::ws2s(l_sPath);
      system(l_sCmd.c_str());

      return l_sPath;
    }

    void preventScreenSaver() {
    }

    std::wstring s2ws(const std::string& str) {
      using convert_typeX = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_typeX, wchar_t> converterX;
      return converterX.from_bytes(str);
    }

    std::string ws2s(const std::wstring& wstr) {
      using convert_typeX = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_typeX, wchar_t> converterX;
      return converterX.to_bytes(wstr);
    }

    std::vector<std::string> splitString(const std::string a_sInput, const char a_sDelimiter) {
      std::vector<std::string> l_vRet;

      std::string l_sInput = a_sInput;

      while (l_sInput.size() > 0) {
        size_t l_iPos = l_sInput.find_first_of(a_sDelimiter);

        if (l_iPos != std::string::npos) {
          l_vRet.push_back(l_sInput.substr(0, l_iPos));
          l_sInput = l_sInput.substr(l_iPos + 1);
        }
        else {
          l_vRet.push_back(l_sInput);
          l_sInput = "";
        }
      }

      return l_vRet;
    }
  }
}