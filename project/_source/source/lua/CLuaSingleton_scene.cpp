// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_scene.h>
#include <scenenodes/CWorldNode.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    /**
    * Change the rotation of a scene node
    * @param ID of the scene node
    * @param The new rotation
    */
    void CLuaSingleton_scene::setrotation(int a_id, const SVector3d& a_rotation) {
      irr::scene::ISceneNode *l_pNode = findnode(a_id);

      if (l_pNode != nullptr) {
        l_pNode->setRotation(irr::core::vector3df((irr::f32)a_rotation.m_x, (irr::f32)a_rotation.m_y, (irr::f32)a_rotation.m_z));
      }
    }

    /**
    * Get the rotation of a scene node (Euler degrees)
    * @param ID of the scene node
    */
    SVector3d CLuaSingleton_scene::getrotation(int a_id) {
      SVector3d l_cRet;
      irr::scene::ISceneNode *l_pNode = findnode(a_id);

      if (l_pNode != nullptr) {
        irr::core::vector3df l_cRot = l_pNode->getRotation();
        l_cRet.m_x = l_cRot.X;
        l_cRet.m_y = l_cRot.Y;
        l_cRet.m_z = l_cRot.Z;
      }

      return l_cRet;
    }

    /**
    * Change the position of a scene node
    * @param ID of the scene node
    * @param The new position
    */
    void CLuaSingleton_scene::setposition(int a_id, const SVector3d& a_position) {
      irr::scene::ISceneNode *l_pNode = findnode(a_id);

      if (l_pNode != nullptr) {
        l_pNode->setPosition(irr::core::vector3df((irr::f32)a_position.m_x, (irr::f32)a_position.m_y, (irr::f32)a_position.m_z));
      }
    }
    /**
    * Get the position of a scene node
    * @param ID of the scene node
    */
    SVector3d CLuaSingleton_scene::getposition(int a_id) {
      SVector3d l_cRet;
      irr::scene::ISceneNode *l_pNode = findnode(a_id);

      if (l_pNode != nullptr) {
        irr::core::vector3df l_cRot = l_pNode->getAbsolutePosition();
        l_cRet.m_x = l_cRot.X;
        l_cRet.m_y = l_cRot.Y;
        l_cRet.m_z = l_cRot.Z;
      }

      return l_cRet;
    }

    /**
    * Change the visibility of a scene node
    * @param ID of the scene node
    * @param The new visibility
    */
    void CLuaSingleton_scene::setvisible(int a_id, bool a_visibe) {
      irr::scene::ISceneNode *l_pNode = findnode(a_id);

      if (l_pNode != nullptr) {
        l_pNode->setVisible(a_visibe);
      }
    }

    /**
    * Get the global members from the CGlobal singleton
    */
    void CLuaSingleton_scene::initialize() {
      m_scenemanager = CGlobal::getInstance()->getSceneManager();

      fillNodeVector(m_scenemanager->getRootSceneNode());
    }

    /**
    * Used to fill the scene node vector
    * @param the current node
    */
    void CLuaSingleton_scene::fillNodeVector(irr::scene::ISceneNode* a_node) {
      std::string l_sName = a_node->getName();

      if (a_node->getType() == irr::scene::ESNT_MESH && l_sName.substr(0, std::string("Marble_").size()) != "Marble_") {
        m_scenenodes.push_back(a_node);
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itNode = a_node->getChildren().begin(); l_itNode != a_node->getChildren().end(); l_itNode++) {
        fillNodeVector(*l_itNode);
      }
    }

    /**
    * Searches a scene node in m_scenenodes by it's ID
    * @param ID of the scene node
    */
    irr::scene::ISceneNode* CLuaSingleton_scene::findnode(int a_ID) {
      for (std::vector<irr::scene::ISceneNode*>::iterator l_itNode = m_scenenodes.begin(); l_itNode != m_scenenodes.end(); l_itNode++) {
        if ((*l_itNode)->getID() == a_ID)
          return *l_itNode;
      }

      return nullptr;
    }
  }
}