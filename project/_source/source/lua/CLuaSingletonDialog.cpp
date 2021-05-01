// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_dialog.h>
#include <scenenodes/CGui3dRoot.h>
#include <scenenodes/CSkyBoxFix.h>
#include <CGlobal.h>

/**
* Load an Irrlicht scene file
* @param Name of the 3d Irrlicht scene
*/
void CLuaSingleton_dialog::loadscene(const std::string& a_filename) {
  dustbin::scenenodes::CSkyBoxFix l_pFix = dustbin::scenenodes::CSkyBoxFix(dustbin::CGlobal::getInstance()->getIrrlichtDevice(), a_filename);
  dustbin::CGlobal::getInstance()->getSceneManager()->loadScene(a_filename.c_str());
  l_pFix.hideOriginalSkybox(dustbin::CGlobal::getInstance()->getSceneManager()->getRootSceneNode());
}

/**
* Initialize the 3d UI
*/
void CLuaSingleton_dialog::init3dgui() {
  irr::scene::ISceneManager *l_pSmgr = dustbin::CGlobal::getInstance()->getSceneManager();
  irr::scene::ISceneNode *l_pNode = findscenenode(l_pSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)dustbin::scenenodes::g_i3dGuiRootID);

  if (l_pNode != nullptr) {
    dustbin::scenenodes::CGui3dRoot *l_pRoot = reinterpret_cast<dustbin::scenenodes::CGui3dRoot *>(l_pNode);
    l_pRoot->initGui3d();
    l_pRoot->setCursorControl(dustbin::CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl());
  }
}

/**
* find the first scene node of a specific type
* @param The parent node
*/
irr::scene::ISceneNode* CLuaSingleton_dialog::findscenenode(irr::scene::ISceneNode* a_parent, irr::scene::ESCENE_NODE_TYPE a_type) {
  irr::scene::ISceneNode *l_pRet = nullptr;

  for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_parent->getChildren().begin(); it != a_parent->getChildren().end(); it++) {
    if ((*it)->getType() == a_type)
      return *it;

    l_pRet = findscenenode(*it, a_type);
  }

  return l_pRet;
}
