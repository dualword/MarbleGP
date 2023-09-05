// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <platform/CPlatform.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <Defines.h>

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
  }
}