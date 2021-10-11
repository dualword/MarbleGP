// (w) 2021 by Dustbin::Games / Christian Keimel

#include <_generated/lua/CLuaSingleton_physics.h>
#include <_generated/lua/CLuaScript_physics.h>
#include <gameclasses/CDynamicThread.h>
#include <gameclasses/COdeNodes.h>
#include <lua/CLuaHelpers.h>

/**
 * Attach the running dynamics thread to this script
 * @param The running dynamics thread
 */
void CLuaScript_physics::setDynamicsThread(dustbin::gameclasses::CDynamicThread* a_Dynamics) {
  m_LuaSgt_physics->setDynamicsThread(a_Dynamics);
}


/**
 * Set the parameters of a joint's motor. Setting velocity and / or force to zero deactivates the motor
 * @param The desired velocity of the joint
 * @param The force used to achieve the velocity
 */
void CLuaSingleton_physics::startmotor(int a_jointid, float a_velocity, float a_force) {
  if (m_Moving.find(a_jointid) != m_Moving.end() && m_Moving[a_jointid]->m_cJoint != 0) {
    dJointID l_cJoint = m_Moving[a_jointid]->m_cJoint;

    dJointSetSliderParam(l_cJoint, dParamVel, a_velocity);
    dJointSetSliderParam(l_cJoint, dParamFMax, a_force);

  }
}

/**
 * Set the marble's contact point which is used to calculate the up-vector of the camera. Useful for long jumps
 * @param ID of the marble
 * @param The new contact point
 */
void CLuaSingleton_physics::setmarbleupvector(int a_marble, const SVector3d& a_contact) {
  if (m_Marbles.find(a_marble) != m_Marbles.end()) {
    m_Marbles[a_marble]->m_vUpVector = dustbin::lua::convertVectorToIrr(a_contact);
  }
}

/**
 * Attach the running dynamics thread to this singleton
 * @param The running dynamics thread
 */
void CLuaSingleton_physics::setDynamicsThread(dustbin::gameclasses::CDynamicThread* a_Dynamics) {
  m_Dynamics = a_Dynamics;

  for (std::vector<dustbin::gameclasses::CObject*>::iterator it = m_Dynamics->getWorld()->m_vObjects.begin(); it != m_Dynamics->getWorld()->m_vObjects.end(); it++) {
    if ((*it)->getType() == dustbin::gameclasses::enObjectType::Marble) {
      m_Marbles[(*it)->m_iId] = reinterpret_cast<dustbin::gameclasses::CObjectMarble*>(*it);
    }
    else {
      m_Objects[(*it)->m_iId] = *it;

      if (!(*it)->m_bStatic)
        m_Moving[(*it)->m_iId] = *it;
    }
  }
}
