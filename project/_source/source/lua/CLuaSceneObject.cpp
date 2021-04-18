// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSceneObject.h>
#include <messages/CMessageHelpers.h>
#include <lua/CLuaTypeHelpers.h>
#include <CGlobal.h>

CLuaSceneObject::CLuaSceneObject() : m_node(nullptr) {

}

CLuaSceneObject::~CLuaSceneObject() {
}

/**
* Replace a texture of the scene node by a texture containing some text
* @param The material of which the texture is replaced
* @param The text to be used for the text texture
* @param Width of the generated texture
* @param Height of the generated texture
* @param Generate the texture with transparent background
* @param Center the text horizontally?
* @param Color of the text to render
* @param Background color of the texture
*/
void CLuaSceneObject::settexttexture(int a_material, const std::string& a_text, int a_width, int a_height, bool a_alpha, bool a_center, SColor a_textcolor, SColor a_background) {
  if (m_node == nullptr)
    throw "Scene node object not initialized.";

  irr::gui::IGUIEnvironment *l_pGui = dustbin::CGlobal::getInstance()->getGuiEnvironment();
  irr::video::IVideoDriver  *l_pDrv = dustbin::CGlobal::getInstance()->getVideoDriver();

  irr::gui::IGUIFont *l_pFont = l_pGui->getFont("data/fonts/big.xml");

  std::string l_sName = dustbin::messages::urlEncode(a_text);
  l_sName += std::string("&background=") + dustbin::lua::luaColorToString(a_background);
  l_sName += std::string("&color="     ) + dustbin::lua::luaColorToString(a_textcolor );
  l_sName += "&width="  + std::to_string(a_width );
  l_sName += "&height=" + std::to_string(a_height);
  l_sName += "&center=" + std::string(a_center ? "1" : "0");
  l_sName += "&alpha="  + std::string(a_alpha  ? "1" : "0");
  l_sName = dustbin::messages::urlEncode(l_sName);

  irr::video::ITexture *l_pTexture = l_pDrv->getTexture(l_sName.c_str());

  if (l_pTexture == nullptr) {
    l_pTexture = l_pDrv->addRenderTargetTexture(irr::core::dimension2du(a_width, a_height), l_sName.c_str());
    l_pDrv->setRenderTarget(l_pTexture, true, true, a_alpha ? irr::video::SColor(0, 0, 0, 0) : dustbin::lua::luaColorToIrr(a_background));

    std::string l_sName = std::string("  ") + a_text;

    while (l_pFont->getDimension(irr::core::stringw(l_sName.c_str()).c_str()).Width > (unsigned)a_width)
      l_sName = l_sName.substr(0, l_sName.size() - 3) + "..";

    l_pFont->draw(irr::core::stringw(l_sName.c_str()), irr::core::recti(0, 0, a_width, a_height), dustbin::lua::luaColorToIrr(a_textcolor), a_center, true);
  }

  m_node->getMaterial(a_material).setTexture(0, l_pTexture);

  l_pDrv->setRenderTarget(0, false, false);
}

/**
* Replace a texture of the scene node by an image
* @param The material of which the texture is replaced
* @param The image to be used for the texture
*/
void CLuaSceneObject::setimagetexture(int a_material, const std::string& a_image) {
}

/**
* Set the scale of the scene object
* @param The new scale
*/
void CLuaSceneObject::setscale(const SVector3d& a_scale) {
  if (m_node != nullptr)
    m_node->setScale(dustbin::lua::luaVectorToIrr(a_scale));
}
/**
* Initialize the object with a child of the passed scene object
* @param The parent object which has a named child that is used for initialization
* @param Name of the child to use for initialization of the object
*/
void CLuaSceneObject::initchild(CLuaSceneObject* a_object, const std::string& a_child) {
  irr::scene::ISceneNode *l_pNode = a_object->getNode();

  if (a_object->getNode() == nullptr)
    throw "Cannot create a clone of an un-initialized scene object.";

  if (l_pNode != nullptr) {
    irr::scene::ISceneNode *l_pChild = findChild(l_pNode, a_child);

    if (l_pChild != nullptr)
      m_node = l_pChild;
  }
}

/**
* Initialize the object by cloning the passed node
* @param The scene object to clone
*/
void CLuaSceneObject::initclone(CLuaSceneObject* a_object) {
  if (a_object->getNode() == nullptr)
    throw "Cannot create a clone of an un-initialized scene object.";

  m_node = a_object->getNode()->clone();
}

/**
* Find out whether or not the object is currently visible
*/
bool CLuaSceneObject::isvisible() {
  return m_node != nullptr ? m_node->isVisible() : false;
}
/**
* Initialize the object with a node from the scene
* @param Name of the node in the scene
*/
void CLuaSceneObject::initialize(const std::string& a_nodename) {
  irr::scene::ISceneManager *l_pMgr = dustbin::CGlobal::getInstance()->getSceneManager();
  irr::scene::ISceneNode *l_pNode = l_pMgr->getSceneNodeFromName(a_nodename.c_str());

  if (l_pNode == nullptr)
    throw (std::string("Scene object \"") + a_nodename + std::string("\" not found.")).c_str();

  m_node = l_pNode;
}

/**
* Set the rotation of the scene object in Euler angles
* @param The new rotation.
*/
void CLuaSceneObject::setrotation(const SVector3d& a_rotation) {
  if (m_node != nullptr)
    m_node->setRotation(dustbin::lua::luaVectorToIrr(a_rotation));
}

/**
* Set the position of the scene object
* @param The new position
*/
void CLuaSceneObject::setposition(const SVector3d& a_position) {
  if (m_node != nullptr)
    m_node->setPosition(dustbin::lua::luaVectorToIrr(a_position));
}

/**
* Set the visibility of the scene node
* @param The new visibility flag
*/
void CLuaSceneObject::setvisible(bool a_visible) {
  if (m_node != nullptr)
    m_node->setVisible(a_visible);
}

/**
* Retrieve the position of the scene node
*/
SVector3d CLuaSceneObject::getposition() {
  if (m_node != nullptr)
    return dustbin::lua::irrVectorToLua(m_node->getPosition());

  return SVector3d();
}

/**
* Search for a child of this scene node
* @param The parent node to search for the child in
* @param The name of the requested child node
*/
irr::scene::ISceneNode* CLuaSceneObject::findChild(irr::scene::ISceneNode* a_parent, const std::string& a_name) {
  if (a_name == a_parent->getName())
    return a_parent;

  for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_parent->getChildren().begin(); it != a_parent->getChildren().end(); it++) {
    irr::scene::ISceneNode *l_pChild = findChild(*it, a_name);
    if (l_pChild != nullptr)
      return l_pChild;
  }

  return nullptr;
}

/**
* Get the Irrlicht scene node attached to the LUA scene node instance
*/
irr::scene::ISceneNode* CLuaSceneObject::getNode() {
  return m_node;
}
