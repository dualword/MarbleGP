// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerAi_V2.h>
#include <controller/IControllerAI.h>
#include <scenenodes/CAiPathNode.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace lua {
    class CLuaScript_ai;
  }

  namespace controller {
    /**
    * @class CControllerAiHelp_V2
    * @author Christian Keimel
    * The AI help controller using the new AI
    */
    class CControllerAiHelp_V2 : public CControllerAi_V2 {
      private:
        bool m_bStarting;   /**< The race is starting. Necessary for the inital path selection */

        std::vector<SAiPathSection *> m_vCurrent;   /**< The current paths (on a road split more than one is possible, and it's up to the user to decide which way to use) */

      public:
        /**
        * The constructor
        * @param a_iMarbleId the marble ID for this controller
        * @param a_sControls details about the skills of the controller
        * @param a_pMarbles an array of the 16 possible marbles, ID of -1 is not used
        * @param a_pLuaScript an optional LUA script to help the C++ code make decirions
        * @param a_cViewport the viewport of the player, necessary for debug data output
        */
        CControllerAiHelp_V2(int a_iMarbleId, const std::string &a_sControls, data::SMarblePosition *a_pMarbles, lua::CLuaScript_ai *a_pLuaScript, const irr::core::recti &a_cViewport);

        virtual ~CControllerAiHelp_V2();

        /**
        * Get the control values for the marble
        * @param a_iMarbleId [out] ID of the marble this controller controls
        * @param a_iCtrlX [out] the steering value 
        * @param a_iCtrlY [out] the throttle value
        * @param a_bBrake [out] is the brake active?
        * @param a_bRearView [out] does the marble look to the back?
        * @param a_bRespawn [out] does the marble want a manual respawn?
        * @param a_eMode [out] the AI mode the marble is currently in
        */
        virtual bool getControlMessage(
          irr::s32 &a_iMarbleId, 
          irr::s8 &a_iCtrlX, 
          irr::s8 &a_iCtrlY, 
          bool &a_bBrake, 
          bool &a_bRearView, 
          bool &a_bRespawn, 
          enMarbleMode &a_eMode
        ) override;
    };
  }
}
