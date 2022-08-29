// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <CGlobal.h>
#include <map>

namespace dustbin {
  namespace helpers {
    /**
    * Parse texture parameters
    * @param a_sType [out] returns the type of texture (default, generated, imported)
    * @param a_sInput the input as string
    * @return a string/string map with all parameters
    */
    std::map<std::string, std::string> parseParameters(std::string &a_sType, const std::string& a_sInput) {
      std::string l_sInput = "";

      size_t l_iPos = a_sInput.find("://");

      if (l_iPos != std::string::npos) {
        a_sType  = a_sInput.substr(0, l_iPos );
        l_sInput = a_sInput.substr(l_iPos + 3);
      }

      std::map<std::string, std::string> l_mParameters;
      size_t l_iPosAmp = std::string::npos;

      do {
        l_iPosAmp = l_sInput.find('&');
        std::string l_sPart;

        if (l_iPosAmp != std::string::npos) {
          l_sPart = l_sInput.substr(0, l_iPosAmp);
          l_sInput = l_sInput.substr(l_iPosAmp + 1);
        }
        else l_sPart = l_sInput;

        size_t l_iPosEq = l_sPart.find('=');

        if (l_iPosEq != std::string::npos) {
          std::string l_sKey   = l_sPart.substr(0, l_iPosEq),
                      l_sValue = l_sPart.substr(l_iPosEq + 1);

          l_mParameters[l_sKey] = l_sValue;
        }
        else {
          if (a_sType == "file") {
            l_mParameters["file"] = l_sInput;
          }
          else {
            std::string s = std::string("Invalid texture string \"") + a_sInput + "\" (" + l_sPart + ")";
            CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", s);
            CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
            // throw std::exception();
          }
        }
      } 
      while (l_iPosAmp != std::string::npos);

      return l_mParameters;
    }

    /**
    * Convert a string to an Irrlicht color
    * @param a_cColor the color that will be changed
    * @param a_sColor the input string
    */
    void fillColorFromString(irr::video::SColor& a_cColor, const std::string& a_sColor) {
      char* l_pEnd = nullptr;

      a_cColor.setAlpha(0xFF);
      a_cColor.setRed(strtol(a_sColor.substr(0, 2).c_str(), &l_pEnd, 16));
      a_cColor.setGreen(strtol(a_sColor.substr(2, 2).c_str(), &l_pEnd, 16));
      a_cColor.setBlue(strtol(a_sColor.substr(4, 2).c_str(), &l_pEnd, 16));
    }

    /**
    * Find a parameter in the map of texture parameters
    * @param a_mParameters the parameter map
    * @param a_sKey the key of the parameter
    * @return the parameter, empty string if the key was not found
    */
    std::string findTextureParameter(std::map<std::string, std::string>& a_mParameters, const std::string a_sKey) {
      if (a_mParameters.find(a_sKey) != a_mParameters.end()) {
        return a_mParameters[a_sKey];
      }

      return "";
    }

