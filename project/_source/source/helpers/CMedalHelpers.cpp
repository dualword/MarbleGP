// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CMedalHelpers.h>
#include <helpers/CDataHelpers.h>
#include <gameclasses/SPlayer.h>
#include <data/CDataStructs.h>
#include <map>

namespace dustbin {
  namespace helpers {
    /**
    * Did the given player win the last race?
    * @param a_pPlayer the player
    * @param a_pTournament the tournament
    */
    static bool didWinLastRace(const data::SPlayerData* a_pPlayer, const gameclasses::STournament* a_pTournament) {
      if (a_pTournament->m_iThisRace == -1) {
        return a_pTournament->m_vRaces.back()->m_vRanking.front()->m_pPlayer->m_sName == a_pPlayer->m_sName;
      }
      else {
        if (a_pTournament->m_iThisRace < (int)a_pTournament->m_vRaces.size()) {
          return a_pTournament->m_vRaces[a_pTournament->m_iThisRace]->m_vRanking.front()->m_pPlayer->m_sName == a_pPlayer->m_sName;
        }
      }

      return false;
    }

    /**
    * Was the last race done in this tournament a black lap race (1 lap)?
    * @param a_pTournament the tournemant to check
    * @return true if the last absolved race was only one lap, false otherwise
    */
    static bool blackRace(const gameclasses::STournament* a_pTournament) {
      if (a_pTournament->m_iThisRace == -1)
        return a_pTournament->m_vRaces.back()->m_iLaps == 1;
      else
        if (a_pTournament->m_iThisRace < (int)a_pTournament->m_vRaces.size())
          return a_pTournament->m_vRaces[a_pTournament->m_iThisRace]->m_iLaps == 1;

      return false;
    }

    /**
    * Is the cup finished and did the player win or dominate it
    * @param a_pTournament the tournament
    * @param a_pPlayer the player
    * @param a_bDominate [out] did the player dominate?
    * @param a_bBlackLap [out] is this a black lap (1 lap per race) cup?
    * @return true if the cup is finished and the player won
    */
    static bool didWinCup(const gameclasses::STournament* a_pTournament, const data::SPlayerData* a_pPlayer, bool& a_bDominate, bool &a_bBlackLap) {
      a_bDominate = false;

      if (a_pTournament->m_iThisRace != -1 && a_pTournament->m_vRaces.back()->m_vRanking.size() > 0) {
        a_bDominate = true;
        a_bBlackLap = true;

        for (auto l_pRace : a_pTournament->m_vRaces) {
          if (l_pRace->m_vRanking.size() > 0) {
            if (l_pRace->m_iLaps > 1)
              a_bBlackLap = false;

            if (l_pRace->m_vRanking.front()->m_pPlayer->m_sName != a_pPlayer->m_sName)
              a_bDominate = false;
          }
        }

        int l_iWinner = a_pTournament->m_vRaces.back()->m_vStandings.front().m_iPlayer;
        for (auto l_pPlayer: a_pTournament->m_vPlayers)
          if (l_pPlayer->m_iPlayer == l_iWinner && l_pPlayer->m_sName == a_pPlayer->m_sName)
            return true;
      }
      return false;
    }

    namespace achievements {
      const std::string c_sGOAT = "GOAT";

      const std::string c_sGpBlackDominate = "BL_DOMGP";
      const std::string c_sGpBlackWinCup   = "BL_CUPGP";
      const std::string c_sGpBlackWinRace  = "BL_WINGP";
      const std::string c_sGpDominate      = "DOMGP";
      const std::string c_sGpWinCup        = "CUPGP";
      const std::string c_sGpWinRace       = "WINGP";
      const std::string c_sGpRace          = "RACEGP";

      const std::string c_sM2BlackDominate = "BL_DOM2";
      const std::string c_sM2BlackWinCup   = "BL_CUP2";
      const std::string c_sM2BlackWinRace  = "BL_WIN2";
      const std::string c_sM2Dominate      = "DOM2";
      const std::string c_sM2WinCup        = "CUP2";
      const std::string c_sM2WinRace       = "WIN2";
      const std::string c_sM2Race          = "RACE2";

      const std::string c_sM3BlackDominate = "BL_DOM3";
      const std::string c_sM3BlackWinCup   = "BL_CUP3";
      const std::string c_sM3BlackWinRace  = "BL_WIN3";
      const std::string c_sM3Dominate      = "DOM3";
      const std::string c_sM3WinCup        = "CUP3";
      const std::string c_sM3WinRace       = "WIN3";
      const std::string c_sM3Race          = "RACE3";

      const std::string s_sNetWinCup  = "NETCUPWIN";
      const std::string c_sNetWinRace = "NETRACEWIN";
      const std::string c_sNetRace    = "NETRACE";

