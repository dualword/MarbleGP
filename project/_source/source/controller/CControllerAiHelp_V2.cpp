// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerAiHelp_V2.h>
#include <_generated/lua/CLuaScript_ai.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CAiPathNode.h>
#include <data/CDataStructs.h>
#include <gui/CGameHUD.h>
#include <CGlobal.h>
#include <cmath>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace controller {
    /**
    * The constructor
    * @param a_iMarbleId the marble ID for this controller
    * @param a_sControls details about the skills of the controller
    * @param a_pMarbles an array of the 16 possible marbles, ID of -1 is not used
    * @param a_pLuaScript an optional LUA script to help the C++ code make decirions
    * @param a_cViewport the viewport of the player, necessary for debug data output
    */
    CControllerAiHelp_V2::CControllerAiHelp_V2(int a_iMarbleId, const std::string& a_sControls, data::SMarblePosition *a_pMarbles, lua::CLuaScript_ai *a_pLuaScript, const irr::core::recti &a_cViewport) :
      CControllerAi_V2(a_iMarbleId, a_sControls, a_pMarbles, a_pLuaScript, a_cViewport), m_bStarting(true)
    {
    }

    CControllerAiHelp_V2::~CControllerAiHelp_V2() {
    }

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
    bool CControllerAiHelp_V2::getControlMessage(
      irr::s32& a_iMarbleId, 
      irr::s8& a_iCtrlX, 
      irr::s8& a_iCtrlY, 
      bool& a_bBrake, 
      bool& a_bRearView, 
      bool& a_bRespawn, 
      enMarbleMode &a_eMode
    ) {
      if (m_aMarbles[m_iIndex].m_iMarbleId == -1)
        return false;

      // The race is starting, we select on section
      if (m_bStarting || m_pCurrent == nullptr) {
        SAiPathSection *l_pStart = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, true, true);

        if (l_pStart != nullptr) {
          m_bStarting = false;
          m_pCurrent = l_pStart;
        }
      }
      else {
        // Transform our velocity to 2d
        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_aMarbles[m_iIndex].m_cPosition + m_pCurrent->m_cNormal, m_aMarbles[m_iIndex].m_cPosition, m_aMarbles[m_iIndex].m_cDirection);

        irr::core::vector3df l_vDummy = m_aMarbles[m_iIndex].m_cPosition + m_aMarbles[m_iIndex].m_cVelocity;
        l_cMatrix.transformVect(l_vDummy);

        m_cVelocity2d.X = l_vDummy.X;
        m_cVelocity2d.Y = l_vDummy.Y;

        irr::core::line2df l_cCheck = irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), m_cVelocity2d.getLengthSQ() != 0.0f ? 250.0f * m_cVelocity2d.normalize() : irr::core::vector2df(0.0f, 0.0f));

        if (m_pDebugRTT != nullptr)
          // If debugging is active we paint the two calculated lines the determine how we move
          m_pDrv->setRenderTarget(m_pDebugRTT, true, false);

        std::vector<SAiPathSection *> l_vCurrent;

        SPathLine2d *l_pPath = m_pCurrent->m_pAiPath->transformTo2d_Help(l_cMatrix, l_cCheck, nullptr);

        if (m_pDebugRTT) {
          draw2dDebugLine(l_cCheck, m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0), m_cOffset);
          m_pCurrent->m_pAiPath->m_cPathLine.debugDraw(m_pDrv, m_cOffset, m_fScale);
        }

        if (m_pDebugRTT != nullptr) {
          m_pDrv->setRenderTarget(nullptr);
        } 

        if (l_pPath != nullptr && l_pPath->m_pParent->m_pParent != m_pCurrent) {
          m_pCurrent = l_pPath->m_pParent->m_pParent;
        }
      }

      return true;
    }
  }
}