    /**
    * Create the default texture string
    * @param a_sNumber the starting number
    * @return the default texture string for the starting number
    */
    std::string createDefaultTextureString(const std::string& l_sNumber) {
      if (l_sNumber == "1")
        return "generate://numbercolor=000000&numberback=4b64f9&numberborder=4b64f9&ringcolor=3548b7&patterncolor=000000&patternback=4b64f9&pattern=texture_marbles2.png";
      else if (l_sNumber == "2")
        return "generate://numbercolor=000000&numberback=fd5320&numberborder=fd5320&ringcolor=ba3b15&patterncolor=000000&patternback=fd5320&pattern=texture_marbles2.png";
      else if (l_sNumber == "3")
        return "generate://numbercolor=000000&numberback=3aec1e&numberborder=3aec1e&ringcolor=28ae13&patterncolor=000000&patternback=3aec1e&pattern=texture_marbles2.png";
      else if (l_sNumber == "4")
        return "generate://numbercolor=000000&numberback=c0c000&numberborder=c0c000&ringcolor=8d8d00&patterncolor=000000&patternback=c0c000&pattern=texture_marbles2.png";
      else if (l_sNumber == "5")
        return "generate://numbercolor=000000&numberback=ffc0cb&numberborder=ffc0cb&ringcolor=bc8d95&patterncolor=000000&patternback=ffc0cb&pattern=texture_marbles2.png";
      else if (l_sNumber == "6")
        return "generate://numbercolor=ffffff&numberback=6a0dad&numberborder=6a0dad&ringcolor=c1bbcc&patterncolor=ffffff&patternback=6a0dad&pattern=texture_marbles2.png";
      else if (l_sNumber == "7")
        return "generate://numbercolor=000000&numberback=00ffff&numberborder=00ffff&ringcolor=00bcbc&patterncolor=000000&patternback=00ffff&pattern=texture_marbles2.png";
      else if (l_sNumber == "8")
        return "generate://numbercolor=ffffff&numberback=000000&numberborder=000000&ringcolor=bbbbbb&patterncolor=ffffff&patternback=000000&pattern=texture_marbles2.png";
      else if (l_sNumber == "9")
        return "generate://numbercolor=000000&numberback=ffdab9&numberborder=ffdab9&ringcolor=b9906b&patterncolor=000000&patternback=ffdab9&pattern=texture_marbles2.png";
      else if (l_sNumber == "10")
        return "generate://numbercolor=000000&numberback=87cefa&numberborder=87cefa&ringcolor=8ebfdd&patterncolor=000000&patternback=87cefa&pattern=texture_marbles2.png";
      else if (l_sNumber == "11")
        return "generate://numbercolor=000000&numberback=daa520&numberborder=daa520&ringcolor=b8860b&patterncolor=000000&patternback=daa520&pattern=texture_marbles2.png";
      else if (l_sNumber == "12")
        return "generate://numbercolor=000000&numberback=9932cc&numberborder=9932cc&ringcolor=da70d6&patterncolor=000000&patternback=9932cc&pattern=texture_marbles2.png";
      else if (l_sNumber == "13")
        return "generate://numbercolor=ffffff&numberback=00008b&numberborder=00008b&ringcolor=0000ff&patterncolor=ffffff&patternback=00008b&pattern=texture_marbles2.png";
      else if (l_sNumber == "14")
        return "generate://numbercolor=000000&numberback=ffdead&numberborder=ffdead&ringcolor=ffefd5&patterncolor=000000&patternback=ffdead&pattern=texture_marbles2.png";
      else if (l_sNumber == "15")
        return "generate://numbercolor=000000&numberback=20b2aa&numberborder=20b2aa&ringcolor=7fffd4&patterncolor=000000&patternback=20b2aa&pattern=texture_marbles2.png";
      else
        return "generate://numbercolor=000000&numberback=ffffff&numberborder=ffffff&ringcolor=dddddd&patterncolor=000000&patternback=ffffff&pattern=texture_marbles2.png";
    }

