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
    class CCmdEditTrack;    /**< Forward declaration of the edit track command */

    /**
    * @class CCmdLoadTrack
    * @author Christian Keimel
    * This command loads a track
    */
    class CCmdLoadTrack : public ICommand {
      public:
        /**
        * The constructor. All work is done here
        * @param a_pEditTrack the edit track command
        */
        CCmdLoadTrack(CCmdEditTrack *a_pEditTrack);
        virtual ~CCmdLoadTrack();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;
    };
  }
}
