// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#include <commands/CCmdSaveTrack.h>
#include <helpers/CStringHelpers.h>
#include <editor/ITrackPart.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_sFileName the file name to save the track to
    * @param a_vTrack the vector with the track parts to be saved
    */
    CCmdSaveTrack::CCmdSaveTrack(const std::string& a_sFileName, const std::vector<tracked::ITrackPart *>& a_vTrack) :
      ICommand("CmdSaveTrack")
    {
      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();

      irr::io::IXMLWriter *l_pWriter = l_pGlobal->getFileSystem()->createXMLWriter(a_sFileName.c_str());

      if (l_pWriter != nullptr) {
        l_pWriter->writeXMLHeader();
        l_pWriter->writeElement(L"DustbinGamesTrack", false);

        for (auto l_pPart : a_vTrack) {
          l_pWriter->writeElement(L"TrackPart", false, L"type", helpers::s2ws(l_pPart->getType()).c_str());

          irr::io::IAttributes *l_pAttr = l_pGlobal->getFileSystem()->createEmptyAttributes();
          l_pPart->serialize(*l_pAttr);
          l_pAttr->write(l_pWriter, false);
          l_pAttr->drop();

          l_pWriter->writeClosingTag(L"TrackPart");
        }

        l_pWriter->writeClosingTag(L"DustbinGamesTrack");

        l_pWriter->drop();
      }
    }

    CCmdSaveTrack::~CCmdSaveTrack() {

    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * return true if the command is finished and can be removed
    */
    bool CCmdSaveTrack::isFinished() {
      return true;
    }

  }
}