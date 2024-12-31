// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#include <platform/CTrackEdPlatform.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdLoadTrack.h>
#include <commands/CCmdEditTrack.h>
#include <CDefines.h>
#include <IGlobal.h>

#include <editor/CTrackPart_FlatSegment.h>
#include <editor/CTrackPart_Checkpoint.h>
#include <editor/CTrackPart_Straight.h>
#include <editor/CTrackPart_Jump.h>
#include <editor/CTrackPart_Turn.h>
#include <editor/CTrackPart_Ramp.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_pEditTrack the edit track command
    */
    CCmdLoadTrack::CCmdLoadTrack(CCmdEditTrack *a_pEditTrack) :
      ICommand("CmdLoadTrack")
    {
      irr::io::path l_sWorkingDir = tracked::IGlobal::getInstance()->getFileSystem()->getWorkingDirectory();

      std::string l_sFileName = tracked::IGlobal::getInstance()->getGlobal ("openfile");
      std::string l_sFolder   = tracked::IGlobal::getInstance()->getSetting("openfolder");

      l_sFileName = platform::portableSelectFileOpen(L"Dustbin Games Track (*.dbt)\0*.dbt\0All Files (*.*)\0*.*", l_sFolder != "" ? helpers::s2ws(l_sFolder).c_str() : L".", helpers::s2ws(l_sFileName).c_str());

      tracked::IGlobal::getInstance()->setGlobal("openfile", l_sFileName);

      tracked::IGlobal::getInstance()->getFileSystem()->changeWorkingDirectoryTo(l_sWorkingDir);

      if (l_sFileName != "") {
        tracked::IGlobal::getInstance()->clearScene();

        if (l_sFileName.find_last_of('\\') != std::string::npos) {
          std::string l_sFolder = l_sFileName.substr(0, l_sFileName.find_last_of('\\'));
          tracked::IGlobal::getInstance()->setSetting("openfolder", l_sFolder);
        }

        irr::io::IFileSystem *l_pFs     = tracked::IGlobal::getInstance()->getFileSystem();
        irr::io::IXMLReader  *l_pReader = l_pFs->createXMLReader(l_sFileName.c_str());

        if (l_pReader) {
          while (l_pReader->read()) {
            std::wstring l_sNode = l_pReader->getNodeName();

            if (l_sNode == L"TrackInformation" && l_pReader->getNodeType() == irr::io::EXN_ELEMENT) {
              irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
              l_pAttr->read(l_pReader);
              a_pEditTrack->loadTrackInformation(*l_pAttr);
              l_pAttr->drop();
            }
            else if (l_sNode == L"TrackPart" && l_pReader->getNodeType() == irr::io::EXN_ELEMENT) {
              std::wstring l_sType = l_pReader->getAttributeValueSafe(L"type");

              tracked::ITrackPart *l_pNew = nullptr;

              if (l_sType == helpers::s2ws(c_sTypeFlat)) {
                l_pNew = new tracked::CTrackPart_FlatSegment(nullptr, irr::core::vector3df());
              }
              else if (l_sType == helpers::s2ws(c_sTypeRamp)) {
                l_pNew = new tracked::CTrackPart_Ramp(nullptr, irr::core::vector3df());
              }
              else if (l_sType == helpers::s2ws(c_sTypeCheckpoint)) {
                l_pNew = new tracked::CTrackPart_Checkpoint(nullptr, irr::core::vector3df());
              }
              else if (l_sType == helpers::s2ws(c_sTypeTurn)) {
                l_pNew = new tracked::CTrackPart_Turn(tracked::CTrackPart_Turn::enTurnAngle::FourtyFive, true, nullptr, irr::core::vector3df());
              }
              else if (l_sType == helpers::s2ws(c_sTypeStraight)) {
                l_pNew = new tracked::CTrackPart_Straight(nullptr, irr::core::vector3df());
              }
              else if (l_sType == helpers::s2ws(c_sTypeJump)) {
                l_pNew = new tracked::CTrackPart_Jump(nullptr, irr::core::vector3df());
              }
              else {
                helpers::addLogMessage(L"Unknown track part \"" + l_sType + L"\".", irr::ELL_ERROR, 5000);
              }

              if (l_pNew != nullptr) {
                irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
                l_pAttr->read(l_pReader);
                l_pNew->deserialize(*l_pAttr);
                l_pAttr->drop();

                a_pEditTrack->addTrackPart(l_pNew);
              }
            }
          }

          a_pEditTrack->createTrack();
          a_pEditTrack->updateReferencePoints();
          l_pReader->drop();

          helpers::postUserEvent(c_iSceneLoaded, c_iSceneLoaded);

          helpers::addLogMessage(L"Track file \"" + helpers::extractFileName(l_sFileName) + L"\" loaded.", irr::ELL_INFORMATION, 8000);
        }
      }
    }

    CCmdLoadTrack::~CCmdLoadTrack() {
      printf("Delete load track command.\n");
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdLoadTrack::isFinished() {
      return true;
    }
  }
}