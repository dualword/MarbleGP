// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#include <platform/CTrackEdPlatform.h>
#include <commands/CCommandHandler.h>
#include <commands/CCmdSaveTrackAs.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdSaveTrack.h>
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_pEditCmd the "edit track" command
    * @param a_pHandler the command handler
    */
    CCmdSaveTrackAs::CCmdSaveTrackAs(CCmdEditTrack *a_pEditCmd, CCommandHandler *a_pHandler) : ICommand("CmdSaveTrackAs") {
      irr::io::path l_sWorkingDir = tracked::IGlobal::getInstance()->getFileSystem()->getWorkingDirectory();

      std::string l_sFileName = tracked::IGlobal::getInstance()->getGlobal ("openfile");
      std::string l_sFolder   = tracked::IGlobal::getInstance()->getSetting("openfolder");

      l_sFileName = platform::portableSelectFileSave(L"Dustbin Games Track (*.dbt)\0*.dbt\0All Files (*.*)\0*.*", l_sFolder != "" ? helpers::s2ws(l_sFolder).c_str() : L".", helpers::s2ws(l_sFileName).c_str());

      tracked::IGlobal::getInstance()->getFileSystem()->changeWorkingDirectoryTo(l_sWorkingDir);

      if (l_sFileName != "") {
        if (l_sFileName.find_last_of('.') != std::string::npos) {
          l_sFileName = l_sFileName.substr(0, l_sFileName.find_last_of('.'));
        }

        l_sFileName += ".dbt";

        if (l_sFileName.find_last_of('\\') != std::string::npos) {
          std::string l_sFolder = l_sFileName.substr(0, l_sFileName.find_last_of('\\'));
          tracked::IGlobal::getInstance()->setSetting("openfolder", l_sFolder);
        }

        tracked::IGlobal::getInstance()->setGlobal("openfile", l_sFileName);

        CCmdSaveTrack *l_pCmd = new CCmdSaveTrack(a_pEditCmd);
        a_pHandler->addCommand(l_pCmd);
      }
    }

    CCmdSaveTrackAs::~CCmdSaveTrackAs() {
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdSaveTrackAs::isFinished() {
      return true;
    }
  }
}
