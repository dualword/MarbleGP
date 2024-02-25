// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    /**
     * @class CSceneNodeBase
     * @author Christian Keimel
     * @brief This is a simple base class for the scenenodes
     */
    class CSceneNodeBase {
      private:
        static int m_iNextSceneNodeId;

      public:
        /**
         * Get the next ID for a MarbleGP scene node
         * @return the next ID for a MarbleGP scene node
         */
        static int getNextSceneNodeId();

        /**
         * Add an ID when loading a scene to make sure we have unique IDs within the scene
         * @param a_iId the ID of the loaded node
         */
        static void addId(int a_iId); 
    };
  }
}