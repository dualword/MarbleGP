// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCmdExportMesh.h>
#include <commands/CCmdEditTrack.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param a_sFileName the file name to save the track to
    * @param m_pEditTrack the edit track command
    */
    CCmdExportMesh::CCmdExportMesh(const std::string &a_sFileName, CCmdEditTrack *a_pEditTrack) : ICommand("CmdExportMesh") {
      irr::io::IWriteFile *l_pFile = tracked::IGlobal::getInstance()->getFileSystem()->createAndWriteFile(a_sFileName.c_str());

      if (l_pFile != nullptr) {
        irr::scene::IMeshWriter *l_pWriter = tracked::IGlobal::getInstance()->getSceneManager()->createMeshWriter(irr::scene::EMWT_OBJ);

        if (l_pWriter != nullptr) {
          l_pWriter->writeMesh(l_pFile, a_pEditTrack->getTrackMesh()->getMesh());
          l_pWriter->drop();
        }
        l_pFile->drop();
      }
    }

    CCmdExportMesh::~CCmdExportMesh() {
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdExportMesh::isFinished() {
      return true;
    }
  }
}