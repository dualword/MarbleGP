// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_physics.h>

namespace dustbin {
  namespace gameclasses {
    irr::core::vector3df quaternionToEuler(const dQuaternion a_aQuaternion);
  }

  namespace lua {
    void eulerToQuaternion(dReal *a_pOut, const irr::core::vector3df &a_cRot) {
      double cr = cos(a_cRot.X * 0.5);
      double sr = sin(a_cRot.X * 0.5);
      double cp = cos(a_cRot.Y * 0.5);
      double sp = sin(a_cRot.Y * 0.5);
      double cy = cos(a_cRot.Z * 0.5);
      double sy = sin(a_cRot.Z * 0.5);

      a_pOut[0] = cr * cp * cy + sr * sp * sy;
      a_pOut[1] = sr * cp * cy - cr * sp * sy;
      a_pOut[2] = cr * sp * cy + sr * cp * sy;
      a_pOut[3] = cr * cp * sy - sr * sp * cy;
    }

    /**
    * Start a motor in the scene by it's ID with a desired speed and force
    * @param ID of the motor
    * @param The desired speed
    * @param The force used to reach to speed
    */
    void CLuaSingleton_physics::startmotor(int a_id, float a_speed, float a_force) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id && (*l_itObj)->m_cJoint != nullptr) {
          if ((*l_itObj)->m_bSliderJoint) {
            dJointSetSliderParam((*l_itObj)->m_cJoint, dParamFMax, a_force);
            dJointSetSliderParam((*l_itObj)->m_cJoint, dParamVel , a_speed);
          }
          else {
            dJointSetHingeParam((*l_itObj)->m_cJoint, dParamFMax, a_force);
            dJointSetHingeParam((*l_itObj)->m_cJoint, dParamVel , a_speed);
          }
        }
      }
    }

    /**
    * Modify the up-vector of a camera, useful for long jumps
    * @param ID of the marble
    * @param The new up-vector of the marble's camera
    */
    void CLuaSingleton_physics::setmarblecameraup(int a_marbleid, const SVector3d& a_upvector) {
      int l_iIndex = a_marbleid - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16 && m_marbles[l_iIndex] != nullptr)
        m_marbles[l_iIndex]->m_vUpVector = irr::core::vector3df(a_upvector.m_x, a_upvector.m_y, a_upvector.m_z);
    }

    /**
    * Get the rotation of a scene node (Euler degrees)
    * @param ID of the scene node
    */
    SVector3d CLuaSingleton_physics::getrotation(int a_id) {
      SVector3d l_cRet;

      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          const dReal* l_aRot = dBodyGetQuaternion((*l_itObj)->m_cBody);
          irr::core::vector3df l_cRot = gameclasses::quaternionToEuler(l_aRot);
          l_cRet.m_x = l_cRot.X;
          l_cRet.m_y = l_cRot.Y;
          l_cRet.m_z = l_cRot.Z;
          break;
        }
      }

      return l_cRet;
    }

    /**
    * Set the rotation of a scene node (Euler degrees)
    * @param ID of the scene node
    * @param The new rotation of the object
    */
    void CLuaSingleton_physics::setrotation(int a_id, const SVector3d& a_rotation) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          irr::core::vector3df v = irr::core::vector3df(a_rotation.m_x, a_rotation.m_y, a_rotation.m_z);
          dQuaternion l_aRot;
          eulerToQuaternion(l_aRot, v);
          dBodySetQuaternion((*l_itObj)->m_cBody, l_aRot);
        }
      }
    }
    /**
    * Set the angular velocity of a scene node (Euler degrees)
    * @param ID of the scene node
    * @param The new angular velocity of the object
    */
    void CLuaSingleton_physics::setangularvel(int a_id, const SVector3d& a_velocity) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          dBodySetAngularVel((*l_itObj)->m_cBody, a_velocity.m_x, a_velocity.m_y, a_velocity.m_z);
        }
      }
    }

    /**
    * Change the hi-stop of a joint attached to a physics body
    * @param ID of the physics object
    * @param The new hi-stop value
    */
    void CLuaSingleton_physics::sethistop(int a_id, float a_histop) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          if ((*l_itObj)->m_cJoint != nullptr) {
            if ((*l_itObj)->m_bSliderJoint)
              dJointSetSliderParam((*l_itObj)->m_cJoint, dParamHiStop, a_histop);
            else
              dJointSetHingeParam((*l_itObj)->m_cJoint, dParamHiStop, a_histop);
          }
          break;
        }
      }
    }
    /**
    * Change the lo-stop of a joint attached to a physics body
    * @param ID of the physics object
    * @param The new lo-stop value
    */
    void CLuaSingleton_physics::setlostop(int a_id, float a_lostop) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          if ((*l_itObj)->m_cJoint != nullptr) {
            if ((*l_itObj)->m_bSliderJoint)
              dJointSetSliderParam((*l_itObj)->m_cJoint, dParamLoStop, a_lostop);
            else
              dJointSetHingeParam((*l_itObj)->m_cJoint, dParamLoStop, a_lostop);
          }
          break;
        }
      }
    }

    /**
    * Set the axis of a joint attached to a physics body
    * @param ID of the physics object
    * @param The new axis
    */
    void CLuaSingleton_physics::setjointaxis(int a_id, const SVector3d& a_axis) {
      for (std::vector<gameclasses::CObject*>::iterator l_itObj = m_world->m_vMoving.begin(); l_itObj != m_world->m_vMoving.end(); l_itObj++) {
        if ((*l_itObj)->m_iId == a_id) {
          if ((*l_itObj)->m_cJoint != nullptr) {
            if ((*l_itObj)->m_bSliderJoint)
              dJointSetSliderAxis((*l_itObj)->m_cJoint, a_axis.m_x, a_axis.m_y, a_axis.m_z);
            else
              dJointSetHingeAxis((*l_itObj)->m_cJoint, a_axis.m_x, a_axis.m_y, a_axis.m_z);
          }
          break;
        }
      }
    }

    /**
    * Set the ODE world this singleton can modify
    * @param The world
    */
    void CLuaSingleton_physics::setworld(gameclasses::CWorld *a_world, gameclasses::CObjectMarble * a_marbles[16]) {
      m_world   = a_world;
      
      for (int i = 0; i < 16; i++)
        m_marbles[i] = a_marbles[i];
    }
  }
}