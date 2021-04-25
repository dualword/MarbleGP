#include <scenenodes/CMarbleGPSceneNodeFactory.h>

#include <irrlicht.h>
#include <irredit.h>

// this example shows how to add own scene nodes into irrEdit.
// You only need two things:
// - a scene node factory 
// - your scene node.
// you can add as many scene nodes with a plugin as you wish.


// entry point for the plugin, will be called by the editor. Return a
// plugin interface if your plugin should run as plugin inside the editor, otherwise
// just modify the engine (add factories etc) and return 0.
__declspec(dllexport) irr::irredit::IIrrEditPlugin* __stdcall createPlugin(irr::irredit::IrrEditServices *services) {
	// we simple register a new scene node factory here, which is only able to
	// create our example scene node

	irr::scene::ISceneManager *l_pSmgr = services->getDevice()->getSceneManager();

	dustbin::scenenodes::CMarbleGPSceneNodeFactory *l_pFactory = new dustbin::scenenodes::CMarbleGPSceneNodeFactory(l_pSmgr);
	l_pSmgr->registerSceneNodeFactory(l_pFactory);
	l_pFactory->drop();
	// CMarbles3SceneNodeFactory* factory = new CMarbles3SceneNodeFactory(mgr);
	// mgr->registerSceneNodeFactory(factory);
	// factory->drop();

	return 0;
}



// -------------------------------------------------------------------------------
// example factory implementation
// -------------------------------------------------------------------------------

