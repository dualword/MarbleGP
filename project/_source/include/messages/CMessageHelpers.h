// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <string>

namespace dustbin {
  namespace messages {
    #define _64_DIGITS_STRING "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+="

    /**
     * URL-decode a string
     * @param toDecode the string to decode
     * @return the URL-decoded string
     */
    std::string urlDecode(const std::string &toDecode);

    /**
    * URL-encode a string
    * @param toEncode the string to encode
    * @return the URL-encoded string
    */
    std::string urlEncode(const std::string &toEncode);

    /**
    * Base64 encode a chunk of data
    * @param a_pData pointer to the data
    * @param a_iSize size of the data chunk
    * @param a_bUrl encode the data for use in URLs?
    * @return a base64 encoded string containing the data
    */
    std::string base64Encode(unsigned char const *a_pData, size_t a_iSize, bool a_bUrl);
  }
}
