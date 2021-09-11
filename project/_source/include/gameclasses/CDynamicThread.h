// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <_generated/messages/IDynamicThread.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/SPlayer.h>
#include <threads/IThread.h>
#include <vector>
#include <chrono>

namespace dustbin {
  namespace gameclasses {
    /**
    * Foreward declaration of the CWorld object which
    * contains all physics objects and is defined in
    * the .cpp file
    */
    class CWorld;
    class CObjectMarble;

    /**
    * @class CDynamicThread
    * @author Christian Keimel
    * This class computes all the dynamics of the actual game
    */
    class CDynamicThread : public threads::IThread, public messages::IDynamicThread {
      private:
        CWorld* m_pWorld;
        CObjectMarble* m_aMarbles[16];

        int m_iWorldStep;

        bool m_bPaused;

        std::chrono::high_resolution_clock::time_point m_cNextStep;

        void createPhysicsObjects(irr::scene::ISceneNode* a_pNode);

        void run();

      protected:
        /**
         * This function receives messages of type "MarbleControl"
         * @param a_ObjectId The ID of the Marble
         * @param a_CtrlX The X Control (steer)
         * @param a_CtrlY The Y Control (throttle)
         * @param a_Brake Is the brake active?
         * @param a_RearView Does the player want to look back?
         * @param a_Respawn Is the manual respawn button pressed?
         */
        virtual void onMarblecontrol(irr::s32 a_ObjectId, irr::s8 a_CtrlX, irr::s8 a_CtrlY, bool a_Brake, bool a_RearView, bool a_Respawn);

        virtual void execute();

      public:
        CDynamicThread(scenenodes::CWorldNode *a_pWorld, const std::vector<gameclasses::SPlayer*> &a_vPlayers);

        virtual ~CDynamicThread();
    };
  }
}