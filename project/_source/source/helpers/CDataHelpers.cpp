// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <helpers/CTextureHelpers.h>
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
      return "DustbinController;control;f%3bl%3bForward%3bh%3ba%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bBackward%3bh%3ba%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb%3bF%3bmZm;control;f%3bl%3bLeft%3bh%3ba%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b%3bF%3bmZm;control;f%3bl%3bRight%3bh%3ba%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bBrake%3bh%3ba%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRearview%3bh%3ba%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bRespawn%3bh%3ba%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bPause%3bh%3ba%3bn%3bqb%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb%3bF%3bmZm;control;f%3bl%3bCancel%2520Race%3bh%3ba%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb%3bF%3bmZm";
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
    * Prepare and update the global data for the next race
    * @param a_sTrack the next track identifier
    * @param a_sInfo additional race information
    * @param a_iLaps the number of laps
    */
    void prepareNextRace(const std::string& a_sTrack, const std::string &a_sInfo, int a_iLaps) {
      data::SGameData l_cData(a_sTrack, a_sInfo, a_iLaps);

      CGlobal *l_pGlobal = CGlobal::getInstance();

      data::SGameSettings l_cSettings;
      l_cSettings.deserialize(l_pGlobal->getSetting("gamesetup"));
      data::SChampionship l_cChampionship = data::SChampionship(l_pGlobal->getGlobal("championship"));
      data::SRacePlayers l_cPlayers;

      l_cPlayers.deserialize(l_pGlobal->getGlobal("raceplayers"));

      // The grid positions of the first race (or fixed order) have already been set in "CMenuFillGrid"
      if (l_cChampionship.m_vRaces.size() > 0 && l_cSettings.m_eGridPos != data::SGameSettings::enGridPos::Fixed) {
        // Now we need to specify the starting grid of the next race
        switch (l_cSettings.m_eGridPos) {
          case data::SGameSettings::enGridPos::LastRace:
            for (int i = 0; i < l_cChampionship.m_vPlayers.size() && i < 16; i++) {
              if (l_cChampionship.m_vRaces.back().m_mAssignment.find(l_cChampionship.m_vRaces.back().m_aResult[i].m_iId) != l_cChampionship.m_vRaces.back().m_mAssignment.end()) {
                l_cData.m_vStartingGrid.push_back(l_cChampionship.m_vRaces.back().m_mAssignment[l_cChampionship.m_vRaces.back().m_aResult[i].m_iId]);
              }
            }

            break;

          case data::SGameSettings::enGridPos::Standings: {
            std::vector<data::SChampionshipPlayer> l_vStanding = l_cChampionship.getStandings();

            for (std::vector<data::SChampionshipPlayer>::iterator l_itPlayer = l_vStanding.begin(); l_itPlayer != l_vStanding.end(); l_itPlayer++) {
              l_cData.m_vStartingGrid.push_back((*l_itPlayer).m_iPlayerId);
            }

            break;
          }

          case data::SGameSettings::enGridPos::Random: {
            for (std::vector<data::SChampionshipPlayer>::iterator l_itPlayer = l_cChampionship.m_vPlayers.begin(); l_itPlayer != l_cChampionship.m_vPlayers.end(); l_itPlayer++) {
              l_cData.m_vStartingGrid.push_back((*l_itPlayer).m_iPlayerId);
            }
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };

            std::shuffle(l_cData.m_vStartingGrid.begin(), l_cData.m_vStartingGrid.end(), l_cRe);
            break;
          }

          default:
            break;
        }

        if ((l_cSettings.m_eGridPos == data::SGameSettings::enGridPos::LastRace || l_cSettings.m_eGridPos == data::SGameSettings::enGridPos::Standings) && l_cSettings.m_bReverseGrid) {
          std::reverse(l_cData.m_vStartingGrid.begin(), l_cData.m_vStartingGrid.end());
        }
      }
      else {
        std::sort(l_cPlayers.m_vPlayers.begin(), l_cPlayers.m_vPlayers.end(), [](data::SPlayerData& a_Pl1, data::SPlayerData& a_Pl2) {
          return a_Pl1.m_iGridPos < a_Pl2.m_iGridPos;
        });

        for (auto l_cPlr: l_cPlayers.m_vPlayers)
          l_cData.m_vStartingGrid.push_back(l_cPlr.m_iPlayerId);
      }

      printf("%s\n", l_cData.toString().c_str());

      l_pGlobal->setGlobal("gamedata", l_cData.serialize());
      l_pGlobal->initNextRaceScreen();
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

#ifdef _WINDOWS
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

      std::vector<int> l_vShadowMap = { 2048, 4096, 8192 };

      std::vector<irr::core::recti> l_vViewports;

      irr::core::dimension2du l_cScreen = l_pDrv->getScreenSize();
      irr::core::dimension2du l_cDim = irr::core::dimension2du(l_cScreen.Width, l_cScreen.Height);

      irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(enFont::Big, l_cDim);

      irr::core::dimension2du l_cTextSize = l_pFont->getDimension(L"Detect Framerate: 6666 Frames.");

      irr::core::recti l_cTextRect = irr::core::recti(
        irr::core::vector2di(l_cDim.Width / 2 - l_cTextSize.Width / 2, l_cDim.Height / 2 - l_cTextSize.Height / 2),
        irr::core::vector2di(l_cDim.Width / 2 + l_cTextSize.Width / 2, l_cDim.Height / 2 + l_cTextSize.Height / 2)
      );

      l_cTextRect.UpperLeftCorner  -= irr::core::vector2di(l_cTextSize.Height / 6);
      l_cTextRect.LowerRightCorner += irr::core::vector2di(l_cTextSize.Height / 6);

      l_vViewports.push_back(irr::core::recti(irr::core::position2di(0, 0), l_cDim));

      for (auto l_iShadowMap: l_vShadowMap) {
        l_pSmgr->clear();
        l_pSmgr->loadScene("data/levels/four_obstacles/track.xml");

        irr::scene::ISceneNode *l_pNode = l_pSmgr->getSceneNodeFromName("AutoGfxCamera");

        if (l_pNode != nullptr && l_pNode->getType() == irr::scene::ESNT_CAMERA) {
          l_pSmgr->setActiveCamera(reinterpret_cast<irr::scene::ICameraSceneNode *>(l_pNode));
        }

        irr::u32 l_iStart = l_pTimer->getRealTime() + 1000;
        irr::u32 l_iFrame = 0;

        bool l_bCount = false;

        while (a_pDevice->run()) {
          l_pDrv->beginScene(true, true);
          l_pSmgr->drawAll();

          l_pDrv->draw2DRectangle(irr::video::SColor(192, 224, 244, 224), l_cTextRect);
          l_pDrv->draw2DRectangleOutline(l_cTextRect, irr::video::SColor(255, 0, 0, 0));
          std::wstring l_sInfo = L"Detect Framerate: " + std::to_wstring(l_iFrame) + L" Frames.";
          l_pFont->draw(l_sInfo.c_str(), l_cTextRect, irr::video::SColor(255, 0, 0, 0), true, true);

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
              printf("Shadow map size %4i: %i Frames (%i / sec).\n", l_iShadowMap, l_iFrame, l_iFrame / 4);
              if (l_iFrame / 4 > 100) {
                a_pSettings->m_iShadows++;
              }
              else return;
              break;
            }
          }
        }
      }
      l_pSmgr->clear();
      printf("Shadow settings: %i\n", a_pSettings->m_iShadows);
    }
#endif

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