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

    /**
    * Convert a string to an Irrlicht color
    * @param a_cColor the color that will be changed
    * @param a_sColor the input string
    */
    void fillColorFromString(irr::video::SColor& a_cColor, const std::string& a_sColor);


    /**
    * Find a parameter in the map of texture parameters
    * @param a_mParameters the parameter map
    * @param a_sKey the key of the parameter
    * @return the parameter, empty string if the key was not found
    */
    std::string findTextureParameter(std::map<std::string, std::string>& a_mParameters, const std::string a_sKey);

    /**
    * Create the default texture string
    * @param a_sNumber the starting number
    * @param a_iClass the class of the marble (0 == MarbleGP, 1 == Marble2, 2 == Marble3)
    * @return the default texture string for the starting number
    */
    std::string createDefaultTextureString(const std::string &a_sNumber, int a_iClass);
  }
}

