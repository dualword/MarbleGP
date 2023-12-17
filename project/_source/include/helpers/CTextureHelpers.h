// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <shaders/CDustbinShaderDefines.h>

#include <irrlicht.h>
#include <string>
#include <vector>
#include <map>

namespace dustbin {
  namespace shaders {
    class CDustbinShaders;
  }

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

    /**
    * Get a vector with all the available texture patterns
    * @return a vector with all the available texture patterns
    */
    std::vector<std::string> getTexturePatterns();

    /**
    * Get the default color combinations
    * @return the default color combinations
    */
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getDefaultColors();

    /**
    * Add a node to the dustbin shader
    * @param a_pShader the shader to add the node to
    * @param a_pNode the node to add
    */
    void addNodeToShader(shaders::CDustbinShaders *a_pShader, irr::scene::ISceneNode* a_pNode);

    /**
    * Convert the shadow setting to the values for the shader
    * @param a_iShadows the shadow setting
    * @param a_pShader the shader instance to be adjusted
    */
    void convertForShader(int a_iShadows, shaders::CDustbinShaders *a_pShader);

#ifdef _OPENGL_ES
    /**
    * Adjust the materials of the node to get proper lighting when using
    * with OpenGL-ES on the raspberry PI
    * @param a_pNode the node to adjust
    */
    void adjustNodeMaterials(irr::scene::ISceneNode* a_pNode);
#endif
  }
}

