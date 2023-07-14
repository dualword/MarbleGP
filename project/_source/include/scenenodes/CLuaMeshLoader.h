/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CLuaMeshLoader
    * @author Christian Keimel
    * This is the scene node loader for the LUA Mesh
    */
    class CLuaMeshLoader : public irr::scene::IMeshLoader {
      private:
        irr::IrrlichtDevice       *m_pDevice;
        irr::scene::ISceneManager *m_pSmgr;

      public:
        CLuaMeshLoader(irr::IrrlichtDevice *a_pDevice, irr::scene::ISceneManager *a_pSmgr);
        virtual ~CLuaMeshLoader();

        /**
        * Load a mesh from the given Irrlicht file object
        * @param a_pFile the file to load the mesh from
        */
        virtual irr::scene::IAnimatedMesh *createMesh(irr::io::IReadFile *a_pFile) override;

        /**
        * Returns "true" if the file has a ".luamesh" file extension
        * @param a_sFileName name of the file to check
        * @return true if it seems to be a lua mesh
        */
        virtual bool isALoadableFileExtension(const irr::io::path &a_sFileName) const override;
    };
  }
}
