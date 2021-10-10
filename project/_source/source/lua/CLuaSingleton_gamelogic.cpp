// (w) 2021 by Dustbin::Games / Christian Keimel

#include <_generated/lua/CLuaSingleton_gamelogic.h>
#include <_generated/lua/CLuaScript_gamelogic.h>

  /**
    * Start a player
    * @param Marble ID of the player to start
    */
void CLuaSingleton_gamelogic::startplayer(int a_Marble) {
  m_Dynamics->startPlayer(a_Marble);
}

/**
 * Set a player to finished
 * @param Marble ID of the player that finished
 * @param Race time of the player in simulation steps
 * @param The number of laps the player has finished
 */
void CLuaSingleton_gamelogic::finishplayer(int a_Marble, int a_Time, int a_Laps) {
  m_Dynamics->finishPlayer(a_Marble, a_Time, a_Laps);
}

/**
 * Stun a player
 * @param Marble ID of the player to be stunnned
 * @param Time that the player will be stunned in simulation steps
 */
void CLuaSingleton_gamelogic::stunnplayer(int a_Marble, int a_StunTime) {
}

/**
 * Respawn a player
 * @param Marble ID of the player to respawn
 */
void CLuaSingleton_gamelogic::respawnplayer(int a_Marble) {
}

/**
 * Attach the running dynamics thread to this singleton
 * @param The running dynamics thread
 */
void CLuaSingleton_gamelogic::setDynamicsThread(dustbin::gameclasses::CDynamicThread *a_Dynamics) {
  m_Dynamics = a_Dynamics;
}

/**
 * Attach the running dynamics thread to this singleton
 * @param The running dynamics thread
 */
void CLuaScript_gamelogic::setDynamicsThread(dustbin::gameclasses::CDynamicThread* a_Dynamics) {
  if (m_LuaSgt_gamelogic != nullptr)
    m_LuaSgt_gamelogic->setDynamicsThread(a_Dynamics);
}
