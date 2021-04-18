// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaCameraObject.h>
#include <lua/CLuaTypeHelpers.h>
#include <CGlobal.h>

CLuaCameraObject::CLuaCameraObject() : m_camera(nullptr) {
}

CLuaCameraObject::~CLuaCameraObject() {
}

/**
* Adds a new camera to the scene
*/
void CLuaCameraObject::initialize() {
  irr::scene::ISceneManager *l_pMgr = dustbin::CGlobal::getInstance()->getSceneManager();

  m_camera = l_pMgr->addCameraSceneNode(l_pMgr->getRootSceneNode());
}

/**
* Set the position of the camera
* @param The new camera position
*/
void CLuaCameraObject::setposition(const SVector3d& a_position) {
  if (m_camera != nullptr)
    m_camera->setPosition(dustbin::lua::luaVectorToIrr(a_position));
}

/**
* Make this camera the scene manager's active camera
*/
void CLuaCameraObject::activate() {
  if (m_camera != nullptr)
    m_camera->getSceneManager()->setActiveCamera(m_camera);
}

/**
* Set the target of the camera
* @param The new camera target
*/
void CLuaCameraObject::settarget(const SVector3d& a_target) {
  if (m_camera != nullptr)
    m_camera->setTarget(dustbin::lua::luaVectorToIrr(a_target));
}

/**
* Set the up-vector of the camera
* @param The new camera up-vector
*/
void CLuaCameraObject::setupvector(const SVector3d& a_upvector) {
  if (m_camera != nullptr)
    m_camera->setUpVector(dustbin::lua::luaVectorToIrr(a_upvector));
}
