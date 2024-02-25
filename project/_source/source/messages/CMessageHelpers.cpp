// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <messages/CMessageHelpers.h>
#include <iomanip>
#include <sstream>
#include <string>

namespace dustbin {
  namespace messages {
    static const char* base64_chars[2] = {
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789"
      "+/",

      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789"
      "-_"};

    unsigned char hexToChar(const std::string &str) {
      short c = 0;

      if(!str.empty()) {
        std::istringstream in(str);

        in >> std::hex >> c;

        if(in.fail()) {
          // throw std::runtime_error("stream decode failure");
        }
      }

      return static_cast<unsigned char>(c);
    }

    std::string urlDecode(const std::string &toDecode) {
      std::ostringstream out;

      for(std::string::size_type i=0; i < toDecode.length(); ++i) {
        if(toDecode.at(i) == '%') {
          std::string str(toDecode.substr(i+1, 2));
          out << hexToChar(str);
          i += 2;
        } else {
          if (toDecode.at(i) == '+') {
            out << " ";
          }
          else {
            out << toDecode.at(i);
          }
        }
      }

      return out.str();
    }

    std::string charToHex(unsigned char c) {
      short i = c;

      std::stringstream s;

      s << "%" << std::setw(2) << std::setfill('0') << std::hex << i;

      return s.str();
    }

    std::string urlEncode(const std::string &toEncode) {
      std::ostringstream out;

      for(std::string::size_type i=0; i < toEncode.length(); ++i) {
        short t = toEncode.at(i);

        if(
          t == 45 ||          // hyphen
          (t >= 48 && t <= 57) ||       // 0-9
          (t >= 65 && t <= 90) ||       // A-Z
          t == 95 ||          // underscore
          (t >= 97 && t <= 122) ||  // a-z
          t == 126            // tilde
          ) {
          out << toEncode.at(i);
        } else {
          out << charToHex(toEncode.at(i));
        }
      }

      return out.str();
    }


    /**
    * Base64 encode a chunk of data
    * @param a_pData pointer to the data
    * @param a_iSize size of the data chunk
    * @param a_bUrl encode the data for use in URLs?
    * @return a base64 encoded string containing the data
    */
    std::string base64Encode(unsigned char const *a_pData, size_t a_iSize, bool a_bUrl) {
      size_t len_encoded = (a_iSize + 2) / 3 * 4;

      unsigned char l_cTrailingChar = a_bUrl ? '.' : '=';

      const char* l_pBase64Chars = base64_chars[a_bUrl];

      std::string l_sReturn;
      l_sReturn.reserve(len_encoded);

      unsigned int pos = 0;

      while (pos < a_iSize) {
        l_sReturn.push_back(l_pBase64Chars[(a_pData[pos + 0] & 0xfc) >> 2]);

        if (pos + 1 < a_iSize) {
          l_sReturn.push_back(l_pBase64Chars[((a_pData[pos + 0] & 0x03) << 4) + ((a_pData[pos + 1] & 0xf0) >> 4)]);

          if (pos + 2 < a_iSize) {
            l_sReturn.push_back(l_pBase64Chars[((a_pData[pos + 1] & 0x0f) << 2) + ((a_pData[pos + 2] & 0xc0) >> 6)]);
            l_sReturn.push_back(l_pBase64Chars[  a_pData[pos + 2] & 0x3f]);
          }
          else {
            l_sReturn.push_back(l_pBase64Chars[(a_pData[pos + 1] & 0x0f) << 2]);
            l_sReturn.push_back(l_cTrailingChar);
          }
        }
        else {
          l_sReturn.push_back(l_pBase64Chars[(a_pData[pos + 0] & 0x03) << 4]);
          l_sReturn.push_back(l_cTrailingChar);
          l_sReturn.push_back(l_cTrailingChar);
        }

        pos += 3;
      }


      return l_sReturn;    
    }
  }
}
