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
    * Set the ODE world this singleton can modify
    * @param The world
    */
    void CLuaSingleton_physics::setworld(gameclasses::CWorld *a_world) {
      m_world = a_world;
    }
  }
}