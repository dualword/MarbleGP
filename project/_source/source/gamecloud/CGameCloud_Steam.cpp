// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#ifdef _STEAM

#include <helpers/CMedalHelpers.h>
#include <gamecloud/CGameCloud.h>
#include <steam_api.h>

#include <steam_api_common.h>

namespace dustbin {
  namespace gamecloud {
    #define _ACH_ID( id, name ) { id, #id, name, "", 0, 0 }

    struct SSteamAchievement {
      int   m_eAchievementID;
      char *m_pchAchievementID;
      char  m_rgchName[128];
      char  m_rgchDescription[256];
      bool  m_bAchieved;
      int   m_iIconImage;

      SSteamAchievement() {
        m_eAchievementID = 0;
        m_pchAchievementID = nullptr;
        
        memset(m_rgchName       , 0, 128);
        memset(m_rgchDescription, 0, 256);

        m_bAchieved  = false;
        m_iIconImage = 0;
      }
    };

    class CSteamAchievements {
      private:
        int64 m_iAppID; // Unsere App-ID
        SSteamAchievement *m_pAchievements; // Errungenschaftsdaten
        int m_iNumAchievements; // Anzahl der Errungenschaften
        bool m_bInitialized; // Haben wir bereits die Statistiken abgerufen und den zugehörigen Rückruf erhalten?

      public:
        CSteamAchievements(SSteamAchievement *Achievements, int NumAchievements);
        ~CSteamAchievements();

        bool RequestStats();

        bool SetAchievement(const char* ID);

        STEAM_CALLBACK(CSteamAchievements, OnUserStatsReceived, UserStatsReceived_t    , m_CallbackUserStatsReceived);
        STEAM_CALLBACK(CSteamAchievements, OnUserStatsStored  , UserStatsStored_t      , m_CallbackUserStatsStored);
        STEAM_CALLBACK(CSteamAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored);
    };

    CSteamAchievements::~CSteamAchievements() {
      if (m_pAchievements != nullptr)
        delete []m_pAchievements;
    }

    bool CSteamAchievements::RequestStats(){
      // Ist Steam geladen? Falls nicht, können wir keine Statistiken abrufen.
      if (NULL == SteamUserStats() || NULL == SteamUser())
      {
        return false;
      }
      // Ist der Nutzer angemeldet?  Falls nicht, können wir keine Statistiken abrufen.
      if (!SteamUser()->BLoggedOn())
      {
        return false;
      }
      // Abfrage der Nutzerstatistiken
      return SteamUserStats()->RequestCurrentStats();
    }

    bool CSteamAchievements::SetAchievement(const char* ID) {
      // Haben wir schon einen Rückruf von Steam erhalten?
      if (m_bInitialized)
      {
        bool l_bAward = true;

        for (int i = 0; i < m_iNumAchievements; i++) {
          if (!strcmp(m_pAchievements[i].m_pchAchievementID, ID) && m_pAchievements[i].m_bAchieved) {
            l_bAward = false;
            break;
          }
        }

        if (l_bAward) {
          SteamUserStats()->SetAchievement(ID);
          return SteamUserStats()->StoreStats();
        }
        else return false;
      }
      // Falls nicht, können wir noch keine Errungenschaften freischalten.
      return false;
    }

    void CSteamAchievements::OnUserStatsReceived( UserStatsReceived_t *pCallback) {
      // Eventuell erhalten wir Rückrufe für Statistiken von anderen Spielen; wir möchten diese ignorieren.
      if ( m_iAppID == pCallback->m_nGameID )
      {
        if ( k_EResultOK == pCallback->m_eResult )
        {
          printf("Received stats and achievements from Steam\n");
          m_bInitialized = true;

          // Errungenschaften laden
          for ( int iAch = 0; iAch < m_iNumAchievements; ++iAch )
          {
            SSteamAchievement &ach = m_pAchievements[iAch];

            SteamUserStats()->GetAchievement(ach.m_pchAchievementID, &ach.m_bAchieved);
            _snprintf( ach.m_rgchName, sizeof(ach.m_rgchName), "%s",
              SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
                "name"));
            _snprintf( ach.m_rgchDescription, sizeof(ach.m_rgchDescription), "%s",
              SteamUserStats()->GetAchievementDisplayAttribute(ach.m_pchAchievementID,
                "desc"));
          }
        }
        else
        {
          char buffer[128];
          _snprintf( buffer, 128, "RequestStats - failed, %d\n", pCallback->m_eResult );
          printf( "%s", buffer);
        }
      }
    }

