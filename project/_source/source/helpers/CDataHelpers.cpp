// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <helpers/CTextureHelpers.h>
#include <shaders/CDustbinShaders.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <platform/CPlatform.h>
#include <json/CIrrJSON.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <Defines.h>
#include <fstream>
#include <random>

namespace dustbin {
  namespace helpers {
    void saveProfiles(std::vector<data::SPlayerData>& a_cProfiles) {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iProfileHead);
      l_cSerializer.addString(c_sProfileHead);

      l_cSerializer.addS32(c_iAllProfileEnd);

      int l_iNum = 1;

      for (auto& l_cProfile : a_cProfiles) {
        printf("Save profile \"%s\"...\n", l_cProfile.m_sName.c_str());
        l_cSerializer.addS32(c_iProfileStart);
        l_cProfile.m_iPlayerId = l_iNum;
        l_cSerializer.addString(l_cProfile.serialize());
        l_cSerializer.addS32(c_iProfileEnd);
        l_iNum++;
      }

      CGlobal::getInstance()->setSetting("profiles", l_cSerializer.getMessageAsString());
    }

    /**
    * Save the AI profiles to ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
    * @return true if saving succeeded, false otherwise
    */
    bool saveAiProfiles(const std::vector<std::tuple<std::string, std::string, std::string, int, int, float>>& a_vAiPlayers) {
      bool l_bRet = false;

      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();
      std::string l_sFile = helpers::ws2s(platform::portableGetDataPath() + L"ai_profiles.xml");

      irr::io::IWriteFile *l_pFile = l_pFs->createAndWriteFile(l_sFile.c_str());

      if (l_pFile) {
        std::string l_sBuffer = "<?xml version=\"1.0\" ?>\n";
        l_pFile->write(l_sBuffer.c_str(), l_sBuffer.size());

        l_sBuffer = "<marblegp>\n";
        l_pFile->write(l_sBuffer.c_str(), l_sBuffer.size());

        for (auto l_cProfile : a_vAiPlayers) {
          char s[0xFF];
          sprintf(s, "%.2f", std::get<5>(l_cProfile));
          l_sBuffer = "  <ai_player name=\"" + std::get<0>(l_cProfile) + "\" abbreviation=\"" + std::get<1>(l_cProfile) + "\" texture=\"" + std::get<2>(l_cProfile) + "\" class1=\"" + std::to_string(std::get<3>(l_cProfile)) + "\" class2=\"" + std::to_string(std::get<4>(l_cProfile)) + "\" deviation=\"" + s + "\" / >\n";

          l_pFile->write(l_sBuffer.c_str(), l_sBuffer.size());
        }

        l_sBuffer = "</marblegp>\n";
        l_pFile->write(l_sBuffer.c_str(), l_sBuffer.size());
        l_pFile->drop();

        l_bRet = true;
      }

      return l_bRet;
    }

    /**
    * Load the AI players from ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
    * @return true if loading succeeded, false otherwise
    */
    bool loadAiProfiles(std::vector<std::tuple<std::string, std::string, std::string, int, int, float>>& a_vAiPlayers) {
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

      std::string l_sFile = helpers::ws2s(platform::portableGetDataPath() + L"/ai_profiles.xml");

      if (!l_pFs->existFile(l_sFile.c_str())) {
        l_sFile = "data/ai_profiles.xml";
      }

      int l_iNum = 1;

      if (l_pFs->existFile(l_sFile.c_str())) {
        irr::io::IXMLReader *l_pXml = l_pFs->createXMLReader(l_sFile.c_str());

        if (l_pXml != nullptr) {
          while (l_pXml->read()) {
            if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT && std::wstring(l_pXml->getNodeName()) == L"ai_player") {
              std::string l_sName    = helpers::ws2s(l_pXml->getAttributeValueSafe(L"name"));
              std::string l_sShort   = helpers::ws2s(l_pXml->getAttributeValueSafe(L"abbreviation"));
              std::string l_sTexture = helpers::ws2s(l_pXml->getAttributeValueSafe(L"texture"));

              int l_iClass1  = l_pXml->getAttributeValueAsInt(L"class1", 0);
              int l_iClass2  = l_pXml->getAttributeValueAsInt(L"class2", 0);

              float l_fDeviation = std::max(-2.5f, std::min(2.5f, l_pXml->getAttributeValueAsFloat(L"deviation", 0.0f)));

              if (l_sName    == "") l_sName    = "Ai Player #" + std::to_string(l_iNum);
              if (l_sShort   == "") l_sShort   = "Ai#" + std::to_string(l_iNum);
              if (l_sTexture == "") l_sTexture = std::to_string(l_iNum);

              a_vAiPlayers.push_back(std::make_tuple(l_sName, l_sShort, l_sTexture, l_iClass1, l_iClass2, l_fDeviation));

              l_iNum++;
            }
          }

          l_pXml->drop();
        }
      }
      else return false;

