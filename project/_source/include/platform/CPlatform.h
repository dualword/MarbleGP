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
    void portableOpenUrl(const std::wstring &a_sUrl);
    void portableFocusWindow(irr::video::IVideoDriver *a_pDrv);
    void preventScreenSaver();
    void portableGetImagePath(std::wstring &a_sPath);
    void portableCopyFile(const std::wstring &a_sSource, const std::wstring &a_sTarget);
    void portableCreateFolder(const std::wstring &a_sFolder);
    bool portableIsFolder(const std::string &a_sPath);
    const std::wstring portableOpenFileDialog(const std::wstring &a_sFilter, const std::wstring &a_sInitialDir, const std::wstring &a_sTitle);
    const std::wstring portableSaveFileDialog(const std::wstring &a_sFilter, const std::wstring &a_sInitialDir, const std::wstring &a_sTitle);

    const std::wstring portableGetDataPath();
    const std::string  portableGetFontPath();

    irr::u64 portableGetFileDate(const std::wstring &a_sFile);

    unsigned long long int portableGetLastModification(const std::wstring &a_sFile);

    std::vector<std::string> getDrives();

    std::wstring s2ws(const std::string& str);
    std::string ws2s(const std::wstring& wstr);
  }
}
