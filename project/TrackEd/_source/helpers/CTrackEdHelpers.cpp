// (w) 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdMainMenu.h>
#include <IGlobal.h>

namespace dustbin {
  namespace helpers {
    /**
    * Add a message to the log
    * @param a_sMessage the message to add
    * @param a_eLevel the level of the log message
    * @param a_iTimeToLive the time to live in milliseconds
    */
    void addLogMessage(const std::wstring &a_sMessage, irr::ELOG_LEVEL a_eLevel, irr::u32 a_iTimeToLive) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        commands::CCmdEditTrack *l_pEdit = l_pGlobal->getEditTrackCommand();
        if (l_pEdit != nullptr) {
          commands::CCmdMainMenu *l_pMain = l_pEdit->getMainMenu();
          if (l_pMain != nullptr) {
            l_pMain->addLogMessage(a_sMessage, a_eLevel, a_iTimeToLive);
          }
        }
      }
    }

    /**
    * Set the "Edit Mode" message
    * @param a_sMessage the new message
    */
    void setEditModeMessage(const std::wstring &a_sMessage) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        commands::CCmdEditTrack *l_pEdit = l_pGlobal->getEditTrackCommand();
        if (l_pEdit != nullptr) {
          commands::CCmdMainMenu *l_pMain = l_pEdit->getMainMenu();
          if (l_pMain != nullptr) {
            l_pMain->setEditMode(a_sMessage);
          }
        }
      }
    }

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::wstring &a_sFile) {
      wchar_t l_sDelimiter[] = { L'/', L'\\', L'\0' };

      for (int i = 0; l_sDelimiter[i] != L'\0'; i++) {
        if (a_sFile.find_last_of(l_sDelimiter[i]) != std::string::npos) {
          return a_sFile.substr(a_sFile.find_last_of(l_sDelimiter[i]) + 1);
        }
      }

      return a_sFile;
    }

    /**
    * Extract the filename of a given path
    * @param a_sFile the filename with path
    * @return the filename without path
    */
    std::wstring extractFileName(const std::string &a_sFile) {
      char l_sDelimiter[] = { '/', '\\', '\0' };

      for (int i = 0; l_sDelimiter[i] != L'\0'; i++) {
        if (a_sFile.find_last_of(l_sDelimiter[i]) != std::string::npos) {
          return helpers::s2ws(a_sFile.substr(a_sFile.find_last_of(l_sDelimiter[i]) + 1));
        }
      }

      return helpers::s2ws(a_sFile);
    }

    /**
    * Post a user event
    * @param a_iUserData the user data
    */
    void postUserEvent(size_t a_iUserData1, size_t a_iUserData2) {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();
      if (l_pGlobal != nullptr) {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_USER_EVENT;
        l_cEvent.UserEvent.UserData1 = a_iUserData1;
        l_cEvent.UserEvent.UserData2 = a_iUserData2;
        l_pGlobal->getDevice()->postEventFromUser(l_cEvent);
      }
    }
  }
}

