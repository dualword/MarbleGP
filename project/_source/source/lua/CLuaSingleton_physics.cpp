// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_physics.h>

namespace dustbin {
  namespace lua {
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