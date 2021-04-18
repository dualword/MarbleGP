// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_dialog.h>
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
