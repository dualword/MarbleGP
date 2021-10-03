// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinId.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CCheckpointNode::CCheckpointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CPhysicsNode(a_pParent, a_pMgr, a_iId),
      m_bFirstInLap(false)
    {
      m_cBox.reset(getPosition());

      setScale(irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      sceneNodeIdUsed(a_iId);
    }

    CCheckpointNode::~CCheckpointNode() {
    }

    void CCheckpointNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CCheckpointNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CCheckpointNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
    }

    void CCheckpointNode::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CCheckpointNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("FirstInLap", m_bFirstInLap);

      // If a checkpoint is marked as "First in Lap"
      // counting starts at "0"
      int l_iPrev = m_bFirstInLap ? 0 : 1;

      for (std::map<irr::s32, std::vector<irr::s32> >::const_iterator it = m_mLinks.begin(); it != m_mLinks.end(); it++) {
        a_pOut->addInt((std::string("Previous_") + std::to_string(l_iPrev)).c_str(), it->first);

        int i = 1;
        for (std::vector<irr::s32>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
          std::string l_sName = "Next_" + std::to_string(it->first) + "_" + std::to_string(i);

          a_pOut->addInt(l_sName.c_str(), *it2);

          i = i + 1;
        }

        a_pOut->addInt((std::string("Next_") + std::to_string(it->first) + "_" + std::to_string(it->second.size() + 1)).c_str(), -1);

        std::string l_sRespawn = "Respawn_" + std::to_string(it->first);

        if (m_mRespawn.find(it->first) != m_mRespawn.end()) {
          irr::s32 l_iKey   = it->first,
                   l_iValue = m_mRespawn.find(it->first)->second;

          a_pOut->addInt(l_sRespawn.c_str(), l_iValue);
        }
        else
          a_pOut->addInt(l_sRespawn.c_str(), -1);

        l_iPrev++;
      }

      if (m_bFirstInLap && m_mLinks.find(0) == m_mLinks.end()) {
        a_pOut->addInt("Previous_0", 0);
        a_pOut->addInt("Next_0_1", -1);
      }

      if (m_bFirstInLap) {
        int i = 1;

        for (std::vector<int>::const_iterator it = m_vFinishLapIDs.begin(); it != m_vFinishLapIDs.end(); it++) {
          a_pOut->addInt((std::string("FinishLap_") + std::to_string(i)).c_str(), *it);
          i++;
        }

        a_pOut->addInt((std::string("FinishLap_") + std::to_string(i)).c_str(), -1);
      }
    }

    void CCheckpointNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      setScale(irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      if (a_pIn->existsAttribute("FirstInLap"))
        m_bFirstInLap = a_pIn->getAttributeAsBool("FirstInLap");

      m_mLinks.clear();

      // If a checkpoint is marked as "First in Lap"
      // counting starts at "0"
      int l_iPrev = m_bFirstInLap ? 0 : 1;
      std::string l_sName = "Previous_" + std::to_string(l_iPrev);

      m_mRespawn.clear();

      while (a_pIn->existsAttribute(l_sName.c_str())) {
        int l_iId = a_pIn->getAttributeAsInt(l_sName.c_str());
        m_mLinks[l_iId] = std::vector<int>();

        int l_iNext = 1;
        std::string l_sNext = "Next_" + std::to_string(l_iId) + "_" + std::to_string(l_iNext);

        while (a_pIn->existsAttribute(l_sNext.c_str())) {
          int l_iNextId = a_pIn->getAttributeAsInt(l_sNext.c_str());

          if (l_iNextId != -1)
            m_mLinks[l_iId].push_back(l_iNextId);

          l_iNext++;
          l_sNext = "Next_" + std::to_string(l_iId) + "_" + std::to_string(l_iNext);
        }

        l_iPrev++;
        l_sName = "Previous_" + std::to_string(l_iPrev);

        std::string l_sRespawn = "Respawn_" + std::to_string(l_iId);

        if (a_pIn->existsAttribute(l_sRespawn.c_str())) {
          irr::s32 l_iRespawn = a_pIn->getAttributeAsInt(l_sRespawn.c_str());
          if (l_iRespawn != -1)
            m_mRespawn[l_iId] = l_iRespawn;
        }
      }

      if (m_bFirstInLap) {
        m_vFinishLapIDs.clear();

        int i = 1;

        std::string l_sName = (std::string("FinishLap_") + std::to_string(i));

        while (a_pIn->existsAttribute(l_sName.c_str())) {
          int l_iId = a_pIn->getAttributeAsInt(l_sName.c_str());
          if (l_iId != -1)
            m_vFinishLapIDs.push_back(l_iId);

          i++;
          l_sName = (std::string("FinishLap_") + std::to_string(i));
        };
      }

      sceneNodeIdUsed(getID());
    }

    irr::scene::ISceneNode* CCheckpointNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent == nullptr) a_pNewParent = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CCheckpointNode* l_pNew = new CCheckpointNode(a_pNewParent, a_pNewManager, getNextSceneNodeId());

      l_pNew->m_bFirstInLap = m_bFirstInLap;

      l_pNew->drop();
      return l_pNew;
    }

    /**
    * Register a previous node
    * @param a_iId ID of the previous node
    */
    void CCheckpointNode::addPreviousNode(int a_iId) {
      if (m_mLinks.find(a_iId) == m_mLinks.end())
        m_mLinks[a_iId] = std::vector<int>();
    }

    bool CCheckpointNode::isFirstInLap() {
      return m_bFirstInLap;
    }
  }
}