// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaSceneNode.h>
#include <LuaBridge/LuaBridge.h>

namespace dustbin {
  namespace lua {
    CLuaSceneNode::CLuaSceneNode(irr::scene::ISceneNode* a_pNode) : m_pNode(a_pNode) {
    }

    CLuaSceneNode::CLuaSceneNode(const CLuaSceneNode& a_cOther) : m_pNode(a_cOther.m_pNode) {
    }

    CLuaSceneNode::~CLuaSceneNode() {
    }

    irr::scene::ISceneNode* CLuaSceneNode::getNode() {
      return m_pNode;
    }

    void CLuaSceneNode::setPosition(float a_fX, float a_fY, float a_fZ) {
      if (m_pNode != nullptr)
        m_pNode->setPosition(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }

    void CLuaSceneNode::setRotation(float a_fX, float a_fY, float a_fZ) {
      if (m_pNode != nullptr)
        m_pNode->setRotation(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }

    void CLuaSceneNode::setScale(float a_fX, float a_fY, float a_fZ) {
      if (m_pNode != nullptr)
        m_pNode->setScale(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }


    void CLuaSceneNode::setVisible(bool a_bVisible) {
      if (m_pNode != nullptr)
        m_pNode->setVisible(a_bVisible);
    }


    void CLuaSceneNode::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSceneNode>("scenenode")
          .addFunction("setposition", &CLuaSceneNode::setPosition)
          .addFunction("setrotation", &CLuaSceneNode::setRotation)
          .addFunction("setscale"   , &CLuaSceneNode::setScale)
          .addFunction("setvisible" , &CLuaSceneNode::setVisible)
        .endClass();
    }
  }
}