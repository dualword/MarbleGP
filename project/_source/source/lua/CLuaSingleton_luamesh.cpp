/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <_generated/luamesh/CLuaSingleton_luamesh.h>

namespace dustbin {
  namespace luamesh {
    /**
    * Convert an Irrlicht color struct to LUA color
    * @param a_cColor the Irrlicht color struct
    * @return a LUA color table with the values of the Irrlicht color
    */
    SColor irrColorToLuaColor(const irr::video::SColor& a_cColor) {
      SColor l_cRet;

      l_cRet.m_a = a_cColor.getAlpha();
      l_cRet.m_r = a_cColor.getRed  ();
      l_cRet.m_g = a_cColor.getGreen();
      l_cRet.m_b = a_cColor.getBlue ();

      return l_cRet;
    }

    /**
    * Convert a LUA color table to Irrlicht color
    * @param a_cColor the LUA color table
    * @return an Irrlicht color with the values of the LUA table
    */
    irr::video::SColor luaColorToIrrColor(const SColor& a_cColor) {
      return irr::video::SColor((irr::u32)a_cColor.m_a, (irr::u32)a_cColor.m_r, (irr::u32)a_cColor.m_g, (irr::u32)a_cColor.m_b);
    }

    /**
    * Add a mesh buffer to the mesh
    */
    int CLuaSingleton_luamesh::addmeshbuffer() {
      if (m_mesh == nullptr) {
        m_mesh = new irr::scene::SMesh();
      }

      irr::scene::SMeshBuffer *l_pBuffer = new irr::scene::SMeshBuffer();
      m_mesh->addMeshBuffer(l_pBuffer);

      return m_mesh->getMeshBufferCount() - 1;
    }

    /**
    * Add a list of vertices to a meshbuffer
    * @param a_meshbuffer index of the mesh buffer to add the vertices to
    * @param a_vertices The vertices to add
    * @param a_indices The indices to add
    */
    void CLuaSingleton_luamesh::addvertices(int a_meshbuffer, std::vector<SVertex3d> a_vertices, std::vector<int> a_indices) {
      if (m_mesh == nullptr) {
        m_mesh = new irr::scene::SMesh();
      }

      if ((irr::u32)a_meshbuffer > m_mesh->getMeshBufferCount()) {
        printf("Invalid mesh buffer index #%i\n", a_meshbuffer);
        return;
      }

      std::vector<irr::u16> l_indices;
      for (std::vector<int>::iterator l_itIdx = a_indices.begin(); l_itIdx != a_indices.end(); l_itIdx++)
        l_indices.push_back((irr::u16)*l_itIdx);

      std::vector<irr::video::S3DVertex> l_vertices;

      for (std::vector<SVertex3d>::iterator l_itVtx = a_vertices.begin(); l_itVtx != a_vertices.end(); l_itVtx++) {
        l_vertices.push_back(irr::video::S3DVertex(
          (*l_itVtx).m_position.m_x,
          (*l_itVtx).m_position.m_y,
          (*l_itVtx).m_position.m_z,
          0.0f,
          1.0f,
          0.0f,
          irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF),
          (*l_itVtx).m_texture.m_x,
          (*l_itVtx).m_texture.m_y
        ));
      }

      m_mesh->getMeshBuffer(a_meshbuffer)->append((void *)l_vertices.data(), (irr::u32)a_vertices.size(), l_indices.data(), (irr::u32)l_indices.size());
    }

    /**
    * Add a list of vertices to a meshbuffer
    * @param a_meshbuffer index of the mesh buffer to add the vertices to
    * @param a_vertices The vertices to add
    * @param a_indices The indices to add
    */
    void CLuaSingleton_luamesh::addvertices_2TextureCoords(int a_meshbuffer, std::vector<SVertex3d_2TextureCoords> a_vertices, std::vector<int> a_indices) {
      if (m_mesh == nullptr) {
        m_mesh = new irr::scene::SMesh();
      }

      irr::core::aabbox3df l_cBox = m_mesh->getBoundingBox();

      if ((irr::u32)a_meshbuffer > m_mesh->getMeshBufferCount()) {
        printf("Invalid mesh buffer index #%i\n", a_meshbuffer);
        return;
      }

      std::vector<irr::u16> l_indices;
      for (std::vector<int>::iterator l_itIdx = a_indices.begin(); l_itIdx != a_indices.end(); l_itIdx++)
        l_indices.push_back((irr::u16)*l_itIdx);

      std::vector<irr::video::S3DVertex> l_vertices;

      for (std::vector<SVertex3d_2TextureCoords>::iterator l_itVtx = a_vertices.begin(); l_itVtx != a_vertices.end(); l_itVtx++) {
        l_vertices.push_back(irr::video::S3DVertex2TCoords(
          (*l_itVtx).m_position.m_x,
          (*l_itVtx).m_position.m_y,
          (*l_itVtx).m_position.m_z,
          0.0f,
          1.0f,
          0.0f,
          irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF),
          (*l_itVtx).m_texture.m_x,
          (*l_itVtx).m_texture.m_y,
          (*l_itVtx).m_texture2.m_x,
          (*l_itVtx).m_texture2.m_y
        ));

        l_cBox.addInternalPoint(irr::core::vector3df((*l_itVtx).m_position.m_x, (*l_itVtx).m_position.m_y, (*l_itVtx).m_position.m_z));
      }

