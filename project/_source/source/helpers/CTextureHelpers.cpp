// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <shaders/CDustbinShaders.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <scenenodes/CSpeedNode.h>
#include <scenenodes/CJointNode.h>
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

      irr::u32 l_iRed   = strtol(a_sColor.substr(0, 2).c_str(), &l_pEnd, 16);
      irr::u32 l_iGreen = strtol(a_sColor.substr(2, 2).c_str(), &l_pEnd, 16);
      irr::u32 l_iBlue  = strtol(a_sColor.substr(4, 2).c_str(), &l_pEnd, 16);

      a_cColor.setRed  (l_iRed);
      a_cColor.setGreen(l_iGreen);
      a_cColor.setBlue (l_iBlue);
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
    std::string createDefaultTextureString(const std::string& l_sNumber, int a_iClass) {
      std::string l_sPattern = a_iClass == 2 ? "texture_rookie.png" : a_iClass == 1 ? "texture_marbles3.png" : "texture_marbles2.png";

      if (l_sNumber == "1")
        return "generate://numbercolor=000000&numberback=4b64f9&numberborder=4b64f9&ringcolor=3548b7&patterncolor=000000&patternback=4b64f9&pattern=" + l_sPattern;
      else if (l_sNumber == "2")
        return "generate://numbercolor=000000&numberback=fd5320&numberborder=fd5320&ringcolor=ba3b15&patterncolor=000000&patternback=fd5320&pattern=" + l_sPattern;
      else if (l_sNumber == "3")
        return "generate://numbercolor=000000&numberback=3aec1e&numberborder=3aec1e&ringcolor=28ae13&patterncolor=000000&patternback=3aec1e&pattern=" + l_sPattern;
      else if (l_sNumber == "4")
        return "generate://numbercolor=000000&numberback=c0c000&numberborder=c0c000&ringcolor=8d8d00&patterncolor=000000&patternback=c0c000&pattern=" + l_sPattern;
      else if (l_sNumber == "5")
        return "generate://numbercolor=000000&numberback=ffc0cb&numberborder=ffc0cb&ringcolor=bc8d95&patterncolor=000000&patternback=ffc0cb&pattern=" + l_sPattern;
      else if (l_sNumber == "6")
        return "generate://numbercolor=ffffff&numberback=6a0dad&numberborder=6a0dad&ringcolor=c1bbcc&patterncolor=ffffff&patternback=6a0dad&pattern=" + l_sPattern;
      else if (l_sNumber == "7")
        return "generate://numbercolor=000000&numberback=00ffff&numberborder=00ffff&ringcolor=00bcbc&patterncolor=000000&patternback=00ffff&pattern=" + l_sPattern;
      else if (l_sNumber == "8")
        return "generate://numbercolor=ffffff&numberback=000000&numberborder=000000&ringcolor=bbbbbb&patterncolor=ffffff&patternback=000000&pattern=" + l_sPattern;
      else if (l_sNumber == "9")
        return "generate://numbercolor=000000&numberback=ffdab9&numberborder=ffdab9&ringcolor=b9906b&patterncolor=000000&patternback=ffdab9&pattern=" + l_sPattern;
      else if (l_sNumber == "10")
        return "generate://numbercolor=000000&numberback=87cefa&numberborder=87cefa&ringcolor=8ebfdd&patterncolor=000000&patternback=87cefa&pattern=" + l_sPattern;
      else if (l_sNumber == "11")
        return "generate://numbercolor=000000&numberback=daa520&numberborder=daa520&ringcolor=b8860b&patterncolor=000000&patternback=daa520&pattern=" + l_sPattern;
      else if (l_sNumber == "12")
        return "generate://numbercolor=000000&numberback=9932cc&numberborder=9932cc&ringcolor=da70d6&patterncolor=000000&patternback=9932cc&pattern=" + l_sPattern;
      else if (l_sNumber == "13")
        return "generate://numbercolor=ffffff&numberback=00008b&numberborder=00008b&ringcolor=0000ff&patterncolor=ffffff&patternback=00008b&pattern=" + l_sPattern;
      else if (l_sNumber == "14")
        return "generate://numbercolor=000000&numberback=ffdead&numberborder=ffdead&ringcolor=ffefd5&patterncolor=000000&patternback=ffdead&pattern=" + l_sPattern;
      else if (l_sNumber == "15")
        return "generate://numbercolor=000000&numberback=20b2aa&numberborder=20b2aa&ringcolor=7fffd4&patterncolor=000000&patternback=20b2aa&pattern=" + l_sPattern;
      else if (l_sNumber == "16")
        return "generate://numbercolor=000000&numberback=ffffff&numberborder=ffffff&ringcolor=dddddd&patterncolor=000000&patternback=ffffff&pattern=" + l_sPattern;
      else
        return l_sNumber + "&pattern=" + l_sPattern;
    }

    /**
    * Get a vector with all the available texture patterns
    * @return a vector with all the available texture patterns
    */
    std::vector<std::string> getTexturePatterns() {
      std::vector<std::string> l_vReturn;

      irr::io::IXMLReaderUTF8 *l_pXml = CGlobal::getInstance()->getFileSystem()->createXMLReaderUTF8("data/patterns/patterns.xml");

      if (l_pXml) {
        while (l_pXml->read()) {
          if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            std::string l_sNode = l_pXml->getNodeName();

            if (l_sNode == "pattern") {
              std::string l_sPattern = l_pXml->getAttributeValueSafe("file");

              if (l_sPattern != "") {
                std::string l_sPath = "data/patterns/" + l_sPattern;

                if (CGlobal::getInstance()->getFileSystem()->existFile(l_sPath.c_str())) {
                  l_vReturn.push_back(l_sPattern);
                }
              }
            }
          }
        }

        l_pXml->drop();
      }

      return l_vReturn;
    }

    /**
    * Get the default color combinations
    * @return the default color combinations
    */
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getDefaultColors() {
      std::vector<std::tuple<std::string, std::string, std::string, std::string>> l_vRet;

      return l_vRet;
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

      std::string s = "data/textures/numbers/" + a_sNumber + ".png\0";

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
              std::string l_sClass = findTextureParameter(l_mParameters, "class");
              int l_iClass = 0;

              if (l_sClass != "")
                l_iClass = std::atoi(l_sClass.c_str());

              std::string l_sTexture = createDefaultTextureString(l_sNumber, l_iClass);
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

            std::string l_sFilePattern = std::string("data/patterns/") + l_sPattern;
            std::string l_sFileTop     = std::string("data/textures/texture_top.png");

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

    /**
    * Add a material to the Dustbin Shader
    * @param a_pShader the shader instance
    * @param a_pNode the node with the material
    * @param a_iMaterial the material ID
    * @param a_eType the type of node this material belongs to
    */
    void addMaterialToShader(shaders::CDustbinShaders *a_pShader, irr::scene::IMeshSceneNode* a_pNode, irr::u32 a_iMaterial, shaders::enObjectType a_eType) {
      irr::video::E_MATERIAL_TYPE l_eMaterial = a_pNode->getMaterial(a_iMaterial).MaterialType;

      if (l_eMaterial == irr::video::EMT_SOLID || a_pShader->isShaderMaterial(l_eMaterial)) {
        std::string l_sName = a_pNode->getName();

        bool l_bMarble = l_sName.substr(0, 7) == "Marble_";
        bool l_bSolidM = !a_pNode->getMaterial(a_iMaterial).Lighting;
        bool l_bWall   = false;

        if (a_pNode->getParent() != nullptr && a_pNode->getParent() != a_pShader->getSceneManager()->getRootSceneNode()) {
          if (a_pNode->getParent()->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_SpeedNodeId && a_iMaterial == 0) {
            l_bSolidM = true;
          }
        }

        shaders::enMaterialType l_eType = shaders::enMaterialType::SolidOne;

        if (a_pNode->getMaterial(a_iMaterial).getTexture(0) != nullptr) {
          std::string l_sTexture = a_pNode->getMaterial(a_iMaterial).getTexture(0)->getName().getPath().c_str();

          if (l_sTexture.find_last_of('/') != std::string::npos)
            l_sTexture = l_sTexture.substr(l_sTexture.find_last_of('/') + 1);

          if (l_sTexture.find_last_of('\\') != std::string::npos)
            l_sTexture = l_sTexture.substr(l_sTexture.find_last_of('\\') + 1);

          if (l_sTexture == "wall_blue_back.png" || l_sTexture == "wall_orange_back.png")
            l_bWall = true;
        }

        if (!l_bSolidM) {
          if (l_bMarble)
            l_eType = shaders::enMaterialType::Marble;
          else if (l_bWall) {
            if (a_pNode->getMaterial(a_iMaterial).getTexture(2) != nullptr)
              l_eType = shaders::enMaterialType::Wall3;
            else if (a_pNode->getMaterial(a_iMaterial).getTexture(1) != nullptr)
              l_eType = shaders::enMaterialType::Wall2;
            else
              l_eType = shaders::enMaterialType::Wall1;
          }
          else if (a_pNode->getMaterial(a_iMaterial).getTexture(2) != nullptr)
            l_eType = shaders::enMaterialType::SolidThree;
          else if (a_pNode->getMaterial(a_iMaterial).getTexture(1) != nullptr)
            l_eType = shaders::enMaterialType::SolidTwo;
        }

        a_pShader->addNodeMaterial(
          a_pNode, 
          a_iMaterial, 
          l_bSolidM ? irr::video::EMT_SOLID : a_pShader->getMaterial(l_eType), 
          true,
          l_bMarble ? shaders::enObjectType::Marble : a_eType
        );
      }
      else if (l_eMaterial == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL) {
        a_pShader->addNodeMaterial(a_pNode, a_iMaterial, irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL, true, a_eType);
      }
    }

    void addNodeToShader(shaders::CDustbinShaders *a_pShader, irr::scene::ISceneNode* a_pNode) {
      if (a_pShader != nullptr) {
        if (a_pNode->getType() == irr::scene::ESNT_MESH) {
          shaders::enObjectType l_eType = shaders::enObjectType::Static;

          if (a_pNode->getParent()->getType() != irr::scene::ESNT_SCENE_MANAGER && a_pNode->getParent()->getParent()->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId)
            l_eType = shaders::enObjectType::Moving;
          else {
            for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itChild = a_pNode->getChildren().begin(); l_itChild != a_pNode->getChildren().end(); l_itChild++) {
              if ((*l_itChild)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_PhysicsNodeId) {
                for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itGrarnChild = (*l_itChild)->getChildren().begin(); l_itGrarnChild != (*l_itChild)->getChildren().end(); l_itGrarnChild++) {
                  if ((*l_itGrarnChild)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_JointNodeId) {
                    l_eType = shaders::enObjectType::Moving;
                    break;
                  }
                }
              }
            }
          }

          bool l_bVisible = true;

          irr::scene::ISceneNode *p = a_pNode;
          while (p != a_pShader->getSceneManager()->getRootSceneNode() && l_bVisible) {
            l_bVisible = p->isVisible();
            p = p->getParent();
          }

          if (l_bVisible) {
            irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(a_pNode);
            for (irr::u32 i = 0; i < a_pNode->getMaterialCount(); i++)
              addMaterialToShader(a_pShader, l_pNode, i, l_eType);
          }
        }

        for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itChild = a_pNode->getChildren().begin(); l_itChild != a_pNode->getChildren().end(); l_itChild++) {
          addNodeToShader(a_pShader, *l_itChild);
        }
      }
    }

    /**
    * Convert the shadow setting to the values for the shader
    * @param a_iShadows the shadow setting
    * @param a_pShader the shader instance to be adjusted
    */
    void convertForShader(int a_iShadows, shaders::CDustbinShaders* a_pShader) {
      if (a_pShader != nullptr) {
        switch (a_iShadows) {
          // Off
          case 0:
            a_pShader->setRenderOptions(shaders::enShadowRender::Off, shaders::enShadowMode::Off, shaders::enShadowQuality::Poor);
            break;

          // Static Solid
          case 1:
            a_pShader->setRenderOptions(shaders::enShadowRender::Static, shaders::enShadowMode::Solid, shaders::enShadowQuality::LoMid);
            break;

          // Static Transparent
          case 2:
            a_pShader->setRenderOptions(shaders::enShadowRender::Static, shaders::enShadowMode::TransColor, shaders::enShadowQuality::HiMid);
            break;

          // Low
          case 3:
            a_pShader->setRenderOptions(shaders::enShadowRender::All, shaders::enShadowMode::Solid, shaders::enShadowQuality::LoMid);
            break;

          // Medium
          case 4:
            a_pShader->setRenderOptions(shaders::enShadowRender::All, shaders::enShadowMode::TransColor, shaders::enShadowQuality::HiMid);
            break;

          // High
          default:
            a_pShader->setRenderOptions(shaders::enShadowRender::All, shaders::enShadowMode::TransColor, shaders::enShadowQuality::High);
            break;
        }
      }
    }

#ifdef _OPENGL_ES
    /**
    * Adjust the materials of the node to get proper lighting when using
    * with OpenGL-ES on the raspberry PI
    * @param a_pNode the node to adjust
    */
    void adjustNodeMaterials(irr::scene::ISceneNode* a_pNode) {
      for (irr::u32 i = 0; i < a_pNode->getMaterialCount(); i++) {
        a_pNode->getMaterial(i).Lighting = false;
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
        adjustNodeMaterials(*it);
    }
#endif
  }
}