    /**
    * Add a fading border to the starting numbers
    * @param a_sNumber the number
    * @param a_cNumberColor the color of the number
    * @param a_cFrameColor the color of the frame
    * @param a_pDrv the video driver
    * @return a texture with the fading border
    */
    irr::video::ITexture* createFadingBorder(const std::string a_sNumber, const irr::video::SColor& a_cNumberColor, const irr::video::SColor& a_cBorderColor, const irr::video::SColor &a_cBackgroundColor, irr::video::IVideoDriver *a_pDrv) {

      irr::video::ITexture* l_pTexture = a_pDrv->addRenderTargetTexture(irr::core::dimension2du(256, 256), "FadingBorder_dummy", irr::video::ECF_A8R8G8B8);
      a_pDrv->setRenderTarget(l_pTexture, true, true, irr::video::SColor(0, a_cBorderColor.getRed(), a_cBorderColor.getGreen(), a_cBorderColor.getBlue()));

      std::string s = "data/textures/numbers/" + a_sNumber + ".png";

      irr::video::ITexture *l_pNumber = a_pDrv->getTexture(s.c_str());

      if (l_pNumber != nullptr) {
        irr::video::IImage *l_pNumberImg = a_pDrv->createImage(l_pNumber, irr::core::position2di(0, 0), irr::core::dimension2du(256, 256));

        if (l_pNumberImg != nullptr) {
          for (int y = 0; y < 256; y++) {
            for (int x = 0; x < 256; x++) {
              irr::video::SColor c = l_pNumberImg->getPixel(x, y);
              if (c.getAlpha() != 0) {
                if (c.getRed() < 128 || c.getGreen() < 128 || c.getBlue() < 128) {
                  if (a_cBackgroundColor != a_cBorderColor) {
                    c.setRed  (a_cBorderColor.getRed  ());
                    c.setGreen(a_cBorderColor.getGreen());
                    c.setBlue (a_cBorderColor.getBlue ());
                  }
                  else c.setAlpha(0);
                }
                else {
                  c.setRed  (a_cNumberColor.getRed  ());
                  c.setGreen(a_cNumberColor.getGreen());
                  c.setBlue (a_cNumberColor.getBlue ());
                }

                l_pNumberImg->setPixel(x, y, c);
              }
            }
          }
        }

        a_pDrv->draw2DImage(a_pDrv->addTexture("numberPlate", l_pNumberImg), irr::core::vector2di(0, 0), true);
        a_pDrv->removeTexture(l_pNumber);
        l_pNumberImg->drop();
      }

      irr::video::IImage* l_pImage = a_pDrv->createImage(l_pTexture, irr::core::position2di(0, 0), irr::core::dimension2du(256, 256));

      a_pDrv->setRenderTarget(nullptr, false, false);

      irr::video::ITexture* l_pRet = a_pDrv->addTexture("FadingBorder_dummy2", l_pImage);
      a_pDrv->removeTexture(l_pTexture);
      l_pImage->drop();

      return l_pRet;
    }  

    /**
    * This function loads the pattern texture and adjusts the marble
    * @param a_sFile the filename of the pattern texture
    * @param a_cColor the color for the pattern
    * @param a_pDrv the video driver
    * @return the adjusted marble texture
    */
    irr::video::ITexture* adjustTextureForMarble(const std::string& a_sFile, const irr::video::SColor& a_cColor, irr::video::IVideoDriver *a_pDrv) {
      irr::video::IImage* l_pImage = a_pDrv->createImageFromFile(a_sFile.c_str());

      if (l_pImage != nullptr) {
        irr::video::SColor l_cCol = a_cColor;
        for (unsigned x = 0; x < l_pImage->getDimension().Width; x++) {
          for (unsigned y = 0; y < l_pImage->getDimension().Height; y++) {
            l_cCol.setAlpha(l_pImage->getPixel(x, y).getAlpha());
            l_pImage->setPixel(x, y, l_cCol);
          }
        }

        irr::video::ITexture* l_pRet = a_pDrv->addTexture("adjustTextureForMarble_dummy", l_pImage);
        l_pImage->drop();
        return l_pRet;
      }

      return nullptr;
    }