      const std::string s_sSplitscreenWinCup  = "SPLITSCREENCUPWIN";
      const std::string c_sSplitscreenWinRace = "SPLITSCREENRACEWIN";
      const std::string c_sSplitscreenRace    = "SPLITSCREENRACE";
    }

    std::map<std::string, std::tuple<std::string, std::string, std::string>> g_mAllAchievements = {
      {
        achievements::c_sGOAT,
        std::make_tuple(
          "Greates of all Times"   , 
          "Awarded to players winning all races in the \"Dustbin::Games Cup\" racing in \"Black Lap\" (one lap per race) mode against MarbleGP marbles only.", 
          "bestofall.png"
        )
      },

      // Black lap achievemnts (MarbleGP)
      {
        achievements::c_sGpBlackDominate, 
        std::make_tuple(
          "MarbleGP Black Lap Cup Dominator",
          "Awarded to players winning all races in a cup racing against MarbleGP marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marblegp_cupallwin.png"
        )
      },
      {
        achievements::c_sGpBlackWinCup,
        std::make_tuple(
          "MarbleGP Black Lap Cup Winner", 
          "Awarded to players winning a cup against MarbleGP marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marblegp_cupwinner.png"
        )
      },
      {
        achievements::c_sGpBlackWinRace, 
        std::make_tuple(
          "MarbleGP Black Lap Race Winner", 
          "Awarded to players winning a race against MarbleGP marbles in the \"Black Lap\" (one lap per race) mode", 
          "black_marblegp_winner.png"
        )
      },

      // MarbleGP Achievements
      {
        achievements::c_sGpDominate,
        std::make_tuple(
          "MarbleGP Cup Dominator",
          "Awarded to players winning all races in a cup against MarbleGP marbles.",
          "marblegp_cupallwin.png"
        )
      },
      {
        achievements::c_sGpWinCup, 
        std::make_tuple(
        "MarbleGP Cup Winner", 
        "Awarded to players winning a cup against MarbleGP Marbles", 
        "marblegp_cupwinner.png"
        )
      },
      {
        achievements::c_sGpWinRace, 
        std::make_tuple(
        "MarbleGP Race Winner", 
        "Awarded to players winning a race against MarbleGP Marbles",
        "marblegp_winner.png"
        )
      },
      {
        achievements::c_sGpRace, 
        std::make_tuple(
          "MarbleGP Racer", 
          "Awarded to players racing against MarbleGP Marbles", 
          "marblegp_racer.png"
        )
      },

      // Black Lap Achievements (Marble2)
      {
        achievements::c_sM2BlackDominate, 
        std::make_tuple(
          "Marble2 Black Lap Cup Dominator", 
          "Awarded to players winning all races in a cup racing against Marble2 marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marble2_cupallwin.png"
        )
      },
      {
        achievements::c_sM2BlackWinCup, 
        std::make_tuple(
          "Marble2 Black Lap Cup Winner", 
          "Awarded to players winning a cup against Marble2 marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marble2_cupwinner.png"
        )
      },
      {
        achievements::c_sM2BlackWinRace, 
        std::make_tuple(
          "Marble2 Black Lap Race Winner", 
          "Awarded to players winning a race against Marble2 marbles in the \"Black Lap\" (one lap per race) mode", 
          "black_marble2_winner.png"
        )
      },

      // Marble2 Achievements
      {
        achievements::c_sM2Dominate, 
        std::make_tuple(
          "Marble2 Cup Dominator", 
          "Awarded to players winning all races in a cup against Marble2 marbles.", 
          "marble2_cupallwin.png"
        )
      },
      {
        achievements::c_sM2WinCup, 
        std::make_tuple(
          "Marble2 Cup Winner", 
          "Awarded to players winning a cup against Marble2 Marbles", 
          "marble2_cupwinner.png"
        )
      },
      {
        achievements::c_sM2WinRace, 
        std::make_tuple(
          "Marble2 Race Winner", 
          "Awarded to players winning a race against Marble2 Marbles",
          "marble2_winner.png"
        )
      },
      {
        achievements::c_sM2Race, 
        std::make_tuple(
          "Marble2 Racer", 
          "Awarded to players racing against Marble2 Marbles", 
          "marble2_racer.png"
        )
      },

      // Black Lap Achievements (Marble3)
      {
        achievements::c_sM3BlackDominate, 
        std::make_tuple(
          "Marble3 Black Lap Cup Dominator", 
          "Awarded to players winning all races in a cup racing against Marble3 marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marble3_cupallwin.png"
        )
      },
      {
        achievements::c_sM3BlackWinCup, 
        std::make_tuple(
          "Marble3 Black Lap Cup Winner", 
          "Awarded to players winning a cup against Marble3 marbles in the \"Black Lap\" (one lap per race) mode",
          "black_marble3_cupwinner.png"
        )
      },
      {
        achievements::c_sM3BlackWinRace, 
        std::make_tuple(
          "Marble3 Black Lap Race Winner", 
          "Awarded to players winning a race against Marble3 marbles in the \"Black Lap\" (one lap per race) mode", 
          "black_marble3_winner.png"
        )
      },

      // Marble3 Achievements
      {
        achievements::c_sM3Dominate,
        std::make_tuple(
          "Marble3 Cup Dominator",
          "Awarded to players winning all races in a cup against Marble3 marbles.",
          "marble3_cupallwin.png"
        )
      },
      {
        achievements::c_sM3WinCup, 
        std::make_tuple(
          "Marble3 Cup Winner", 
          "Awarded to players winning a cup against Marble3 Marbles", 
          "marble3_cupwinner.png"
        )
      },
      {
        achievements::c_sM3WinRace, 
        std::make_tuple(
          "Marble3 Race Winner", 
          "Awarded to players winning against Marble3 Marbles",
          "marble3_winner.png"
        )
      },
      {
        achievements::c_sM3Race, 
        std::make_tuple(
        "Marble3 Racer", 
        "Awarded to players racing against Marble3 Marbles", 
        "marble3_racer.png"
        )
      },

      // Network Achievements
      {
        achievements::s_sNetWinCup, 
        std::make_tuple(
          "Network Cup Winner", 
          "Awarded to players winning a MarbleGP Cup in a net game", 
          "network_cupwinner.png"
        )
      },
      {
        achievements::c_sNetWinRace, 
        std::make_tuple(
          "Network Race Winner", 
          "Awarded to players winning a MarbleGP Race in a net game", 
          "network_winner.png"
        )
      },
      {
        achievements::c_sNetRace, 
        std::make_tuple(
          "Network Racer", 
          "Awarded to players racing against other players in a net game", 
          "network_racer.png"
        )
      },

      // Splitscreen Achievements
      {
        achievements::s_sSplitscreenWinCup, 
        std::make_tuple(
          "Network Cup Winner", 
          "Awarded to players winning a MarbleGP Cup in a splitscreen game", 
          "splitscreen_cupwinner.png"
        )
      },
      {
        achievements::c_sSplitscreenWinRace, 
        std::make_tuple(
          "Network Race Winner", 
          "Awarded to players winning a MarbleGP Race in a splitscreen game", 
          "splitscreen_winner.png"
        )
      },
      {
        achievements::c_sSplitscreenRace, 
        std::make_tuple(
          "Splitscreen Racer", 
          "Awarded to players racing against other players in a splitscreen game", 
          "splitscreen_racer.png"
        )
      }
    };

