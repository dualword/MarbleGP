/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
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
  }
}
