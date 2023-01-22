/* Generated by the Dustbin::Games LuaBind Python Script (w) 2020 by Christian Keimel / Dustbin::Games */
#pragma once

#include <lua/luawrap.hpp>
#include <lua/ILuaClass.h>
#include <_generated/lua/CLuaSingleton_system.h>

namespace dustbin {
  namespace lua {

    /**
     * @class CCLuaScript_ai
     * @author Dustbin::Games LuaBind Python Script
     * A LUA script to help the AI make decisions
     */
    class CLuaScript_ai : public ILuaClass {
      protected:

        CLuaSingleton_system *m_LuaSgt_system;

      private:
        void *getObjectFromLuaStack(const std::string &a_sIdentifier);

      public:
        CLuaScript_ai(const std::string &a_sScript);
        virtual ~CLuaScript_ai();

        /**
         * Inform the script about the marble class (0 == MarbleGP, 1 == Marble2, 2 == Marble3)
         * @param The marble class of the AI controlled marble
         */
        void marbleclass(int a_class);
        /**
         * Triggered on every simulation step
         * @param The current step number
         */
        void onstep(int a_stepno);
        /**
         * Triggered whenever a marble passes a trigger
         * @param The ID of the marble that passed the trigger
         * @param The ID of the trigger that the marble passed
         */
        void ontrigger(int a_objectid, int a_triggerid);
        /**
         * Called whenever a marble passes a checkpoint
         * @param The ID of the marble that passed the trigger
         * @param The ID of the checkpoint that the marble passed
         */
        void oncheckpoint(int a_objectid, int a_checkpointid);
        /**
         * Update on the race positioning
         * @param The marble of the updated position
         * @param The new position of the marble
         * @param The marble's current lap
         * @param The deficit in steps to the marble ahead, negative number shows a deficit in laps
         * @param The deficit in steps to the leading marble, negative number shows a deficit in laps
         */
        void onraceposition(int a_marbleid, int a_position, int a_laps, int a_deficitA, int a_deficitL);
        /**
         * A player respawns
         * @param ID of the player's marble
         * @param Respawn state (1 == respawn start, 2 == camera respawn, 3 == respawn done)
         */
        void onplayerrespawn(int a_marbleid, int a_state);
        /**
         * Called when a road split is ahead of the marble
         * @param The marble of the updated position
         * @param Tag defined in the track file to identify the split
         */
        int decide_roadsplit(int a_marbleid, int a_tag);
        /**
         * Called when a block is ahead of the marble
         * @param The marble of the updated position
         * @param Tag defined in the track file to identify the split
         */
        bool decide_blocker(int a_marbleid, int a_tag);
        /**
         * A LUA message was received from the physics LUA script
         * @param First number for any information
         * @param Other number for any information
         * @param String for any further information
         */
        void onluamessage(int a_NumberOne, int a_NumberTwo, const std::string &a_Data);
    };
  }
}
