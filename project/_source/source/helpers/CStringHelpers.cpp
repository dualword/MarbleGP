// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <CGlobal.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <codecvt>
#include <stdio.h>
#include <locale>

namespace dustbin {
  namespace helpers {
    /**
    * Convert a string to a wide string
    * @param a_sInput the std::string to convert
    * @return a std::wstring with the content of the input string
    */
    std::wstring s2ws(const std::string& a_sInput) {
      using convert_typeX = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_typeX, wchar_t> converterX;
      return converterX.from_bytes(a_sInput);
    }

    /**
    * Convert a wide string to a string
    * @param a_sInput the wide string to convert
    * @return a std::string with the content of the input
    */
    std::string ws2s(const std::wstring& a_sInput) {
      using convert_typeX = std::codecvt_utf8<wchar_t>;
      std::wstring_convert<convert_typeX, wchar_t> converterX;
      return converterX.to_bytes(a_sInput);
    }

    /**
    * Split a string by a delimiter
    * @param a_sInput the string to split
    * @param a_cDelimiter the delimiter char to split the string
    * @return a vector with all the parts of the string, delimiter not included
    */
    std::vector<std::string> splitString(const std::string a_sInput, const char a_cDelimiter) {
      std::vector<std::string> l_vRet;

      std::string l_sInput = a_sInput;

      while (l_sInput.size() > 0) {
        size_t l_iPos = l_sInput.find_first_of(a_cDelimiter);

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

    /**
    * Fit a string to a dimension using a specific font (one line, only width)
    * @param a_sText the string to fit
    * @param a_pFont the font to use
    * @param a_cSize the size to fit the string to
    * @return the string that fits the size using the font
    */
    std::wstring fitString(const std::wstring& a_sText, irr::gui::IGUIFont* a_pFont, const irr::core::dimension2du& a_cSize) {
      std::wstring s = a_sText;

      irr::core::dimension2du l_cSize = a_pFont->getDimension(s.c_str());

      while (l_cSize.Width > a_cSize.Width) {
        s = s.substr(0, s.size() - 3) + L"..";
        l_cSize = a_pFont->getDimension(s.c_str());
      }

      return s;
    }

    /**
    * Convert a timestamp in steps to a time string. Negative times are interpreted as lap deficits
    * @param a_iSteps the steps to convert
    * @return a string with the steps converted to a readable time
    */
    std::wstring convertToTime(irr::s32 a_iSteps) {
      if (a_iSteps >= 0) {
        // Convert to 1/100th seconds integer
        irr::s32 l_iTime = (irr::s32)(a_iSteps / 1.2f);

        irr::s32 l_iHundrts = l_iTime % 100; l_iTime /= 100;
        irr::s32 l_iSeconds = l_iTime %  60; l_iTime /=  60;
        irr::s32 l_iMinutes = l_iTime;

        std::wstring l_sHundrts = std::to_wstring(l_iHundrts); while (l_sHundrts.size() < 2) l_sHundrts = L"0" + l_sHundrts;
        std::wstring l_sSeconds = std::to_wstring(l_iSeconds); if (l_iMinutes > 0) while (l_sSeconds.size() < 2) l_sSeconds = L"0" + l_sSeconds;
        std::wstring l_sMinutes = std::to_wstring(l_iMinutes);

        std::wstring l_sRet = L"";

        if (l_iMinutes > 0)
          l_sRet = l_sMinutes + L":";

        return l_sRet + l_sSeconds + L"." + l_sHundrts;
      }
      else if (a_iSteps == -1) {
        return L"1 Lap";
      }
      else {
        return std::to_wstring(abs(a_iSteps)) + L" Laps";
      }
    }

    /**
    * Add a string to a %APPDATA%\DustbinGames\MarbleGP\debug.log
    * @param a_sInput the string to add
    */
    void addToDebugLog(const std::string& a_sInput) {
      std::string l_sFile = ws2s(platform::portableGetDataPath()) + "debug.log";
      std::ofstream l_cFile;
      l_cFile.open(l_sFile, std::ios::app);
      l_cFile << a_sInput << std::endl;
      l_cFile.close();
    }

    /**
    * Clear the debug log (at startup)
    */
    void clearDebugLog() {
      std::string l_sFile = ws2s(platform::portableGetDataPath()) + "debug.log";
      std::ofstream l_cFile;
      l_cFile.open(l_sFile, std::ios::trunc);
      l_cFile << "MarbleGP Starting." << std::endl;
      l_cFile.close();
    }

    /**
    * Load a textfile
    * @param a_sFile path to the file
    * @return the content of the file as string
    */
    std::string loadTextFile(const std::string& a_sFile) {
      std::string l_sRet = "";

      irr::io::IReadFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(a_sFile.c_str());

      if (l_pFile != nullptr) {
        char *s = new char[l_pFile->getSize() + 1];
        memset(s, 0, l_pFile->getSize() + 1);
        l_pFile->read(s, l_pFile->getSize());
        l_sRet = s;
        l_pFile->drop();
        delete []s;
      }

      return l_sRet;
    }
  }
}