// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

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
  }
}
