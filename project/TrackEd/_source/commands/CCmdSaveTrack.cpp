// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#include <helpers/CTrackEdHelpers.h>
#include <commands/CCmdSaveTrack.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_pEditCmd the "edit track" command
    */
    CCmdSaveTrack::CCmdSaveTrack(CCmdEditTrack *a_pEditCmd) :
      ICommand("CmdSaveTrack")
    {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();

      std::string l_sFileName = tracked::IGlobal::getInstance()->getGlobal("openfile");

      if (l_sFileName != "") {
        irr::io::IXMLWriter *l_pWriter = l_pGlobal->getFileSystem()->createXMLWriter(l_sFileName.c_str());

        if (l_pWriter != nullptr) {
          l_pWriter->writeXMLHeader();
          l_pWriter->writeElement(L"DustbinGamesTrack", false);
          l_pWriter->writeLineBreak();

          l_pWriter->writeElement(L"TrackInformation", false);
          l_pWriter->writeLineBreak();

          irr::io::IAttributes *l_pAttr = l_pGlobal->getFileSystem()->createEmptyAttributes();
          a_pEditCmd->saveTrackInformation(*l_pAttr);
          l_pAttr->write(l_pWriter, false);
          l_pAttr->drop();

          l_pWriter->writeClosingTag(L"TrackInformation");
          l_pWriter->writeLineBreak();

          for (auto l_pPart : a_pEditCmd->getTrack()) {
            l_pWriter->writeElement(L"TrackPart", false, L"type", helpers::s2ws(l_pPart->getType()).c_str());
            l_pWriter->writeLineBreak();

            l_pAttr = l_pGlobal->getFileSystem()->createEmptyAttributes();
            l_pPart->serialize(*l_pAttr);
            l_pAttr->write(l_pWriter, false);
            l_pAttr->drop();

            l_pWriter->writeClosingTag(L"TrackPart");
            l_pWriter->writeLineBreak();
          }

          l_pWriter->writeClosingTag(L"DustbinGamesTrack");
          l_pWriter->writeLineBreak();

          l_pWriter->drop();
          helpers::addLogMessage(L"Track file \"" + helpers::extractFileName(l_sFileName) + L"\" Saved.", irr::ELL_INFORMATION, 5000);
        }
        else helpers::addLogMessage(L"Error while saving track file.", irr::ELL_ERROR, 5000);
        
      }
    }

    CCmdSaveTrack::~CCmdSaveTrack() {

    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdSaveTrack::isFinished() {
      return true;
    }

  }
}