      m_mesh->setBoundingBox(l_cBox);
      m_mesh->getMeshBuffer(a_meshbuffer)->append((void *)l_vertices.data(), (irr::u32)a_vertices.size(), l_indices.data(), (irr::u32)l_indices.size());
    }
    /**
    * Get the mesh created by the script
    */
    irr::scene::SMesh* CLuaSingleton_luamesh::getmesh() {
      return m_mesh;
    }

    /**
    * Get the material table of a meshbuffer
    * @param Index of the mesh buffer
    */
    SMaterial CLuaSingleton_luamesh::getmaterial(int a_index) {
      SMaterial l_cRet;

      if (m_mesh != nullptr && a_index >= 0 && a_index < (int)m_mesh->getMeshBufferCount()) {
        irr::video::SMaterial l_cMaterial = m_mesh->getMeshBuffer(a_index)->getMaterial();

        l_cRet.m_AmbientColor     = irrColorToLuaColor(l_cMaterial.AmbientColor);
        l_cRet.m_AntiAliasing     = l_cMaterial.AntiAliasing;
        l_cRet.m_BackfaceCulling  = l_cMaterial.BackfaceCulling;
        l_cRet.m_BlendOperation   = (MaterialBlendOperations)l_cMaterial.BlendOperation;
        l_cRet.m_DiffuseColor     = irrColorToLuaColor(l_cMaterial.DiffuseColor);
        l_cRet.m_EmissiveColor    = irrColorToLuaColor(l_cMaterial.EmissiveColor);
        l_cRet.m_FogEnable        = l_cMaterial.FogEnable;
        l_cRet.m_GouraudShading   = l_cMaterial.GouraudShading;
        l_cRet.m_Lighting         = l_cMaterial.Lighting;
        l_cRet.m_MaterialType     = (MaterialType)l_cMaterial.MaterialType;
        l_cRet.m_NormalizeNormals = l_cMaterial.NormalizeNormals;
        l_cRet.m_Shininess        = l_cMaterial.Shininess;
        l_cRet.m_SpecularColor    = irrColorToLuaColor(l_cMaterial.SpecularColor);
        l_cRet.m_Thickness        = l_cMaterial.Thickness;
        l_cRet.m_Wireframe        = l_cMaterial.Wireframe;
      }

      return l_cRet;
    }

    /**
    * Set the material table of a meshbuffer
    * @param Index of the mesh buffer
    * @param The material parameters for the mesh buffer
    */
    void CLuaSingleton_luamesh::setmaterial(int a_index, const SMaterial& a_material) {
      if (m_mesh != nullptr && a_index >= 0 && a_index < (int)m_mesh->getMeshBufferCount()) {
        m_mesh->getMeshBuffer(a_index)->getMaterial().AmbientColor     = luaColorToIrrColor(a_material.m_AmbientColor);
        m_mesh->getMeshBuffer(a_index)->getMaterial().AntiAliasing     = a_material.m_AntiAliasing;
        m_mesh->getMeshBuffer(a_index)->getMaterial().BackfaceCulling  = a_material.m_BackfaceCulling;
        m_mesh->getMeshBuffer(a_index)->getMaterial().BlendOperation   = (irr::video::E_BLEND_OPERATION)a_material.m_BlendOperation;
        m_mesh->getMeshBuffer(a_index)->getMaterial().DiffuseColor     = luaColorToIrrColor( a_material.m_DiffuseColor);
        m_mesh->getMeshBuffer(a_index)->getMaterial().EmissiveColor    = luaColorToIrrColor(a_material.m_EmissiveColor);
        m_mesh->getMeshBuffer(a_index)->getMaterial().FogEnable        = a_material.m_FogEnable;
        m_mesh->getMeshBuffer(a_index)->getMaterial().GouraudShading   = a_material.m_GouraudShading;
        m_mesh->getMeshBuffer(a_index)->getMaterial().Lighting         = a_material.m_Lighting; 
        m_mesh->getMeshBuffer(a_index)->getMaterial().MaterialType     = (irr::video::E_MATERIAL_TYPE)a_material.m_MaterialType;
        m_mesh->getMeshBuffer(a_index)->getMaterial().NormalizeNormals = a_material.m_NormalizeNormals;
        m_mesh->getMeshBuffer(a_index)->getMaterial().Shininess        = a_material.m_Shininess;
        m_mesh->getMeshBuffer(a_index)->getMaterial().SpecularColor    = luaColorToIrrColor(a_material.m_SpecularColor);
        m_mesh->getMeshBuffer(a_index)->getMaterial().Thickness        = a_material.m_Thickness;
        m_mesh->getMeshBuffer(a_index)->getMaterial().Wireframe        = a_material.m_Wireframe;
      }
    }

    /**
    * Set the texture of a mesh buffer material
    * @param Index of the mesh buffer
    * @param Index of the texture
    * @param Filename of the texture
    */
    void CLuaSingleton_luamesh::settexture(int a_index, int a_texture, const std::string& a_filename) {
      if (m_device != nullptr && m_mesh != nullptr && a_index >= 0 && a_index < (int)m_mesh->getMeshBufferCount()) {
        irr::video::ITexture *l_pTexture = m_device->getVideoDriver()->getTexture(a_filename.c_str());

        if (l_pTexture != nullptr)
          m_mesh->getMeshBuffer(a_index)->getMaterial().setTexture((irr::u32)a_texture, l_pTexture);
      }
    }

    /**
    * End the creation of the mesh
    */
    void CLuaSingleton_luamesh::finish() {
      irr::scene::IMeshManipulator *l_pManipulator = m_device->getSceneManager()->getMeshManipulator();
      l_pManipulator->recalculateNormals(m_mesh, true);
      m_mesh->recalculateBoundingBox();
    }
  }
}