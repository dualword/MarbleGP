// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <map>

namespace dustbin {
  namespace helpers {
    /**
    * Get an image from a string. The following prefixes are possible:
    * - file://: load a file from a subfolder
    * - generate://: generate a marble texture
    * @param a_sInput the URI of the file
    * @param a_pDrv the video driver
    * @param a_pFs the Irrlicht file system
    * @return an Irrlicht texture object with the requested image or nullptr
    */
    irr::video::ITexture* createTexture(const std::string& a_sUri, irr::video::IVideoDriver *a_pDrv, irr::io::IFileSystem *a_pFs);

    /**
    * Parse texture parameters
    * @param a_sType [out] returns the type of texture (default, generated, imported)
    * @param a_sInput the input as string
    * @return a string/string map with all parameters
    */
    std::map<std::string, std::string> parseParameters(std::string &a_sType, const std::string& a_sInput);
  }
}

