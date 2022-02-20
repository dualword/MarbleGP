// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <stdlib.h>
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
    }  }
}