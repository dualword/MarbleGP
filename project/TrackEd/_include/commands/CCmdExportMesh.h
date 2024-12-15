// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    class CCmdEditTrack;  /**< Forward declaration of the edit track command */

    /**
    * @class CCmdExportMesh
    * @author Christian Keimel
    * This command exports the track as a mesh
    */
    class CCmdExportMesh : public ICommand {
      public:
        /**
        * The constructor. All work is done here
        * @param a_sFileName the file name to save the track to
        * @param m_pEditTrack the edit track command
        */
        CCmdExportMesh(const std::string &a_sFileName, CCmdEditTrack *a_pEditTrack);
        virtual ~CCmdExportMesh();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;
    };
  }
}
