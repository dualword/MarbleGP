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

    std::wstring keyCodeToString(irr::EKEY_CODE e) {
      if (e == irr::KEY_LBUTTON   ) return L"KEY_LBUTTON";
      if (e == irr::KEY_RBUTTON   ) return L"KEY_RBUTTON";
      if (e == irr::KEY_CANCEL    ) return L"KEY_CANCEL";
      if (e == irr::KEY_MBUTTON   ) return L"KEY_MBUTTON";
      if (e == irr::KEY_XBUTTON1  ) return L"KEY_XBUTTON1";
      if (e == irr::KEY_XBUTTON2  ) return L"KEY_XBUTTON2";
      if (e == irr::KEY_BACK      ) return L"KEY_BACK";
      if (e == irr::KEY_TAB       ) return L"KEY_TAB";
      if (e == irr::KEY_CLEAR     ) return L"KEY_CLEAR";
      if (e == irr::KEY_RETURN    ) return L"KEY_RETURN";
      if (e == irr::KEY_SHIFT     ) return L"KEY_SHIFT";
      if (e == irr::KEY_CONTROL   ) return L"KEY_CONTROL";
      if (e == irr::KEY_MENU      ) return L"KEY_MENU";
      if (e == irr::KEY_PAUSE     ) return L"KEY_PAUSE";
      if (e == irr::KEY_CAPITAL   ) return L"KEY_CAPITAL";
      if (e == irr::KEY_KANA      ) return L"KEY_KANA";
      if (e == irr::KEY_HANGUEL   ) return L"KEY_HANGUEL";
      if (e == irr::KEY_HANGUL    ) return L"KEY_HANGUL";
      if (e == irr::KEY_JUNJA     ) return L"KEY_JUNJA";
      if (e == irr::KEY_FINAL     ) return L"KEY_FINAL";
      if (e == irr::KEY_HANJA     ) return L"KEY_HANJA";
      if (e == irr::KEY_KANJI     ) return L"KEY_KANJI";
      if (e == irr::KEY_ESCAPE    ) return L"KEY_ESCAPE";
      if (e == irr::KEY_CONVERT   ) return L"KEY_CONVERT";
      if (e == irr::KEY_NONCONVERT) return L"KEY_NONCONVERT";
      if (e == irr::KEY_ACCEPT    ) return L"KEY_ACCEPT";
      if (e == irr::KEY_MODECHANGE) return L"KEY_MODECHANGE";
      if (e == irr::KEY_SPACE     ) return L"KEY_SPACE";
      if (e == irr::KEY_PRIOR     ) return L"KEY_PRIOR";
      if (e == irr::KEY_NEXT      ) return L"KEY_NEXT";
      if (e == irr::KEY_END       ) return L"KEY_END";
      if (e == irr::KEY_HOME      ) return L"KEY_HOME";
      if (e == irr::KEY_LEFT      ) return L"KEY_LEFT";
      if (e == irr::KEY_UP        ) return L"KEY_UP";
      if (e == irr::KEY_RIGHT     ) return L"KEY_RIGHT";
      if (e == irr::KEY_DOWN      ) return L"KEY_DOWN";
      if (e == irr::KEY_SELECT    ) return L"KEY_SELECT";
      if (e == irr::KEY_PRINT     ) return L"KEY_PRINT";
      if (e == irr::KEY_EXECUT    ) return L"KEY_EXECUT";
      if (e == irr::KEY_SNAPSHOT  ) return L"KEY_SNAPSHOT";
      if (e == irr::KEY_INSERT    ) return L"KEY_INSERT";
      if (e == irr::KEY_DELETE    ) return L"KEY_DELETE";
      if (e == irr::KEY_HELP      ) return L"KEY_HELP";
      if (e == irr::KEY_KEY_0     ) return L"KEY_KEY_0";
      if (e == irr::KEY_KEY_1     ) return L"KEY_KEY_1";
      if (e == irr::KEY_KEY_2     ) return L"KEY_KEY_2";
      if (e == irr::KEY_KEY_3     ) return L"KEY_KEY_3";
      if (e == irr::KEY_KEY_4     ) return L"KEY_KEY_4";
      if (e == irr::KEY_KEY_5     ) return L"KEY_KEY_5";
      if (e == irr::KEY_KEY_6     ) return L"KEY_KEY_6";
      if (e == irr::KEY_KEY_7     ) return L"KEY_KEY_7";
      if (e == irr::KEY_KEY_8     ) return L"KEY_KEY_8";
      if (e == irr::KEY_KEY_9     ) return L"KEY_KEY_9";
      if (e == irr::KEY_KEY_A     ) return L"KEY_KEY_A";
      if (e == irr::KEY_KEY_B     ) return L"KEY_KEY_B";
      if (e == irr::KEY_KEY_C     ) return L"KEY_KEY_C";
      if (e == irr::KEY_KEY_D     ) return L"KEY_KEY_D";
      if (e == irr::KEY_KEY_E     ) return L"KEY_KEY_E";
      if (e == irr::KEY_KEY_F     ) return L"KEY_KEY_F";
      if (e == irr::KEY_KEY_G     ) return L"KEY_KEY_G";
      if (e == irr::KEY_KEY_H     ) return L"KEY_KEY_H";
      if (e == irr::KEY_KEY_I     ) return L"KEY_KEY_I";
      if (e == irr::KEY_KEY_J     ) return L"KEY_KEY_J";
      if (e == irr::KEY_KEY_K     ) return L"KEY_KEY_K";
      if (e == irr::KEY_KEY_L     ) return L"KEY_KEY_L";
      if (e == irr::KEY_KEY_M     ) return L"KEY_KEY_M";
      if (e == irr::KEY_KEY_N     ) return L"KEY_KEY_N";
      if (e == irr::KEY_KEY_O     ) return L"KEY_KEY_O";
      if (e == irr::KEY_KEY_P     ) return L"KEY_KEY_P";
      if (e == irr::KEY_KEY_Q     ) return L"KEY_KEY_Q";
      if (e == irr::KEY_KEY_R     ) return L"KEY_KEY_R";
      if (e == irr::KEY_KEY_S     ) return L"KEY_KEY_S";
      if (e == irr::KEY_KEY_T     ) return L"KEY_KEY_T";
      if (e == irr::KEY_KEY_U     ) return L"KEY_KEY_U";
      if (e == irr::KEY_KEY_V     ) return L"KEY_KEY_V";
      if (e == irr::KEY_KEY_W     ) return L"KEY_KEY_W";
      if (e == irr::KEY_KEY_X     ) return L"KEY_KEY_X";
      if (e == irr::KEY_KEY_Y     ) return L"KEY_KEY_Y";
      if (e == irr::KEY_KEY_Z     ) return L"KEY_KEY_Z";
      if (e == irr::KEY_LWIN      ) return L"KEY_LWIN";
      if (e == irr::KEY_RWIN      ) return L"KEY_RWIN";
      if (e == irr::KEY_APPS      ) return L"KEY_APPS";
      if (e == irr::KEY_SLEEP     ) return L"KEY_SLEEP";
      if (e == irr::KEY_NUMPAD0   ) return L"KEY_NUMPAD0";
      if (e == irr::KEY_NUMPAD1   ) return L"KEY_NUMPAD1";
      if (e == irr::KEY_NUMPAD2   ) return L"KEY_NUMPAD2";
      if (e == irr::KEY_NUMPAD3   ) return L"KEY_NUMPAD3";
      if (e == irr::KEY_NUMPAD4   ) return L"KEY_NUMPAD4";
      if (e == irr::KEY_NUMPAD5   ) return L"KEY_NUMPAD5";
      if (e == irr::KEY_NUMPAD6   ) return L"KEY_NUMPAD6";
      if (e == irr::KEY_NUMPAD7   ) return L"KEY_NUMPAD7";
      if (e == irr::KEY_NUMPAD8   ) return L"KEY_NUMPAD8";
      if (e == irr::KEY_NUMPAD9   ) return L"KEY_NUMPAD9";
      if (e == irr::KEY_MULTIPLY  ) return L"KEY_MULTIPLY";
      if (e == irr::KEY_ADD       ) return L"KEY_ADD";
      if (e == irr::KEY_SEPARATOR ) return L"KEY_SEPARATOR";
      if (e == irr::KEY_SUBTRACT  ) return L"KEY_SUBTRACT";
      if (e == irr::KEY_DECIMAL   ) return L"KEY_DECIMAL";
      if (e == irr::KEY_DIVIDE    ) return L"KEY_DIVIDE";
      if (e == irr::KEY_F1        ) return L"KEY_F1";
      if (e == irr::KEY_F2        ) return L"KEY_F2";
      if (e == irr::KEY_F3        ) return L"KEY_F3";
      if (e == irr::KEY_F4        ) return L"KEY_F4";
      if (e == irr::KEY_F5        ) return L"KEY_F5";
      if (e == irr::KEY_F6        ) return L"KEY_F6";
      if (e == irr::KEY_F7        ) return L"KEY_F7";
      if (e == irr::KEY_F8        ) return L"KEY_F8";
      if (e == irr::KEY_F9        ) return L"KEY_F9";
      if (e == irr::KEY_F10       ) return L"KEY_F10";
      if (e == irr::KEY_F11       ) return L"KEY_F11";
      if (e == irr::KEY_F12       ) return L"KEY_F12";
      if (e == irr::KEY_F13       ) return L"KEY_F13";
      if (e == irr::KEY_F14       ) return L"KEY_F14";
      if (e == irr::KEY_F15       ) return L"KEY_F15";
      if (e == irr::KEY_F16       ) return L"KEY_F16";
      if (e == irr::KEY_F17       ) return L"KEY_F17";
      if (e == irr::KEY_F18       ) return L"KEY_F18";
      if (e == irr::KEY_F19       ) return L"KEY_F19";
      if (e == irr::KEY_F20       ) return L"KEY_F20";
      if (e == irr::KEY_F21       ) return L"KEY_F21";
      if (e == irr::KEY_F22       ) return L"KEY_F22";
      if (e == irr::KEY_F23       ) return L"KEY_F23";
      if (e == irr::KEY_F24       ) return L"KEY_F24";
      if (e == irr::KEY_NUMLOCK   ) return L"KEY_NUMLOCK";
      if (e == irr::KEY_SCROLL    ) return L"KEY_SCROLL";
      if (e == irr::KEY_LSHIFT    ) return L"KEY_LSHIFT";
      if (e == irr::KEY_RSHIFT    ) return L"KEY_RSHIFT";
      if (e == irr::KEY_LCONTROL  ) return L"KEY_LCONTROL";
      if (e == irr::KEY_RCONTROL  ) return L"KEY_RCONTROL";
      if (e == irr::KEY_LMENU     ) return L"KEY_LMENU";
      if (e == irr::KEY_RMENU     ) return L"KEY_RMENU";
      if (e == irr::KEY_PLUS      ) return L"KEY_PLUS";
      if (e == irr::KEY_COMMA     ) return L"KEY_COMMA";
      if (e == irr::KEY_MINUS     ) return L"KEY_MINUS";
      if (e == irr::KEY_PERIOD    ) return L"KEY_PERIOD";
      if (e == irr::KEY_ATTN      ) return L"KEY_ATTN";
      if (e == irr::KEY_CRSEL     ) return L"KEY_CRSEL";
      if (e == irr::KEY_EXSEL     ) return L"KEY_EXSEL";
      if (e == irr::KEY_EREOF     ) return L"KEY_EREOF";
      if (e == irr::KEY_PLAY      ) return L"KEY_PLAY";
      if (e == irr::KEY_ZOOM      ) return L"KEY_ZOOM";
      if (e == irr::KEY_PA1       ) return L"KEY_PA1";
      if (e == irr::KEY_OEM_CLEAR ) return L"KEY_OEM_CLEAR";
      return L"UNKNOWN";
    }


    /**
    * Trim a string
    * @param a_sInput the string to trim
    * @return the trimmed string
    */
    std::string trimString(const std::string& a_sInput) {
      auto start = a_sInput.begin();
      while (start != a_sInput.end() && std::isspace(*start)) {
        start++;
      }

      auto end = a_sInput.end();
      do {
        end--;
      } while (std::distance(start, end) > 0 && std::isspace(*end));

      return std::string(start, end + 1);    
    }

    /**
    * Divide a name string into name and bot class (input: "<name>|<class>"
    * @param a_sInput the original string
    * @param a_sName [out] the name
    * @param a_sBot [out] the bot class, if the name is not a bot the string is empty
    * @return true if a bot class was found, false otherwise
    */
    bool splitStringNameBot(const std::string& a_sInput, std::string& a_sName, std::string& a_sBot) {
      size_t l_iPos = a_sInput.find_last_of('|');

      if (l_iPos == std::string::npos) {
        a_sName = a_sInput;
        return false;
      }
      else {
        a_sName = a_sInput.substr(0, l_iPos);
        a_sBot  = a_sInput.substr(l_iPos + 1);
        return true;
      }
    }

    /**
    * Divide a name string into name and bot class (input: "<name>|<class>"
    * @param a_sInput the original string
    * @param a_sName [out] the name
    * @param a_sBot [out] the bot class, if the name is not a bot the string is empty
    * @return true if a bot class was found, false otherwise
    */
    bool splitStringNameBot(const std::wstring& a_sInput, std::wstring& a_sName, std::wstring& a_sBot) {
      size_t l_iPos = a_sInput.find_last_of(L'|');

      if (l_iPos == std::string::npos) {
        a_sName = a_sInput;
        return false;
      }
      else {
        a_sName = a_sInput.substr(0, l_iPos);
        a_sBot  = a_sInput.substr(l_iPos + 1);
        return true;
      }
    }
  }
}