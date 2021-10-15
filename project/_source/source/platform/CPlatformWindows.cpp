// (w) 2021 by Dustbin::Games / Christian Keimel
#include <Windows.h>

#include <irrlicht/irrlicht.h>
#include <filesystem>
#include <shellapi.h>
#include <winerror.h>
#include <CGlobal.h>
#include <shlwapi.h>
#include <direct.h>
#include <stdlib.h>
#include "shlobj.h"
#include <cstring>
#include <codecvt>
#include <tchar.h>
#include <time.h>
#include <string>
#include <vector>
#include <ctime>

namespace dustbin {
  namespace platform {
    void portableItoa(int a_iValue, char *a_sBuffer, int a_iLen) {
      _itoa_s(a_iValue, a_sBuffer, a_iLen, 10);
    }

    void portableDateStr(long a_iTime, char *a_sBuffer, int a_iLen) {
      std::tm ptm;
      __time32_t l_iDummy = a_iTime;
      _localtime32_s(&ptm, &l_iDummy);
      std::strftime(a_sBuffer, a_iLen, "%d %b %Y", &ptm);
    }

    const std::string portableGetFontPath() {
      char l_sPath[MAX_PATH] = "";
      SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, l_sPath);
      return l_sPath;
    }

    const std::wstring portableGetDataPath() {
      TCHAR l_sPath       [MAX_PATH],
            l_sDataPath   [MAX_PATH],
            l_sTexturePath[MAX_PATH];

      if (SUCCEEDED(SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, l_sPath))) {
        PathAppend(l_sPath, L"\\DustbinGames\\");
        irr::io::path l_cPath = irr::io::path(l_sPath);

        if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);

        PathAppend(l_sPath, L"\\MarbleGP\\");

        if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);

        _tcscpy_s(l_sDataPath, MAX_PATH, l_sPath);
        PathAppend(l_sDataPath, L"\\data\\");

        if (!PathFileExists(l_sDataPath)) CreateDirectory(l_sDataPath, NULL);

        _tcscpy_s(l_sTexturePath, MAX_PATH, l_sDataPath);
        PathAppend(l_sTexturePath, L"\\textures\\");

        if (!PathFileExists(l_sTexturePath)) CreateDirectory(l_sTexturePath, NULL);

        /*_tcscpy_s(l_sReplay, MAX_PATH, l_sPath);
        PathAppend(l_sReplay, L"\\Replay\\");

        if (!PathFileExists(l_sReplay)) CreateDirectory(l_sReplay, NULL);

        _tcscpy_s(l_sLua, MAX_PATH, l_sPath);
        PathAppend(l_sLua, L"\\lua\\");

        if (!PathFileExists(l_sLua)) CreateDirectory(l_sLua, NULL);

        _tcscpy_s(l_sData, MAX_PATH, l_sPath);
        PathAppend(l_sData, L"\\data\\");

        _tcscpy_s(l_sLua, MAX_PATH, l_sPath);
        PathAppend(l_sLua, L"\\data\\cup_definitions");

        if (!PathFileExists(l_sData)) CreateDirectory(l_sData, NULL);

        _tcscpy_s(l_sTexture, MAX_PATH, l_sPath);
        PathAppend(l_sTexture, L"\\textures\\");

        if (!PathFileExists(l_sTexture)) CreateDirectory(l_sTexture, NULL);

        _tcscpy_s(l_sTracks, MAX_PATH, l_sPath);

        PathAppend(l_sImport, L"\\imported_tracks\\");
        if (!PathFileExists(l_sImport)) CreateDirectory(l_sImport, NULL);*/
      }

      return l_sPath;
    }

    const std::wstring portableGetTexturePath() {
      std::wstring l_sPath = portableGetDataPath();
      l_sPath += L"\\data\\textures\\";

      if (!PathFileExists(l_sPath.c_str())) CreateDirectory(l_sPath.c_str(), NULL);

      return l_sPath;
    }

    void preventScreenSaver() {
      SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
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
