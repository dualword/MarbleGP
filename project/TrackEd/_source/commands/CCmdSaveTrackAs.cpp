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
    * @param a_vTrack the vector with the track parts to be saved
    * @param a_pHandler the command handler
    */
    CCmdSaveTrackAs::CCmdSaveTrackAs(const std::vector<tracked::ITrackPart *> &a_vTrack, CCommandHandler *a_pHandler) : ICommand("CmdSaveTrackAs") {
      std::string l_sFileName = tracked::IGlobal::getInstance()->getGlobal("openfile");

      l_sFileName = platform::portableSelectFileSave(L"Dustbin Games Track (*.dbt)\0*.dbt\0All Files (*.*)\0*.*", L".", helpers::s2ws(l_sFileName).c_str());

      tracked::IGlobal::getInstance()->setGlobal("openfile", l_sFileName);

      CCmdSaveTrack *l_pCmd = new CCmdSaveTrack(a_vTrack);
      a_pHandler->addCommand(l_pCmd);
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
