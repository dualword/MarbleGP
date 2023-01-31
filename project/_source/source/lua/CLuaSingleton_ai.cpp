// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/lua/CLuaSingleton_ai.h>
#include <_generated/lua/CLuaScript_ai.h>

namespace dustbin {
  namespace lua {
    /**
    * Get the position of a (non marble) object in the level
    * @param ID of the object
    */
    SVector3d CLuaSingleton_ai::getobjectposition(int a_objectid) {
      SVector3d l_cRet;

      if (m_objects.find(a_objectid) != m_objects.end()) {
        l_cRet.m_x = std::get<0>(m_objects[a_objectid]).X;
        l_cRet.m_y = std::get<0>(m_objects[a_objectid]).Y;
        l_cRet.m_z = std::get<0>(m_objects[a_objectid]).Z;
      }

      return l_cRet;
    }

    /**
    * Get the rotation of a (non marble) object in the level
    * @param ID of the object
    */
    SVector3d CLuaSingleton_ai::getobjectrotation(int a_objectid) {
      SVector3d l_cRet;

      if (m_objects.find(a_objectid) != m_objects.end()) {
        l_cRet.m_x = std::get<1>(m_objects[a_objectid]).X;
        l_cRet.m_y = std::get<1>(m_objects[a_objectid]).Y;
        l_cRet.m_z = std::get<1>(m_objects[a_objectid]).Z;
      }

      return l_cRet;
    }

    /**
    * Get the linear velocity of a (non marble) object in the level
    * @param ID of the object
    */
    SVector3d CLuaSingleton_ai::getobjectvelocity(int a_objectid) {
      SVector3d l_cRet;
      
      if (m_objects.find(a_objectid) != m_objects.end()) {
        l_cRet.m_x = std::get<2>(m_objects[a_objectid]).X;
        l_cRet.m_y = std::get<2>(m_objects[a_objectid]).Y;
        l_cRet.m_z = std::get<2>(m_objects[a_objectid]).Z;
      }

      return l_cRet;
    }

    /**
    * Little helper function to turn the data retrieved from getobjectrotation to something usefule.
    * @param The vector to rotate
    * @param Euler rotation to rotate the vector by
    */
    SVector3d CLuaSingleton_ai::rotatevector(const SVector3d& a_vector, const SVector3d& a_rotation) {
      SVector3d l_cRet;

      irr::core::matrix4 l_cMatrix = irr::core::matrix4();
      l_cMatrix = l_cMatrix.setRotationDegrees(irr::core::vector3df(a_rotation.m_x, a_rotation.m_y, a_rotation.m_z));

      irr::core::vector3df l_cVector = irr::core::vector3df(a_vector.m_x, a_vector.m_y, a_vector.m_z);

      l_cMatrix.rotateVect(l_cVector);

      l_cRet.m_x = l_cVector.X;
      l_cRet.m_y = l_cVector.Y;
      l_cRet.m_z = l_cVector.Z;

      return l_cRet;
    }

    /**
    * Callback to inform the singleton about a moved object
    * @param ID of the object
    * @param The new position of the object
    * @param The new rotation of the object
    * @param The new linear veclocity of the object
    */
    void CLuaSingleton_ai::objectmoved(int a_objectid, const irr::core::vector3df& a_position, const irr::core::vector3df& a_rotation, const irr::core::vector3df& a_veclocity) {
      m_objects[a_objectid] = std::make_tuple(a_position, a_rotation, a_veclocity);
    }

    /**
    * Get the AI LUA singleton
    */
    CLuaSingleton_ai* CLuaScript_ai::getsingleton() {
      return m_LuaSgt_ai;
    }
  }
}