    /**
    * Check whether or not a player has achieved a new achievement
    * @param a_pPlayer the player to check
    * @param a_pTournament the current tournament
    * @return a vector with the achievement identifiers
    */
    std::vector<std::string> checkForNewAchievements(data::SPlayerData* a_pPlayer, gameclasses::STournament* a_pTournament) {

      bool l_bFinished = true;
      
      for (auto l_pRace : a_pTournament->m_vRaces) {
        if (l_pRace->m_vRanking.size() == 0) {
          l_bFinished = false;
          break;
        }
      }

      bool l_bSplitscreen = false;
      bool l_bNetworkGame = false;
      bool l_bMarble3AI   = false;
      bool l_bMarble2AI   = false;
      bool l_bMarbleGPAI  = false;

      int l_iLocalPlayers = 0;
      int l_iPlayerCount  = (int)a_pTournament->m_vPlayers.size();

      std::vector<std::string> l_vAwarded;

      for (auto l_pPlayer : a_pTournament->m_vPlayers) {
        if (l_pPlayer->m_eType == data::enPlayerType::Local) {
          l_iLocalPlayers++;
          if (l_iLocalPlayers > 1)
            l_bSplitscreen = true;
        }

        if (l_pPlayer->m_eType == data::enPlayerType::Network) {
          l_bNetworkGame = true;
        }

        switch (getAiClass(l_pPlayer)) {
          case -1:
            // No Bot, nothing to do
            break;

          case 0:
            l_bMarbleGPAI = true;
            break;

          case 1:
            l_bMarble2AI = true;
            break;

          case 2:
            l_bMarble3AI = true;
            break;
        }
      }

      if (l_bSplitscreen) {
        // Check for splitscreen medals
        l_vAwarded.push_back(achievements::c_sSplitscreenRace);

        if (didWinLastRace(a_pPlayer, a_pTournament))
          l_vAwarded.push_back(achievements::c_sSplitscreenWinRace);
      }
      else if (l_bNetworkGame) {
        // Check for network medals
        l_vAwarded.push_back(achievements::c_sNetRace);

        if (didWinLastRace(a_pPlayer, a_pTournament))
          l_vAwarded.push_back(achievements::c_sNetWinRace);
      }
      else {
        if (l_bMarble3AI) {
          l_vAwarded.push_back(achievements::c_sM3Race);

          // Check for Marble3 medals
          if (didWinLastRace(a_pPlayer, a_pTournament))
            l_vAwarded.push_back(blackRace(a_pTournament) ? achievements::c_sM3BlackWinRace : achievements::c_sM3WinRace);

          bool l_bDominate = false;
          bool l_bBlackLap = false;

          if (didWinCup(a_pTournament, a_pPlayer, l_bDominate, l_bBlackLap)) {
            if (l_bBlackLap) {
              l_vAwarded.push_back(achievements::c_sM3BlackWinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sM3BlackDominate);
            }
            else {
              l_vAwarded.push_back(achievements::c_sM3WinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sM3WinCup);
            }
          }
        }

        if (l_bMarble2AI) {
          l_vAwarded.push_back(achievements::c_sM2Race);

          // Check for Marble2 medals
          if (didWinLastRace(a_pPlayer, a_pTournament))
            l_vAwarded.push_back(blackRace(a_pTournament) ? achievements::c_sM2BlackWinRace : achievements::c_sM2WinRace);

          bool l_bDominate = false;
          bool l_bBlackLap = false;

          if (didWinCup(a_pTournament, a_pPlayer, l_bDominate, l_bBlackLap)) {
            if (l_bBlackLap) {
              l_vAwarded.push_back(achievements::c_sM2BlackWinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sM2BlackDominate);
            }
            else {
              l_vAwarded.push_back(achievements::c_sM2WinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sM2WinCup);
            }
          }
        }

        if (l_bMarbleGPAI) {
          l_vAwarded.push_back(achievements::c_sGpRace);

          // Check for MarbleGP medals
          if (didWinLastRace(a_pPlayer, a_pTournament))
            l_vAwarded.push_back(blackRace(a_pTournament) ? achievements::c_sGpBlackWinRace : achievements::c_sGpWinRace);

          bool l_bDominate = false;
          bool l_bBlackLap = false;

          if (didWinCup(a_pTournament, a_pPlayer, l_bDominate, l_bBlackLap)) {
            if (l_bBlackLap) {
              l_vAwarded.push_back(achievements::c_sGpBlackWinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sGpBlackDominate);
            }
            else {
              l_vAwarded.push_back(achievements::c_sGpWinCup);
              if (l_bDominate)
                l_vAwarded.push_back(achievements::c_sGpWinCup);
            }
          }
        }
        
        if (a_pTournament->m_sCup == "Dustbin::Games Cup" && l_bFinished && l_bMarbleGPAI && !l_bMarble2AI && !l_bMarble3AI && l_iPlayerCount == 16) {
          // Check for GOAT medal
          bool l_bDominate = false;
          bool l_bBlackLap = false;

          if (didWinCup(a_pTournament, a_pPlayer, l_bDominate, l_bBlackLap)) {
            if (l_bDominate && l_bBlackLap)
              l_vAwarded.push_back(achievements::c_sGOAT);
          }
        }
      }

      for (auto l_tOld : a_pPlayer->m_vAchievements) {
        auto l_itAward = std::find(l_vAwarded.begin(), l_vAwarded.end(), std::get<0>(l_tOld));

        if (l_itAward != l_vAwarded.end()) {
          printf("Medal %s is not new, removing!\n", (*l_itAward).c_str());
          l_vAwarded.erase(l_itAward);
        }
      }

      for (auto l_sNew : l_vAwarded) {
        printf("%s\n", l_sNew.c_str());
      }

      return l_vAwarded;
    }

    /**
    * Get all available achievements
    * @return vector with the data (0 == id, 1 == name, 2 == description, 3 == image)
    */
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getAchievements() {
      std::vector<std::tuple<std::string, std::string, std::string, std::string>> l_vRet;

      for (auto l_cAchievement : g_mAllAchievements) {
        l_vRet.push_back(std::make_tuple(
          l_cAchievement.first,
          std::get<0>(l_cAchievement.second),
          std::get<1>(l_cAchievement.second),
          std::get<2>(l_cAchievement.second)
        ));
      }

      return l_vRet;
    }


    /**
    * Get an achievement by it's ID
    * @param a_sId the ID of the achievement
    * @return vector with the data (0 == id, 1 == name, 2 == description, 3 == image)
    */
    std::tuple<std::string, std::string, std::string, std::string> getAchievementById(const std::string& a_sId) {
      if (g_mAllAchievements.find(a_sId) != g_mAllAchievements.end()) {
        return std::make_tuple(
          a_sId,
          std::get<0>(g_mAllAchievements[a_sId]),
          std::get<1>(g_mAllAchievements[a_sId]),
          std::get<2>(g_mAllAchievements[a_sId]));
      }

      return std::make_tuple("", "", "", "");
    }
  }
}