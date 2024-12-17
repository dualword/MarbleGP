// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#include <platform/CTrackEdPlatform.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdLoadTrack.h>
#include <commands/CCmdEditTrack.h>
#include <IGlobal.h>

#include <editor/CTrackPart_Checkpoint.h>
#include <editor/CTrackPart_Bezier.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_pEditTrack the edit track command
    */
    CCmdLoadTrack::CCmdLoadTrack(CCmdEditTrack *a_pEditTrack) :
      ICommand("CmdLoadTrack")
    {
      std::string l_sFileName = tracked::IGlobal::getInstance()->getGlobal("openfile");

      l_sFileName = platform::portableSelectFileOpen(L"Dustbin Games Track (*.dbt)\0*.dbt\0All Files (*.*)\0*.*", L".", helpers::s2ws(l_sFileName).c_str());

      tracked::IGlobal::getInstance()->setGlobal("openfile", l_sFileName);

      if (l_sFileName != "") {
        irr::io::IFileSystem *l_pFs     = tracked::IGlobal::getInstance()->getFileSystem();
        irr::io::IXMLReader *l_pReader = l_pFs->createXMLReader(l_sFileName.c_str());

        if (l_pReader) {
          a_pEditTrack->clearTrack();

          while (l_pReader->read()) {
            std::wstring l_sNode = l_pReader->getNodeName();

            if (l_sNode == L"TrackPart" && l_pReader->getNodeType() == irr::io::EXN_ELEMENT) {
              std::wstring l_sType = l_pReader->getAttributeValueSafe(L"type");

              if (l_sType == L"Bezier") {
                tracked::CTrackPart_Bezier *l_pNew = new tracked::CTrackPart_Bezier(nullptr, irr::core::vector3df());
                irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
                l_pAttr->read(l_pReader);
                l_pNew->deserialize(*l_pAttr);
                l_pAttr->drop();

                a_pEditTrack->addTrackPart(l_pNew);
              }
              else if (l_sType == L"Checkpoint") {
                tracked::CTrackPart_Checkpoint *l_pNew = new tracked::CTrackPart_Checkpoint(nullptr, irr::core::vector3df());
                irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
                l_pAttr->read(l_pReader);
                l_pNew->deserialize(*l_pAttr);
                l_pAttr->drop();

                a_pEditTrack->addTrackPart(l_pNew);
              }
              else printf("Unknown track part \"%ls\"\n", l_sType.c_str());
            }
          }

          a_pEditTrack->createTrack();
          a_pEditTrack->updateReferencePoints();
          l_pReader->drop();
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