      return true;
    }

    /**
    * Get the data of all tracks
    * @return a std map with the track folder as key and the corresponding name as value
    */
    std::map<std::string, std::string> getTrackNameMap() {
      std::map<std::string, std::string> l_mResult;

      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

      irr::io::IReadFile *l_pFile = l_pFs->createAndOpenFile("data/levels/tracks.dat");

      if (l_pFile != nullptr) {
        long l_iSize = l_pFile->getSize();
        char *s = new char[l_iSize + 1];
        memset(s, 0, static_cast<size_t>(l_iSize) + 1);
        l_pFile->read(s, l_iSize);
        std::vector<std::string> l_vTracks = helpers::splitString(s, '\n');
        delete []s;

        for (std::vector<std::string>::iterator it = l_vTracks.begin(); it != l_vTracks.end(); it++) {
          std::string l_sTrack = *it;
          if (l_sTrack.substr(l_sTrack.size() - 1) == "\r")
            l_sTrack = l_sTrack.substr(0, l_sTrack.size() - 1);

          std::string l_sFile = "data/levels/" + l_sTrack + "/track.xml";

          if (l_pFs->existFile(l_sFile.c_str())) {
            std::string l_sXml = "data/levels/" + l_sTrack + "/info.xml", l_sName = l_sTrack;

            if (l_pFs->existFile(l_sXml.c_str())) {
              irr::io::IXMLReaderUTF8 *l_pXml = l_pFs->createXMLReaderUTF8(l_sXml.c_str());
              if (l_pXml) {
                bool l_bName = false;

                while (l_pXml->read()) {
                  std::string l_sNode = l_pXml->getNodeName();

                  if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                    if (l_sNode == "name")
                      l_bName = true;
                  }
                  else if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                    if (l_bName)
                      l_mResult[l_sTrack] = l_pXml->getNodeData();
                  }
                  else if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
                    if (l_sNode == "name")
                      l_bName = false;
                  }
                }
                l_pXml->drop();
              }
            }
          }
        }

        l_pFile->drop();
      }
      return l_mResult;
    }


    /**
    * Get the possible AI help options
    * @return a vector with the possible AI help options
    */
    std::vector<std::string> getAiHelpOptions() {
      return std::vector<std::string> { "Off", "Display", "Low", "Medium", "High", "Bot (MarbleGP)", "Bot (Marble2)", "Bot (Marble3)" };
    }

    /**
    * Get the string value for an AI help option
    */
    std::string getAiHelpString(data::SPlayerData::enAiHelp a_eAiHelp) {
      switch (a_eAiHelp)
      {
        case dustbin::data::SPlayerData::enAiHelp::Off    : return "Off"           ; break;
        case dustbin::data::SPlayerData::enAiHelp::Display: return "Display"       ; break;
        case dustbin::data::SPlayerData::enAiHelp::Low    : return "Low"           ; break;
        case dustbin::data::SPlayerData::enAiHelp::Medium : return "Medium"        ; break;
        case dustbin::data::SPlayerData::enAiHelp::High   : return "High"          ; break;
        case dustbin::data::SPlayerData::enAiHelp::BotMgp : return "Bot (MarbleGP)"; break;
        case dustbin::data::SPlayerData::enAiHelp::BotMb2 : return "Bot (Marble2)" ; break;
        case dustbin::data::SPlayerData::enAiHelp::BotMb3 : return "Bot (Marble3)" ; break;
        default:
          return "";
          break;
      }
    }

    /**
    * Get the string of the controller type
    */
    std::wstring getControllerType(const std::string& a_sCtrl) {
      if (a_sCtrl == "DustbinTouchControl")
        return L"Touch Controller";
      else if (a_sCtrl == "DustbinGyroscope")
        return L"Gyroscope";
      else {
        controller::CControllerBase l_pCtrl;
        l_pCtrl.deserialize(a_sCtrl);
        if (l_pCtrl.usesJoystick())
          return L"Gamepad";
        else
          return L"Keyboard";
      }

      return L"";
    }

    /**
    * Get the default controller string for Keyboard game controls
    * @return the default controller string for Keyboard game controls
    */
    std::string getDefaultGameCtrl_Keyboard() {
#ifdef _WINDOWS
      return "DustbinController;control;f%3bl%3bForward%3bh%3ba%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bBackward%3bh%3ba%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb%3bF%3bmZm;control;f%3bl%3bLeft%3bh%3ba%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bRight%3bh%3ba%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bBrake%3bh%3ba%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRearview%3bh%3ba%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRespawn%3bh%3ba%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bPause%3bh%3ba%3bn%3bqb%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bCancel%2520Race%3bh%3ba%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb%3bF%3bmZm";
#else
      return "DustbinTouchControl";
#endif
    }

    /**
    * Get the default controller string for Gamepad game controls
    * @return the default controller string for Keyboard game controls
    */
    std::string getDefaultGameCtrl_Gamepad() {
      return "DustbinController;control;f%3bl%3bForward%3bh%3bb%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bBackward%3bh%3bb%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb%3bF%3bmZm;control;f%3bl%3bLeft%3bh%3bb%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bRight%3bh%3bb%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bBrake%3bh%3bc%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRearview%3bh%3bc%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRespawn%3bh%3bc%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bPause%3bh%3bc%3bn%3bt%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bCancel%2520Race%3bh%3bc%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb%3bF%3bmZm";
    }

    /**
    * Get the icon folder depending on the viewport height
    * @param a_iHeight the viewport height
    * @return the corresponding icon folder
    */
    std::string getIconFolder(irr::s32 a_iHeight) {
      std::vector<int> l_vHeight;

      irr::io::IReadFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile("data/icons/icons.txt");
      if (l_pFile != nullptr) {
        char *p = new char[l_pFile->getSize() + 1];
        memset(p, 0, l_pFile->getSize() + 1);

        l_pFile->read(p, l_pFile->getSize());

        std::vector<std::string> l_vLines = helpers::splitString(p, '\n');

        for (auto l_sLine : l_vLines) {
          if (l_sLine != "")
            l_vHeight.push_back(std::atoi(l_sLine.c_str()));
        }

        l_pFile->drop();
        delete[] p;
      }
      else return "data/images/";

      if (l_vHeight.size() > 0) {
        int l_iHeight = *l_vHeight.begin();

        for (auto l_itHeight : l_vHeight) {
          if (l_itHeight < a_iHeight)
            break;
        
          l_iHeight = l_itHeight;
        }

        return "data/icons/" + std::to_string(l_iHeight) + "/";
      }
      else return "data/images/";
    }


    /**
    * Load all cup definitions
    * @return a vector with all cup definitions
    */
    std::vector<data::SMarbleGpCup> loadCupDefinitions() {
      std::vector<data::SMarbleGpCup> l_vCups;

      std::string l_aFiles[] = {
        "data/cups.json",
        helpers::ws2s(platform::portableGetDataPath()) + "/cups.json",
        ""
      };

      for (int i = 0; l_aFiles[i] != ""; i++) {
        irr::io::IReadFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(l_aFiles[i].c_str());

        if (l_pFile != nullptr) {
          json::CIrrJSON *l_pJson = new json::CIrrJSON(l_pFile);

          int l_iState = 0;

          while (l_pJson->read()) {
            switch (l_iState) {
              case 0:
                if (l_pJson->getType() == json::CIrrJSON::enToken::ArrayStart)
                  l_iState = 1;
                break;

              case 1:
                if (l_pJson->getType() == json::CIrrJSON::enToken::ObjectStart) {
                  l_vCups.push_back(data::SMarbleGpCup(l_pJson));
                  l_vCups.back().m_bUserDefined = i != 0;
                }
                break;
            }
          }

          delete l_pJson;
          l_pFile->drop();
        }
      }

      return l_vCups;
    }

    /**
    * Read the lines of a text file
    * @param a_sPath the path of the file to read
    * @return the lines of the file in a std::vector
    */
    std::vector<std::string> readLinesOfFile(const std::string& a_sPath) {
      std::string l_sDummy = "";

      irr::io::IReadFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(a_sPath.c_str());

      if (l_pFile != nullptr) {
        char *l_pBuffer = new char[l_pFile->getSize() + 1];
        memset(l_pBuffer, 0, l_pFile->getSize() + 1);
        l_pFile->read(l_pBuffer, l_pFile->getSize());

        l_sDummy = l_pBuffer;

        delete []l_pBuffer;
        l_pFile->drop();
      }

      return helpers::splitString(l_sDummy, '\n');
    }

   /**
    * Function for auto-detection of suitable game graphics settings
    * @param a_pDevice the Irrlicht device
    * @param a_pSettings the settings to be adjusted
    */
    void gfxAutoDetection(irr::IrrlichtDevice* a_pDevice, data::SSettings* a_pSettings) {
      irr::video::IVideoDriver  *l_pDrv  = a_pDevice->getVideoDriver   ();
      irr::scene::ISceneManager *l_pSmgr = a_pDevice->getSceneManager  ();
      irr::gui::IGUIEnvironment *l_pGui  = a_pDevice->getGUIEnvironment();

      a_pSettings->m_iShadows = 0;

      irr::ITimer *l_pTimer = a_pDevice->getTimer();

      std::vector<irr::core::recti> l_vViewports;

      irr::core::dimension2du l_cScreen = l_pDrv->getScreenSize();
      irr::core::dimension2du l_cDim = irr::core::dimension2du(l_cScreen.Width, l_cScreen.Height);

      irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(enFont::Big, l_cDim);

      l_vViewports.push_back(irr::core::recti(irr::core::position2di(0, 0), l_cDim));

      l_pSmgr->clear();
      l_pSmgr->loadScene("data/levels/four_obstacles/track.xml");

      irr::scene::ISceneNode *l_pEmpty = l_pSmgr->addEmptySceneNode();
      l_pEmpty->setPosition(irr::core::vector3df(0.0f, 35.0f, 0.0f));
      irr::scene::ICameraSceneNode *l_pCamera = l_pSmgr->addCameraSceneNode(l_pEmpty, irr::core::vector3df(550.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f));
      irr::scene::ISceneNodeAnimator *l_pAnim = l_pSmgr->createRotationAnimator(irr::core::vector3df(0.0f, 0.125f, 0.0f));
      l_pEmpty->addAnimator(l_pAnim);
      l_pAnim->drop();

      shaders::CDustbinShaders *l_pShader = CGlobal::getInstance()->getShader();
      l_pShader->addLightCamera();

      helpers::addNodeToShader(
        l_pShader, 
        l_pSmgr->getRootSceneNode(), 
        nullptr, 
        helpers::countMeshSceneNodes(l_pSmgr->getRootSceneNode())
      );

      std::wstring l_sHeadline = L"Framerate Detection";

      //          Text       Frames FPS         Size                 Position               Position FPS
      std::tuple<std::wstring, int, int, irr::core::dimension2du, irr::core::vector2di, irr::core::vector2di> l_aData[] = {
        std::make_tuple(L"Shadows Off: ", 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L"Static Low: " , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L"Static High: ", 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L"Low: "        , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L"Medium: "     , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L"High: "       , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
        std::make_tuple(L""             , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di())
      };

      irr::core::dimension2du l_cHead = l_pFont->getDimension(l_sHeadline.c_str());
      irr::core::dimension2du l_cText;

      irr::core::vector2di l_cPos = irr::core::vector2di(0, 5 * l_cHead.Height / 2);

      for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
        std::get<3>(l_aData[i]) = l_pFont->getDimension(std::get<0>(l_aData[i]).c_str());
        std::get<4>(l_aData[i]) = irr::core::vector2di(0, l_cPos.Y);

        if (std::get<3>(l_aData[i]).Width  > l_cText.Width ) l_cText .Width  = std::get<3>(l_aData[i]).Width;
        if (std::get<3>(l_aData[i]).Height > l_cText.Height) l_cText .Height = std::get<3>(l_aData[i]).Height;

        l_cPos.Y += 5 * l_cHead.Height / 4;
      }

      for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
        std::get<4>(l_aData[i]).X = l_cScreen.Width / 2 - std::get<3>(l_aData[i]).Width;
      }

      l_cPos.Y += l_cHead.Height / 2;

      irr::core::dimension2du l_cBoxSize = irr::core::dimension2du(2 * l_cText.Width, l_cPos.Y);

      irr::core::recti l_cBox = irr::core::recti(
        irr::core::vector2di(l_cScreen.Width / 2 - l_cBoxSize.Width / 2 - l_cHead.Height, l_cScreen.Height / 2 - l_cBoxSize.Height / 2),
        l_cBoxSize + irr::core::dimension2du(2 * l_cHead.Height, 0)
      );

      for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
        std::get<4>(l_aData[i]).Y += l_cBox.UpperLeftCorner.Y;

        std::get<5>(l_aData[i]) = irr::core::vector2di(l_cScreen.Width / 2, std::get<4>(l_aData[i]).Y);
      }

      irr::core::dimension2du l_cFps = irr::core::dimension2du(l_cBox.getWidth() / 2, l_cHead.Height);

      irr::u32 l_iFlags = 0;
      for (int i = 0; i < 7; i++) {
        if (i < 6) 
          l_iFlags = helpers::convertForShader(i, l_pShader);

        irr::u32 l_iStart = l_pTimer->getRealTime() + 1000;
        irr::u32 l_iFrame = 0;

        bool l_bCount = false;

        l_pShader->clearShadowMaps();
        l_pShader->startShadowMaps();
        l_pShader->renderShadowMap((irr::u32)shaders::enShadowMap::TranspColor | (irr::u32)shaders::enShadowMap::Transparent | (irr::u32)shaders::enShadowMap::Solid);
        l_pShader->endShadowMaps();

        while (a_pDevice->run()) {
          if (l_iFlags != 0) {
            l_pShader->startShadowMaps();
            l_pShader->renderShadowMap(l_iFlags);
            l_pShader->endShadowMaps();
          }

          l_pDrv->beginScene(true, true);
          l_pShader->renderScene();

          l_pDrv->draw2DRectangle(irr::video::SColor(192, 192, 192, 192), l_cBox);
          l_pFont->draw(l_sHeadline.c_str(), 
            irr::core::recti(
              l_cBox.UpperLeftCorner + irr::core::vector2di(0, l_cHead.Height / 2), 
              irr::core::dimension2du(l_cBox.getWidth(), l_cHead.Height)
            ),
            irr::video::SColor(0xFF, 0, 0, 0),
            true, 
            true
          );

          for (int j = 0; std::get<0>(l_aData[j]) != L""; j++) {
            l_pFont->draw(
              std::get<0>(l_aData[j]).c_str(), 
              irr::core::recti(
                std::get<4>(l_aData[j]), 
                std::get<3>(l_aData[j])
              ),
              i == j ? irr::video::SColor(0xFF, 0x80, 0x40, 0x00) : irr::video::SColor(0xFF, 0, 0, 0),
              false,
              true
            );

            if (j == i) {
              if (!l_bCount) {
                l_pFont->draw(L"Framecount", irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0x80, 0x40, 0x00), false, true);
              }
              else {
                l_pFont->draw(std::to_wstring(l_iFrame).c_str(), irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0x80, 0x40, 0x00), false, true);
              }
            }
            else if (j < i) {
              l_pFont->draw((std::to_wstring(std::get<2>(l_aData[j])) + L" FPS").c_str(), irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0, 0, 0), false, true);
            }
            else {
              l_pFont->draw(L"---", irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0, 0, 0), false, true);
            }
          }

          l_pDrv->endScene();
          std::wstring s = L"Current Framerate: " + std::to_wstring(l_pDrv->getFPS()) + L" FPS";
          a_pDevice->setWindowCaption(s.c_str());

          if (!l_bCount) {
            if (l_pTimer->getRealTime() > l_iStart) {
              printf("Start Frame Count...\n");
              l_bCount = true;
            }
          }
          else {
            l_iFrame++;
            if (l_pTimer->getRealTime() > l_iStart + 4000) {
#ifdef _WINDOWS
              if (l_iFrame / 4 > 100)
#else
              if (l_iFrame / 4 > 30)
#endif
                a_pSettings->m_iShadows = i;
              else 
                i = 6;

              std::get<2>(l_aData[i]) = l_iFrame / 4;
              break;
            }
          }
        }
      }
      l_pSmgr->clear();
      l_pShader->clear();
      printf("Shadow settings: %i\n", a_pSettings->m_iShadows);
    }

    std::vector<std::tuple<std::string, std::string>> g_vDefaultNames;     /**< Vector with the default names (0 == first name, 1 == surname) */

    /**
    * Generate a random profile
    * @param a_sName [out] name of the profile
    * @param a_sShort [out] abbreviation of the name
    * @param a_sTexture [out] texture of the profile
    */
    void createRandomProfile(std::string& a_sName, std::string& a_sShort) {
      if (g_vDefaultNames.size() == 0) {
        std::vector<std::string> l_vNames = helpers::readLinesOfFile("data/names.txt");

        for (auto l_sName : l_vNames) {
          std::vector<std::string> l_vSplit = helpers::splitString(l_sName, ',');

          std::string l_sFirstName = l_vSplit.size() > 0 ? l_vSplit[0] : "";
          std::string l_sSurName   = l_vSplit.size() > 0 ? l_vSplit[1] : "";

          g_vDefaultNames.push_back(std::make_tuple(l_sFirstName, l_sSurName));
        }
      }

      if (g_vDefaultNames.size() > 0) {
        std::string l_sFirstName = std::get<0>(g_vDefaultNames[std::rand() % g_vDefaultNames.size()]);
        std::string l_sSurName   = std::get<1>(g_vDefaultNames[std::rand() % g_vDefaultNames.size()]);

        std::string l_sName = l_sFirstName + " " + l_sSurName;

        a_sName  = l_sName;
        a_sShort = l_sFirstName.substr(0, 2) + l_sSurName.substr(0, 3);
      }
      else printf("No default names loaded.\n");
    }

    std::vector<std::tuple<std::string, std::string, std::string, std::string>> g_vDefaultColors;     /**< The default colors for the random textures */
    std::vector<std::string>                                                    g_vDefaultPatterns;   /**< A List of the available patterns */

    /**
    * Generate a random texture
    * @return a random texture string
    */
    std::string createRandomTexture() {
      std::string l_sRet = "";

      if (g_vDefaultColors.size() == 0) {
        std::vector<std::string> l_vColors = helpers::readLinesOfFile("data/colors.txt");

        for (std::vector<std::string>::iterator l_itColor = l_vColors.begin(); l_itColor != l_vColors.end(); l_itColor++) {
          std::vector<std::string> l_vDefault = helpers::splitString(*l_itColor, ',');
          while (l_vDefault.size() < 4)
            l_vDefault.push_back("");
          g_vDefaultColors.push_back(std::make_tuple(l_vDefault[0], l_vDefault[1], l_vDefault[2], l_vDefault[3]));
        }
      }

      if (g_vDefaultPatterns.size() == 0) {
        g_vDefaultPatterns = helpers::getTexturePatterns();
      }

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(g_vDefaultColors.begin(), g_vDefaultColors.end(), l_cRe);
      }

      std::tuple<std::string, std::string, std::string, std::string> l_tColor = *g_vDefaultColors.begin();

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(g_vDefaultPatterns.begin(), g_vDefaultPatterns.end(), l_cRe);
      }

      std::string l_sPattern = *g_vDefaultPatterns.begin();

      std::vector<int> l_vIndex = { 0, 1, 2 };

      std::vector<std::vector<std::string>> l_vElements;

      if (std::get<3>(l_tColor) == "") {
        l_vElements.push_back({ "numbercolor", "ringcolor" });
        l_vElements.push_back({ "patterncolor" });
        l_vElements.push_back({ "numberback", "patternback", "numberborder" });
      }
      else {
        l_vIndex.push_back(3);

        l_vElements.push_back({ "numbercolor" });
        l_vElements.push_back({ "ringcolor" });
        l_vElements.push_back({ "patterncolor" });
        l_vElements.push_back({ "numberback", "patternback", "numberborder" });
      }

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(l_vIndex.begin(), l_vIndex.end(), l_cRe);
      }

      for (std::vector<int>::iterator l_itIndex = l_vIndex.begin(); l_itIndex != l_vIndex.end(); l_itIndex++) {
        for (std::vector<std::string>::iterator l_itPart = l_vElements[*l_itIndex].begin(); l_itPart != l_vElements[*l_itIndex].end(); l_itPart++) {
          if (l_sRet == "")
            l_sRet = "generate://";
          else
            l_sRet += "&";

          l_sRet += *l_itPart + "=";

          switch (*l_itIndex) {
            case 0: l_sRet += std::get<0>(l_tColor); break;
            case 1: l_sRet += std::get<1>(l_tColor); break;
            case 2: l_sRet += std::get<2>(l_tColor); break;
            case 3: l_sRet += std::get<3>(l_tColor); break;
          }
        }
      }

      return l_sRet + "&pattern=" + l_sPattern;
    }
  }
}