// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace helpers {
    /**
    * Convert a string to a wide string
    * @param a_sInput the std::string to convert
    * @return a std::wstring with the content of the input string
    */
    std::wstring s2ws(const std::string& a_sInput);

    /**
    * Convert a wide string to a string
    * @param a_sInput the wide string to convert
    * @return a std::string with the content of the input
    */
    std::string ws2s(const std::wstring& a_sInput);

    /**
    * Split a string by a delimiter
    * @param a_sInput the string to split
    * @param a_cDelimiter the delimiter char to split the string
    * @return a vector with all the parts of the string, delimiter not included
    */
    std::vector<std::string> splitString(const std::string a_sInput, const char a_cDelimiter);

    /**
    * Fit a string to a dimension using a specific font (one line)
    * @param a_sText the string to fit
    * @param a_pFont the font to use
    * @param a_cSize the size to fit the string to
    * @return the string that fits the size using the font
    */
    std::wstring fitString(const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont, const irr::core::dimension2du &a_cSize);

    /**
    * Convert a timestamp in steps to a time string
    * @param a_iSteps the steps to convert
    * @return a string with the steps converted to a readable time
    */
    std::wstring convertToTime(irr::s32 a_iSteps);

    /**
    * Add a string to a %APPDATA%\DustbinGames\MarbleGP\debug.log
    * @param a_sInput the string to add
    */
    void addToDebugLog(const std::string &a_sInput);

    /**
    * Clear the debug log (at startup)
    */
    void clearDebugLog();

    /**
    * Load a textfile
    * @param a_sFile path to the file
    * @return the content of the file as string
    */
    std::string loadTextFile(const std::string& a_sFile);

    /**
    * Convert an Irrlicht key code to it's string representation
    * @param e the key code
    * @return the string representation
    */
    std::wstring keyCodeToString(irr::EKEY_CODE e);
  }
}
