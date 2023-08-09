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
    * Load the AI players from ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
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

              float l_fDeviation = l_pXml->getAttributeValueAsFloat(L"deviation", 0.0f);

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
  }
}