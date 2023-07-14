/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <_generated/luamesh/CLuaScript_meshbuilder.h>
#include <scenenodes/CLuaMeshLoader.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    const irr::io::path c_sExtension = ".luamesh";

    CLuaMeshLoader::CLuaMeshLoader(irr::IrrlichtDevice *a_pDevice, irr::scene::ISceneManager *a_pSmgr) : m_pDevice(a_pDevice), m_pSmgr(a_pSmgr) {
    }

    CLuaMeshLoader::~CLuaMeshLoader() {
    }

    /**
    * Load a mesh from the given Irrlicht file object
    * @param a_pFile the file to load the mesh from
    */
    irr::scene::IAnimatedMesh* CLuaMeshLoader::createMesh(irr::io::IReadFile* a_pFile) {
      char *l_pBuffer = new char[a_pFile->getSize() + 1];
      memset(l_pBuffer, 0, a_pFile->getSize() + 1);
      a_pFile->read(l_pBuffer, a_pFile->getSize());
      std::string l_sScript = l_pBuffer;
      delete []l_pBuffer;

      dustbin::luamesh::CLuaScript_meshbuilder l_cBuilder = dustbin::luamesh::CLuaScript_meshbuilder(l_sScript, m_pDevice);

      irr::scene::IMesh *l_pMesh = l_cBuilder.getmesh();

      return new irr::scene::SAnimatedMesh(l_pMesh);
    }

    /**
    * Returns "true" if the file has a ".luamesh" file extension
    * @param a_sFileName name of the file to check
    * @return true if it seems to be a lua mesh
    */
    bool CLuaMeshLoader::isALoadableFileExtension(const irr::io::path& a_sFileName) const {
      return a_sFileName.subString(a_sFileName.size() - c_sExtension.size(), c_sExtension.size(), true) == c_sExtension;
    }
  }
}