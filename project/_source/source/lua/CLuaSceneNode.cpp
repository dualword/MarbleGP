// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaSceneNode.h>
#include <LuaBridge/LuaBridge.h>
#include <_generated/lua/lua_tables.h>
#include <lua/CLuaHelpers.h>

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

    int CLuaSceneNode::setPosition(lua_State *a_pState) {
      if (m_pNode != nullptr) {
        int l_iArgC = lua_gettop(a_pState);
        if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setposition\". 1 argument required."); return 0; }

        SVector3d l_vPos;
        l_vPos.loadFromStack(a_pState); lua_pop(a_pState, 1);

        m_pNode->setPosition(convertVectorToIrr(l_vPos));
      }
      return 0;
    }

    int CLuaSceneNode::setRotation(lua_State *a_pState) {
      if (m_pNode != nullptr) {
        int l_iArgC = lua_gettop(a_pState);
        if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setrotation\". 1 argument required."); return 0; }

        SVector3d l_vRotation;
        l_vRotation.loadFromStack(a_pState); lua_pop(a_pState, 1);

        m_pNode->setRotation(convertVectorToIrr(l_vRotation));
      }
      return 0;
    }

    int CLuaSceneNode::setScale(lua_State *a_pState) {
      if (m_pNode != nullptr) {
        int l_iArgC = lua_gettop(a_pState);
        if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setscale\". 1 argument required."); return 0; }

        SVector3d l_vScale;
        l_vScale.loadFromStack(a_pState); lua_pop(a_pState, 1);

        m_pNode->setScale(convertVectorToIrr(l_vScale));
      }
      return 0;
    }

    int CLuaSceneNode::getPosition(lua_State* a_pState) {
      if (m_pNode != nullptr) {
        SVector3d l_cPos = convertVectorToLua(m_pNode->getPosition());
        l_cPos.pushToStack(a_pState);
        return 1;
      }
      else return 0;
    }

    void CLuaSceneNode::setVisible(bool a_bVisible) {
      if (m_pNode != nullptr)
        m_pNode->setVisible(a_bVisible);
    }

    bool CLuaSceneNode::isVisible() {
      return m_pNode != nullptr && m_pNode->isVisible();
    }

    void CLuaSceneNode::setTexture(int a_iMaterial, const std::string& a_sTexture) {
      if (m_pNode != nullptr) {
        if (a_iMaterial >= 0 && a_iMaterial < (int)m_pNode->getMaterialCount()) {
          irr::video::ITexture* l_pTexture = CGlobal::getInstance()->createTexture(a_sTexture);
          if (l_pTexture != nullptr)
            m_pNode->getMaterial(a_iMaterial).setTexture(0, l_pTexture);
        }
      }
    }

    void CLuaSceneNode::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSceneNode>("scenenode")
          .addFunction("setposition", &CLuaSceneNode::setPosition)
          .addFunction("setrotation", &CLuaSceneNode::setRotation)
          .addFunction("setscale"   , &CLuaSceneNode::setScale)
          .addFunction("setvisible" , &CLuaSceneNode::setVisible)
          .addFunction("getposition", &CLuaSceneNode::getPosition)
          .addFunction("isvisible"  , &CLuaSceneNode::isVisible)
          .addFunction("settexture" , &CLuaSceneNode::setTexture)
        .endClass();
    }
  }
}