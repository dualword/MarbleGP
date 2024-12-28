// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    class CCmdEditTrack;  /**< Forward declaration of the edit track command */

    /**
    * @class CCmdAddMarbleGP
    * @author Christian Keimel
    * Use this command to add the track to MarbleGP
    */
    class CCmdAddMarbleGP : public ICommand {
      private:
      irr::io::IFileSystem      *m_pFs;
      irr::video::IVideoDriver  *m_pDrv;
      irr::scene::ISceneManager *m_pOldSmgr;
      irr::scene::ISceneManager *m_pNewSmgr;
      irr::scene::ISceneNode    *m_pWorld;

      CCmdEditTrack *m_pEditTrack;

      std::string m_sFolder;

      /**
      * Export the track and add it to the new scene manager
      */
      bool exportAndPrepareTrack();

      /**
      * Add the checkpoints to the track
      */
      void addCheckpoints();

      /**
      * Add the AI, light and rostrum scene nodes
      */
      void addAiLightAndRostrum();

      /**
      * Create and save the thumbnail
      */
      void createThumbnail();

      /**
      * Write the info XML file
      */
      void createInfoFile();

      irr::scene::ISceneNode *findNode(irr::scene::ISceneNode *a_pParent, irr::scene::ESCENE_NODE_TYPE a_eType);

      public:
        /**
        * The constructor. All work is done here
        * @param m_pEditTrack the edit track command
        */
        CCmdAddMarbleGP(CCmdEditTrack *a_pEditTrack);
        virtual ~CCmdAddMarbleGP();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;
    };
  }
}
