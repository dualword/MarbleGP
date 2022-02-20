/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */

#include <scenenodes/CTriggerTimerNode.h>
#include <scenenodes/CDustbinId.h>
#include <string>

namespace dustbin {
  namespace scenenodes {


    CTriggerTimeNode::CTriggerTimeNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId) {
    }

    CTriggerTimeNode::~CTriggerTimeNode() {
    }

    void CTriggerTimeNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CTriggerTimeNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CTriggerTimeNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_TrigerTimerNodeId;
    }

    irr::scene::ISceneNode* CTriggerTimeNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      CTriggerTimeNode *l_pClone = new CTriggerTimeNode(a_pNewParent != nullptr ? a_pNewParent : Parent, a_pNewManager != nullptr ? a_pNewManager : getSceneManager(), getNextSceneNodeId());

      for (std::vector<STriggerAction>::const_iterator it = m_vActions.begin(); it != m_vActions.end(); it++) {
        STriggerAction l_cAction = STriggerAction(*it);
        l_pClone->m_vActions.push_back(l_cAction);
      }
      

      return l_pClone;
    }

    void CTriggerTimeNode::OnRegisterSceneNode() {
    }

    void CTriggerTimeNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      int i = 1;

      m_vActions.clear();

      while (a_pIn->existsAttribute((std::string("Action_") + std::to_string(i)).c_str())) {
        m_vActions.push_back(STriggerAction(a_pIn, i, enTriggerType::Timer));
        i++;
      }
    }

    void CTriggerTimeNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      int i = 1;

      for (std::vector<STriggerAction>::const_iterator it = m_vActions.begin(); it != m_vActions.end(); it++) {
        if ((*it).m_eAction != enAction::None || it == m_vActions.end() - 1) {
          (*it).serialize(a_pOut, i, enTriggerType::Timer);
          i++;
        }
      }

      if (m_vActions.size() == 0 || m_vActions.back().m_eAction != enAction::None)
        a_pOut->addEnum((std::string("Action_") + std::to_string(i)).c_str(), 0, g_ActionTypes);
    }
  }
}