    /**
    * Get an image from a string. The following prefixes are possible:
    * - file://: load a file from a subfolder
    * - generate://: generate a marble texture
    * @param a_sInput the URI of the file
    * @return an Irrlicht texture object with the requested image or nullptr
    */
    irr::video::ITexture* createTexture(const std::string& a_sUri, irr::video::IVideoDriver *a_pDrv, irr::io::IFileSystem *a_pFs) {
      irr::video::ITexture* l_pRet = nullptr;

      size_t l_iPos = a_sUri.find("://");

      std::string l_sType;

      std::map<std::string, std::string> l_mParameters = parseParameters(l_sType, a_sUri);

      if (l_iPos != std::string::npos) {
        if (l_sType == "file") {
          l_pRet = a_pDrv->getTexture(l_mParameters["file"].c_str());
        }
        else if (l_sType == "imported") {
          if (l_mParameters.find("file") != l_mParameters.end()) {
            std::string l_sFile = helpers::ws2s(platform::portableGetTexturePath()) + l_mParameters["file"];

            if (a_pFs->existFile(l_sFile.c_str())) {
              l_pRet = a_pDrv->getTexture(l_sFile.c_str());
            }
            else {
              CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", std::string("Cannot find texture \"") + l_sFile + "\".");
              CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
              // throw std::exception();
            }
          }
          else {
            CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "No file defined for imported texture.");
            CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
            // throw std::exception();
          }
        }
        else if (l_sType == "generate" || l_sType == "default") {
          irr::core::recti l_aDestRect[2] = {
            irr::core::recti(0,   0, 512, 256),
            irr::core::recti(0, 256, 512, 512)
          };

          irr::video::ITexture* l_pTexture = a_pDrv->findTexture(a_sUri.c_str());

          // printf("Texture: %s\n", a_sUri.c_str());
          if (l_pTexture == nullptr) {
            std::string l_sNumber = findTextureParameter(l_mParameters, "number");

            if (l_sNumber == "")
              l_sNumber = "1";

            if (l_sType == "default") {
              std::string l_sTexture = createDefaultTextureString(l_sNumber);
              l_mParameters = parseParameters(l_sType, l_sTexture);
            }

            std::string l_sColorNumber       = findTextureParameter(l_mParameters, "numbercolor" );
            std::string l_sColorNumberBack   = findTextureParameter(l_mParameters, "numberback"  );
            std::string l_sColorNumberBorder = findTextureParameter(l_mParameters, "numberborder");
            std::string l_sColorRing         = findTextureParameter(l_mParameters, "ringcolor"   );
            std::string l_sColorPattern      = findTextureParameter(l_mParameters, "patterncolor");
            std::string l_sColorPatternBack  = findTextureParameter(l_mParameters, "patternback" );
            std::string l_sPattern           = l_mParameters["pattern"];

            if (l_sNumber == "")
              l_sNumber = "1";

            irr::video::SColor l_cColorNumber;
            irr::video::SColor l_cColorNumberBack;
            irr::video::SColor l_cColorNumberBorder;
            irr::video::SColor l_cColorRing;
            irr::video::SColor l_cColorPattern;
            irr::video::SColor l_cColorPatternBack;

            fillColorFromString(l_cColorNumber      , l_sColorNumber      );
            fillColorFromString(l_cColorNumberBack  , l_sColorNumberBack  );
            fillColorFromString(l_cColorNumberBorder, l_sColorNumberBorder);
            fillColorFromString(l_cColorRing        , l_sColorRing        );
            fillColorFromString(l_cColorPattern     , l_sColorPattern     );
            fillColorFromString(l_cColorPatternBack , l_sColorPatternBack );

            // Render Texture

            std::string l_sFilePattern = std::string("data/patterns/") + l_sPattern,
                        l_sFileTop     = std::string("data/textures/texture_top.png");

            if (!a_pFs->existFile(l_sFilePattern.c_str())) {
              CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", std::string("Cannot find texture pattern \"") + l_sPattern + "\".");
              CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
              // throw std::exception();
            }

            if (!a_pFs->existFile(l_sFileTop.c_str())) {
              CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", std::string("Cannot find texture part \"texture_top.png\"."));
              CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
              // throw std::exception();
            }

            irr::video::ITexture* l_pNumber = nullptr;

            l_pNumber = createFadingBorder(l_sNumber, l_cColorNumber, l_cColorNumberBorder, l_cColorNumberBack, a_pDrv);

            l_pTexture = a_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), a_sUri.c_str());
            a_pDrv->setRenderTarget(l_pTexture, true, true, l_cColorNumberBack);
            a_pDrv->draw2DRectangle(l_cColorPatternBack, irr::core::recti(0, 255, 512, 512));

            if (l_pNumber != nullptr) {
              a_pDrv->draw2DImage(l_pNumber, irr::core::vector2di(0, 0), true);
              a_pDrv->draw2DImage(l_pNumber, irr::core::vector2di(256, 0), true);

              a_pDrv->removeTexture(l_pNumber);
            }

            irr::video::ITexture* l_pTop = adjustTextureForMarble(l_sFileTop, l_cColorRing, a_pDrv);
            a_pDrv->draw2DImage(l_pTop, l_aDestRect[0], irr::core::recti(0, 0, 511, 255), nullptr, nullptr, true);
            irr::video::ITexture*l_pPattern = adjustTextureForMarble(l_sFilePattern, l_cColorPattern, a_pDrv);
            a_pDrv->draw2DImage(l_pPattern, l_aDestRect[1], irr::core::recti(0, 0, 512, 256), nullptr, nullptr, true);

            a_pDrv->setRenderTarget(0, false, false);
          }

          return l_pTexture;
        }
      }

      return l_pRet;  
    }
  }
}