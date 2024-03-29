// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
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
          if ((*l_itObj)->m_eJoint == dustbin::gameclasses::CObject::enJointType::Slider) {
            dJointSetSliderParam((*l_itObj)->m_cJoint, dParamFMax, a_force);
            dJointSetSliderParam((*l_itObj)->m_cJoint, dParamVel , a_speed);
          }
          else {
            dJointSetHingeParam((*l_itObj)->m_cJoint, dParamFMax, a_force);
            dJointSetHingeParam((*l_itObj)->m_cJoint, dParamVel , a_speed);
          }

          if (m_dynamics != nullptr && (m_motorParams.find(a_id) == m_motorParams.end() || std::get<0>(m_motorParams[a_id]) != a_speed || std::get<1>(m_motorParams[a_id]) != a_force)) {
            m_motorParams[a_id] = std::make_tuple(a_speed, a_force);
            m_dynamics->jointStartMotor((*l_itObj)->m_iId, a_speed, a_force, (*l_itObj)->m_eJoint);
          }

          break;
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
            if ((*l_itObj)->m_eJoint == dustbin::gameclasses::CObject::enJointType::Slider)
              dJointSetSliderParam((*l_itObj)->m_cJoint, dParamHiStop, a_histop);
            else
              dJointSetHingeParam((*l_itObj)->m_cJoint, dParamHiStop, a_histop);
          }

          if (m_dynamics != nullptr)
            m_dynamics->setJointHiStop(a_id, a_histop);

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
            if ((*l_itObj)->m_eJoint == dustbin::gameclasses::CObject::enJointType::Slider)
              dJointSetSliderParam((*l_itObj)->m_cJoint, dParamLoStop, a_lostop);
            else
              dJointSetHingeParam((*l_itObj)->m_cJoint, dParamLoStop, a_lostop);
          }

          if (m_dynamics != nullptr)
            m_dynamics->setJointLoStop(a_id, a_lostop);

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
            if ((*l_itObj)->m_eJoint == dustbin::gameclasses::CObject::enJointType::Slider)
              dJointSetSliderAxis((*l_itObj)->m_cJoint, a_axis.m_x, a_axis.m_y, a_axis.m_z);
            else
              dJointSetHingeAxis((*l_itObj)->m_cJoint, a_axis.m_x, a_axis.m_y, a_axis.m_z);
          }

          if (m_dynamics != nullptr)
            m_dynamics->setJointAxis(a_id, irr::core::vector3df(a_axis.m_x, a_axis.m_y, a_axis.m_z));

          break;
        }
      }
    }

    /**
    * Send a message to the LUA scripts for scene and AI
    * @param First number for any information
    * @param Other number for any information
    * @param String for any further information
    */
    void CLuaSingleton_physics::sendluamessage(int a_NumberOne, int a_NumberTwo, const std::string& a_Data) {
      if (m_dynamics != nullptr)
        m_dynamics->sendMessageFromLUA(a_NumberOne, a_NumberTwo, a_Data);
    }

    /**
    * A method to forward a trigger to the game state
    * @param The ID of the trigger
    * @param The object that has triggered
    */
    void CLuaSingleton_physics::sendtrigger(int a_TriggerId, int a_ObjectId) {
      if (m_dynamics != nullptr)
        m_dynamics->sendTriggerToUI(a_TriggerId, a_ObjectId);
    }

    /**
    * Set the ODE world this singleton can modify
    * @param The world
    */
    void CLuaSingleton_physics::setworld(gameclasses::CWorld *a_world, gameclasses::CDynamicThread * a_dynamics) {
      m_world    = a_world;
      m_dynamics = a_dynamics;
    }

    /**
    * Add a marble to the LUA physics script
    * @param The marble to add
    * @param ID of the marble (10000 .. 100016)
    */
    void CLuaSingleton_physics::addmarble(gameclasses::CObjectMarble* a_marble, int a_id) {
      int l_iIndex = a_id - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16)
        m_marbles[l_iIndex] = a_marble;
    }
  }
}