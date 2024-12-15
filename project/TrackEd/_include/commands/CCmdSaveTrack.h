// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
#pragma once

#include <irrlicht.h>
#include <vector>

#include <commands/ICommand.h>

namespace dustbin {
  namespace tracked {
    class ITrackPart;   /**< Forward declaratio of the ITrackPart interface */
  }

  namespace commands {
    /**
    * @class CCmdSaveTrack
    * @author Christian Keimel
    * This command saves a track
    */
    class CCmdSaveTrack : public ICommand {
      public:
        /**
        * The constructor. All work is done here
        * @param a_sFileName the file name to save the track to
        * @param a_vTrack the vector with the track parts to be saved
        */
        CCmdSaveTrack(const std::string &a_sFileName, const std::vector<tracked::ITrackPart *> &a_vTrack);
        virtual ~CCmdSaveTrack();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;
    };
  }
}
