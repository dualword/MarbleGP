/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
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

    void portableOpenUrl(const std::wstring &a_sUrl) {
      ShellExecute(NULL, L"open",  a_sUrl.c_str(), 0, 0 , SW_SHOWNORMAL);
    }

    void portableFocusWindow(irr::video::IVideoDriver *a_pDrv) {
      HWND l_cHwnd = reinterpret_cast<HWND>(a_pDrv->getExposedVideoData().OpenGLWin32.HWnd);
      BringWindowToTop(l_cHwnd);
      SetForegroundWindow(l_cHwnd);
    }

    bool portableIsFolder(const std::string &a_sPath) {
      DWORD l_iType = GetFileAttributesA(a_sPath.c_str());
      if (l_iType == INVALID_FILE_ATTRIBUTES)
        return false;

      if (l_iType & FILE_ATTRIBUTE_DIRECTORY)
        return true;

      return false;
    }

    void portableCopyFile(const std::wstring &a_sSource, const std::wstring &a_sTarget) {
      CopyFile(a_sSource.c_str(), a_sTarget.c_str(), false);
    }

    void portableCreateFolder(const std::wstring &a_sFolder) {
      _wmkdir(a_sFolder.c_str());
    }

    void portableGetImagePath(std::wstring &a_sPath) {
      wchar_t l_sPath[MAX_PATH];
      HRESULT result = SHGetFolderPath(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, l_sPath);
      a_sPath = l_sPath;
      if (a_sPath.back() != '/' && a_sPath.back() != '\\')
        a_sPath += '/';
    }

    const std::string portableGetFontPath() {
      char l_sPath[MAX_PATH] = "";
      SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, l_sPath);
      return l_sPath;
    }

    const std::wstring portableGetDataPath() {
      TCHAR l_sPath   [MAX_PATH],
        l_sGhost  [MAX_PATH],
        l_sReplay [MAX_PATH],
        l_sLua    [MAX_PATH],
        l_sData   [MAX_PATH],
        l_sTexture[MAX_PATH],
        l_sTracks [MAX_PATH],
        l_sImport [MAX_PATH];

      if (SUCCEEDED(SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, l_sPath))) {
        PathAppend(l_sPath, L"\\DustbinGames\\");
        irr::io::path l_cPath = irr::io::path(l_sPath);

        if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);

        PathAppend(l_sPath, L"\\MarbleGP\\");

        if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);

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

    unsigned long long int portableGetLastModification(const std::wstring &a_sFile) {
      WIN32_FILE_ATTRIBUTE_DATA l_cData;
      GetFileAttributesEx(a_sFile.c_str(), GetFileExInfoStandard, &l_cData);

      SYSTEMTIME l_cSt;
      FILETIME   l_cFt;

      l_cFt.dwHighDateTime = l_cData.ftLastWriteTime.dwHighDateTime;
      l_cFt.dwLowDateTime  = l_cData.ftLastWriteTime.dwLowDateTime;

      FileTimeToSystemTime(&l_cFt, &l_cSt);

      tm l_cTime;

      l_cTime.tm_sec  = l_cSt.wSecond;
      l_cTime.tm_min  = l_cSt.wMinute;
      l_cTime.tm_hour = l_cSt.wHour;
      l_cTime.tm_mday = l_cSt.wDay;
      l_cTime.tm_mon  = l_cSt.wMonth - 1;
      l_cTime.tm_year = l_cSt.wYear - 1900;

      return (unsigned long long int)mktime(&l_cTime);
    }

    void preventScreenSaver() {
      SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
    }

    std::vector<std::string> getDrives() {
      std::vector<std::string> l_vRet;

      DWORD l_iDrives = GetLogicalDrives();

      for (unsigned i = 0; i < 32; i++) {
        DWORD l_iFlag = 1 << i;
        if ((l_iDrives & l_iFlag) != 0) {
          std::string l_sDrive(1, (char)('A' + i));
          l_sDrive += ":\\";
          l_vRet.push_back(l_sDrive);
        }
      }

      return l_vRet;
    }

    irr::u64 portableGetFileDate(const std::wstring &a_sFile) {
      HANDLE l_hFile = CreateFile(a_sFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

      if (l_hFile == INVALID_HANDLE_VALUE) {
        printf("Invalid file hande  (%S)\n", a_sFile.c_str());
      }
      else {
        FILETIME l_cCreate, l_cAccess, l_cWrite;
        if (GetFileTime(l_hFile, &l_cCreate, &l_cAccess, &l_cWrite)) {
          ULARGE_INTEGER l_iRet;
          l_iRet.HighPart = l_cWrite.dwHighDateTime;
          l_iRet.LowPart  = l_cWrite.dwLowDateTime;

          return l_iRet.QuadPart;
        }
        else {
          printf("Cannot query file time (%S)\n", a_sFile.c_str());
          return 0;
        }
      }

      return 0;
    }

    const std::wstring portableOpenFileDialog(const std::wstring &a_sFilter, const std::wstring &a_sInitialDir, const std::wstring &a_sTitle) {
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();
      irr::io::path l_sPath = l_pFs->getWorkingDirectory();

      OPENFILENAME l_cOf;
      wchar_t l_sFileName[MAX_PATH];
      ZeroMemory(l_sFileName, sizeof(wchar_t) * MAX_PATH);
      ZeroMemory(&l_cOf, sizeof(l_cOf));

      l_cOf.lStructSize = sizeof(l_cOf);
      l_cOf.hwndOwner = nullptr;
      l_cOf.lpstrFilter = a_sFilter.c_str();
      l_cOf.lpstrFile = l_sFileName;
      l_cOf.nFilterIndex = 1;
      l_cOf.nMaxFile = MAX_PATH;
      l_cOf.Flags = OFN_DONTADDTORECENT | OFN_EXPLORER | OFN_FILEMUSTEXIST;
      l_cOf.lpstrTitle = a_sTitle.c_str();
      l_cOf.lpstrInitialDir = a_sInitialDir.c_str();

      try {
        if (GetOpenFileName(&l_cOf)) {
          l_pFs->changeWorkingDirectoryTo(l_sPath);
          return std::wstring(l_sFileName);
        }
        else {
          l_pFs->changeWorkingDirectoryTo(l_sPath);
          switch (CommDlgExtendedError()) {
          case CDERR_DIALOGFAILURE   : throw "CDERR_DIALOGFAILURE\n"  ; break;
          case CDERR_FINDRESFAILURE  : throw "CDERR_FINDRESFAILURE\n" ; break;
          case CDERR_INITIALIZATION  : throw "CDERR_INITIALIZATION\n" ; break;
          case CDERR_LOADRESFAILURE  : throw "CDERR_LOADRESFAILURE\n" ; break;
          case CDERR_LOADSTRFAILURE  : throw "CDERR_LOADSTRFAILURE\n" ; break;
          case CDERR_LOCKRESFAILURE  : throw "CDERR_LOCKRESFAILURE\n" ; break;
          case CDERR_MEMALLOCFAILURE : throw "CDERR_MEMALLOCFAILURE\n"; break;
          case CDERR_MEMLOCKFAILURE  : throw "CDERR_MEMLOCKFAILURE\n" ; break;
          case CDERR_NOHINSTANCE     : throw "CDERR_NOHINSTANCE\n"    ; break;
          case CDERR_NOHOOK          : throw "CDERR_NOHOOK\n"         ; break;
          case CDERR_NOTEMPLATE      : throw "CDERR_NOTEMPLATE\n"     ; break;
          case CDERR_STRUCTSIZE      : throw "CDERR_STRUCTSIZE\n"     ; break;
          case FNERR_BUFFERTOOSMALL  : throw "FNERR_BUFFERTOOSMALL\n" ; break;
          case FNERR_INVALIDFILENAME : throw "FNERR_INVALIDFILENAME\n"; break;
          case FNERR_SUBCLASSFAILURE : throw "FNERR_SUBCLASSFAILURE\n"; break;
          default                    : throw "You cancelled.\n";
          }
        }
      }
      catch (std::string& l_sException) {
        l_pFs->changeWorkingDirectoryTo(l_sPath);
        std::string s = l_sException;
        if (s != "You cancelled.\n")
          throw s;
        else
          return L"";
      }
    }

    const std::wstring portableSaveFileDialog(const std::wstring &a_sFilter, const std::wstring &a_sInitialDir, const std::wstring &a_sTitle) {
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();
      irr::io::path l_sPath = l_pFs->getWorkingDirectory();

      OPENFILENAME l_cOf;
      wchar_t l_sFileName[MAX_PATH];
      ZeroMemory(l_sFileName, sizeof(wchar_t) * MAX_PATH);
      ZeroMemory(&l_cOf, sizeof(l_cOf));

      l_cOf.lStructSize = sizeof(l_cOf);
      l_cOf.hwndOwner = nullptr;
      l_cOf.lpstrFilter = a_sFilter.c_str();
      l_cOf.lpstrFile = l_sFileName;
      l_cOf.nFilterIndex = 1;
      l_cOf.nMaxFile = MAX_PATH;
      l_cOf.Flags = OFN_DONTADDTORECENT | OFN_EXPLORER;
      l_cOf.lpstrTitle = a_sTitle.c_str();
      l_cOf.lpstrInitialDir = a_sInitialDir.c_str();

      try {
        if (GetOpenFileName(&l_cOf)) {
          l_pFs->changeWorkingDirectoryTo(l_sPath);
          return l_sFileName;
        }
        else {
          l_pFs->changeWorkingDirectoryTo(l_sPath);
          switch (CommDlgExtendedError()) {
          case CDERR_DIALOGFAILURE   : throw "CDERR_DIALOGFAILURE\n"  ; break;
          case CDERR_FINDRESFAILURE  : throw "CDERR_FINDRESFAILURE\n" ; break;
          case CDERR_INITIALIZATION  : throw "CDERR_INITIALIZATION\n" ; break;
          case CDERR_LOADRESFAILURE  : throw "CDERR_LOADRESFAILURE\n" ; break;
          case CDERR_LOADSTRFAILURE  : throw "CDERR_LOADSTRFAILURE\n" ; break;
          case CDERR_LOCKRESFAILURE  : throw "CDERR_LOCKRESFAILURE\n" ; break;
          case CDERR_MEMALLOCFAILURE : throw "CDERR_MEMALLOCFAILURE\n"; break;
          case CDERR_MEMLOCKFAILURE  : throw "CDERR_MEMLOCKFAILURE\n" ; break;
          case CDERR_NOHINSTANCE     : throw "CDERR_NOHINSTANCE\n"    ; break;
          case CDERR_NOHOOK          : throw "CDERR_NOHOOK\n"         ; break;
          case CDERR_NOTEMPLATE      : throw "CDERR_NOTEMPLATE\n"     ; break;
          case CDERR_STRUCTSIZE      : throw "CDERR_STRUCTSIZE\n"     ; break;
          case FNERR_BUFFERTOOSMALL  : throw "FNERR_BUFFERTOOSMALL\n" ; break;
          case FNERR_INVALIDFILENAME : throw "FNERR_INVALIDFILENAME\n"; break;
          case FNERR_SUBCLASSFAILURE : throw "FNERR_SUBCLASSFAILURE\n"; break;
          default                    : throw "You cancelled.\n";
          }
        }
      }
      catch (std::string& s) {
        l_pFs->changeWorkingDirectoryTo(l_sPath);
        if (s != "You cancelled.\n")
          throw s;
        else
          return L"";
      }
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
  }
}
