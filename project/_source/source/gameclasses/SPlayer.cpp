// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gameclasses/SPlayer.h>

namespace dustbin {
  namespace gameclasses {
    /**
    * The default constructor
    */
    SPlayer::SPlayer() :
      m_iPlayer        (0),
      m_iId            (-1),
      m_iPosition      (99),
      m_iLastPosUpdate (0),
      m_iDiffLeader    (0),
      m_iDiffAhead     (0),
      m_sName          (""),
      m_sTexture       (""),
      m_sController    (""),
      m_sShortName     (""),
      m_sNumber        (L""),
      m_bWithdrawn     (false),
      m_bShowRanking   (true),
      m_bAutoThrottle  (true),
      m_iState         (0),
      m_eType          (data::enPlayerType::Local),
      m_eAiHelp        (data::SPlayerData::enAiHelp::Off),
      m_pMarble        (nullptr),
      m_pController    (nullptr)
    {
      m_cText = irr::video::SColor(0xFF,    0,    0,    0);
      m_cBack = irr::video::SColor(0x80, 0xFF, 0xFF, 0xFF);
      m_cFrme = irr::video::SColor(0x80,    0,    0,    0);
    }

    /**
    * The destructor
    */
    SPlayer::~SPlayer() {
      if (m_pMarble != nullptr)
        delete m_pMarble;

      if (m_pController != nullptr)
        delete m_pController;
    }

    /**
    * The main constructor
    * @param a_iPlayer the player id
    * @param a_sName the name of the player
    * @param a_sTexture the texture string of the player's marble
    * @param a_sController the controller configuration string of the player
    * @param a_pMarble the marble of the player
    */
    SPlayer::SPlayer(int a_iPlayer, const std::string& a_sName, const std::string& a_sTexture, const std::string &a_sController, const std::string &a_sShortName, bool a_bAutoThrottle, data::SPlayerData::enAiHelp a_eAiHelp, gameclasses::SMarbleNodes* a_pMarble, data::enPlayerType a_eType) :
      m_iPlayer       (a_iPlayer),
      m_iPosition     (99),
      m_iLastPosUpdate(0),
      m_iDiffLeader   (0),
      m_iDiffAhead    (0),
      m_sName         (a_sName),
      m_sWName        (helpers::s2ws(a_sName)),
      m_sTexture      (a_sTexture),
      m_sController   (a_sController),
      m_sShortName    (a_sShortName),
      m_bWithdrawn    (false),
      m_eType         (a_eType),
      m_eAiHelp       (a_eAiHelp),
      m_pMarble       (a_pMarble),
      m_pController   (nullptr)
    {

      if (m_pMarble != nullptr && m_pMarble->m_pPositional != nullptr)
        m_iId = m_pMarble->m_pPositional->getID();
      else
        m_iId = -1;

      if (m_pMarble != nullptr) {
        m_pMarble->m_pRotational->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(m_sTexture));
      }
      else printf("Empty texture string.\n");

      std::string l_sType = "";

      std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, m_sTexture);
      m_sNumber = helpers::s2ws(helpers::findTextureParameter(l_mParams, "number"));

      if (l_sType == "default") {
        l_mParams = helpers::parseParameters(l_sType, helpers::createDefaultTextureString(helpers::ws2s(m_sNumber), 0));
      }

      if (helpers::findTextureParameter(l_mParams, "numbercolor" ) != "") helpers::fillColorFromString(m_cText, helpers::findTextureParameter(l_mParams, "numbercolor" ));
      if (helpers::findTextureParameter(l_mParams, "numberback"  ) != "") helpers::fillColorFromString(m_cBack, helpers::findTextureParameter(l_mParams, "numberback"  ));
      if (helpers::findTextureParameter(l_mParams, "numberborder") != "") helpers::fillColorFromString(m_cBack, helpers::findTextureParameter(l_mParams, "numberborder"));

      m_cBack.setAlpha(0x80);
      m_cFrme.setAlpha(0x80);
    }

    void SPlayer::setName(const std::string& a_sName) {
      m_sName  =                       a_sName;
      m_sWName = helpers::s2ws(a_sName);
    }

    bool SPlayer::isBot() {
      return m_eAiHelp == data::SPlayerData::enAiHelp::BotMgp || m_eAiHelp == data::SPlayerData::enAiHelp::BotMb2 || m_eAiHelp == data::SPlayerData::enAiHelp::BotMb3;
    }
  }
}