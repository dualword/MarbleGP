/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */

#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <winerror.h>
#include <Windows.h>
#include <shlwapi.h>
#include "shlobj.h"

namespace dustbin {
  namespace platform {
    /**
    * This file contains functions for the Windows platform
    */

    /**
    * Get the path to the system's fonts
    * @return the path to the system's fonts
    */
    std::string portableGetFontPath() {
      char l_sPath[MAX_PATH] = "";
      SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, l_sPath);
      return l_sPath;
    }

    /**
    * Select a font using the system's font selection dialog
    * @return the selected font
    */
    std::string portableSelectFont(int& a_iFontSize) {
      std::string l_sFont = "";
      CHOOSEFONT  l_cFont;
      LOGFONT     l_cLog;

      l_cFont.lStructSize = sizeof(CHOOSEFONT);
      l_cFont.hwndOwner = (HWND)NULL;
      l_cFont.lpLogFont = &l_cLog;
      l_cFont.Flags = CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
      l_cFont.rgbColors = RGB(0, 0, 0);
      l_cFont.lCustData = 0L;
      l_cFont.lpfnHook = (LPCFHOOKPROC)NULL;
      l_cFont.lpTemplateName = (LPCWSTR)NULL;
      l_cFont.hInstance = (HINSTANCE)NULL;
      l_cFont.lpszStyle = (LPWSTR)NULL;
      l_cFont.nFontType = SCREEN_FONTTYPE;
      l_cFont.nSizeMin = 24;

      if (ChooseFont(&l_cFont)) {
        l_sFont = helpers::ws2s(l_cFont.lpLogFont->lfFaceName);
        a_iFontSize = l_cFont.iPointSize / 10;
      }
      
      return l_sFont;
    }

    /**
    * Show a select folder dialog
    * @return the selected folder, empty string if the dialog was cancelled
    */
    std::string portableSelectFolder() {
      std::string l_sFolderName;

      TCHAR l_sBuffer[MAX_PATH] = L"";
      BROWSEINFO l_cBrowseInfo;
      ZeroMemory(&l_cBrowseInfo, sizeof(BROWSEINFO));
      l_cBrowseInfo.pszDisplayName = l_sBuffer;
      l_cBrowseInfo.pidlRoot = NULL;
      l_cBrowseInfo.lpszTitle = L"directory folder is selected from the following:";
      l_cBrowseInfo.ulFlags = BIF_NEWDIALOGSTYLE; ;
      LPITEMIDLIST l_pItemList = SHBrowseForFolder(&l_cBrowseInfo);
      if (NULL != l_pItemList)
      {
        SHGetPathFromIDList(l_pItemList, l_sBuffer);
        l_sFolderName = helpers::ws2s(l_sBuffer);
      }
      return l_sFolderName;
    }

    /**
    * Show an open file dialog
    * @param a_sFilter the file filter string
    * @param a_sDefault the folder to open the dialog in or the file name
    * @param a_sFileName the default file name
    * @return the selected file, empty string if the dialog was cancelled
    */
    std::string portableSelectFileOpen(const wchar_t* a_sFilter, const wchar_t* a_sFolder, const wchar_t* a_sFileName) {
      OPENFILENAME ofn;
      // wchar_t FilterSpec[] = L"Object Files(.obj)\0*.obj\0Text Files(.txt)\0.txt\0All Files(.)\0*.*\0";
      wchar_t Title[] = L"Open....";
      wchar_t szFileName[MAX_PATH];
      wchar_t szFileTitle[MAX_PATH];

      *szFileName = 0; *szFileTitle = 0;

      wcscpy_s(szFileName, a_sFileName);

      /* fill in non-variant fields of OPENFILENAME struct. */
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = GetFocus();
      ofn.lpstrFilter = a_sFilter;
      ofn.lpstrCustomFilter = NULL;
      ofn.nMaxCustFilter = 0;
      ofn.nFilterIndex = 0;
      ofn.lpstrFile = szFileName;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrInitialDir = a_sFolder;
      ofn.lpstrFileTitle = szFileTitle;
      ofn.nMaxFileTitle = MAX_PATH;
      ofn.lpstrTitle = Title;
      ofn.lpstrDefExt = 0;//I've set to null for demonstration
      ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

      ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

      if (!GetOpenFileName((LPOPENFILENAME)&ofn))
      {
        return ""; // Failed or cancelled
      }
      else
      {
        return helpers::ws2s(szFileName);
      }
    }

    /**
    * Show a save file dialog
    * @param a_sFilter the file filter string
    * @param a_sDefault the folder to open the dialog in or the file name
    * @param a_sFileName the default file name
    * @return the selected file, empty string if the dialog was cancelled
    */
    std::string portableSelectFileSave(const wchar_t* a_sFilter, const wchar_t* a_sFolder, const wchar_t* a_sFileName) {
      OPENFILENAME ofn;
      // wchar_t FilterSpec[] = L"Object Files(.obj)\0*.obj\0Text Files(.txt)\0.txt\0All Files(.)\0*.*\0";
      wchar_t Title[] = L"Open....";
      wchar_t szFileName[MAX_PATH];
      wchar_t szFileTitle[MAX_PATH];

      *szFileName = 0; *szFileTitle = 0;

      wcscpy_s(szFileName, a_sFileName);

      /* fill in non-variant fields of OPENFILENAME struct. */
      ofn.lStructSize = sizeof(OPENFILENAME);
      ofn.hwndOwner = GetFocus();
      ofn.lpstrFilter = a_sFilter;
      ofn.lpstrCustomFilter = NULL;
      ofn.nMaxCustFilter = 0;
      ofn.nFilterIndex = 0;
      ofn.lpstrFile = szFileName;
      ofn.nMaxFile = MAX_PATH;
      ofn.lpstrInitialDir = a_sFolder;
      ofn.lpstrFileTitle = szFileTitle;
      ofn.nMaxFileTitle = MAX_PATH;
      ofn.lpstrTitle = Title;
      ofn.lpstrDefExt = 0;//I've set to null for demonstration
      ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

      ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

      if (!GetSaveFileName((LPOPENFILENAME)&ofn))
      {
        return ""; // Failed or cancelled
      }
      else
      {
        return helpers::ws2s(szFileName);
      }
    }
  }
}