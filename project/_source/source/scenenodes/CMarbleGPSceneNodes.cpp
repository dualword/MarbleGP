// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodes.h>

namespace dustbin {
  namespace scenenodes {
    /**
    * Get the next ID for a MarbleGP scene node
    * @return the next ID for a MarbleGP scene node
    */
    int CSceneNodeBase::getNextSceneNodeId() {
      return m_iNextSceneNodeId++;
    }

    /**
    * Add an ID when loading a scene to make sure we have unique IDs within the scene
    * @param a_iId the ID of the loaded node
    */
    void CSceneNodeBase::addId(int a_iId) {
      if (a_iId >= m_iNextSceneNodeId) m_iNextSceneNodeId = a_iId + 1;
    }

    int CSceneNodeBase::m_iNextSceneNodeId = 23000;
  }
}