    void CSteamAchievements::OnUserStatsStored(UserStatsStored_t *pCallback) {
      // Eventuell erhalten wir Rückrufe für Statistiken von anderen Spielen; wir möchten diese ignorieren.
      if ( m_iAppID == pCallback->m_nGameID )
      {
        if ( k_EResultOK == pCallback->m_eResult )
        {
          printf("Stored stats for Steam\n");
        }
        else
        {
          char buffer[128];
          _snprintf( buffer, 128, "StatsStored - failed, %d\n", pCallback->m_eResult );
          printf("%s", buffer);
        }
      }
    }

    void CSteamAchievements::OnAchievementStored(UserAchievementStored_t *pCallback) {
      // Eventuell erhalten wir Rückrufe für Statistiken von anderen Spielen; wir möchten diese ignorieren.
      if ( m_iAppID == pCallback->m_nGameID )
      {
        printf("Stored Achievement for Steam\n");
      }
    }

    CSteamAchievements::CSteamAchievements(SSteamAchievement *Achievements, int NumAchievements) :
      m_iAppID(0),
      m_bInitialized(false),
      m_CallbackUserStatsReceived(this, &CSteamAchievements::OnUserStatsReceived),
      m_CallbackUserStatsStored  (this, &CSteamAchievements::OnUserStatsStored),
      m_CallbackAchievementStored(this, &CSteamAchievements::OnAchievementStored)
    {
      m_iAppID           = SteamUtils()->GetAppID();
      m_pAchievements    = Achievements;
      m_iNumAchievements = NumAchievements;
      RequestStats();
    }


    bool g_bSteamWasInitialized = false;

    CSteamAchievements *g_pSteamAchievements = nullptr;

    /**
    * Initialize whatever library / cloud service is used
    * @return true if initialization was successful, false otherwise
    */
    bool initialize() {
      if (SteamAPI_Init()) {
        g_bSteamWasInitialized = true;

        std::vector<std::tuple<std::string, std::string, std::string, std::string>> l_vAchievements = helpers::getAchievements();

        SSteamAchievement *l_pSteamAchs = new SSteamAchievement[l_vAchievements.size()];

        int l_iCount = 0;
        for (auto l_cAch : l_vAchievements) {
          l_pSteamAchs[l_iCount].m_eAchievementID   = l_iCount;
          l_pSteamAchs[l_iCount].m_pchAchievementID = new char[std::get<0>(l_cAch).size() + 1];

          memset(l_pSteamAchs[l_iCount].m_pchAchievementID, 0, std::get<0>(l_cAch).size() + 1);
          memcpy(l_pSteamAchs[l_iCount].m_pchAchievementID, std::get<0>(l_cAch).c_str(), std::get<0>(l_cAch).size());

          l_iCount++;
        }
          

        g_pSteamAchievements = new CSteamAchievements(l_pSteamAchs, (int)l_vAchievements.size());
        return true;
      }
      else return false;
    }

    /**
    * Shut down whatever library / cloud service is used
    */
    void shutdown() {
      if (g_bSteamWasInitialized)
        SteamAPI_Shutdown();

      if (g_pSteamAchievements)
        delete g_pSteamAchievements;
    }


    /**
    * Poll the cloud service
    */
    void pollService() {
      if (g_pSteamAchievements != nullptr)
        SteamAPI_RunCallbacks();
    }

    /**
    * Add an achievement to the cloud
    * @param a_sId ID of the achievement
    */
    void awardAchievement(const std::string& a_sId) {
      if (g_pSteamAchievements != nullptr)
        g_pSteamAchievements->SetAchievement(a_sId.c_str());
    }
  }
}

#endif
