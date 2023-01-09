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
      m_cAiData.m_iAvoid       = -1;
      m_cAiData.m_iRoadBlock   = -1;
      m_cAiData.m_fThrottleAdd = 1.0f;
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
        if (m_p2dPath != nullptr) {
          if (m_p2dPath->m_cLines[0].end.Y > 0.0f && m_p2dPath->m_vNext.size() > 0) {
            m_pCurrent = (*m_p2dPath->m_vNext.begin())->m_pParent->m_pParent;
          }
        }

        // Transform our velocity to 2d
        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_aMarbles[m_iIndex].m_cPosition + m_pCurrent->m_cNormal, m_aMarbles[m_iIndex].m_cPosition, m_aMarbles[m_iIndex].m_cDirection);

        irr::core::vector3df l_vDummy = m_aMarbles[m_iIndex].m_cPosition + m_aMarbles[m_iIndex].m_cVelocity;
        l_cMatrix.transformVect(l_vDummy);

        m_cVelocity2d.X = l_vDummy.X;
        m_cVelocity2d.Y = l_vDummy.Y;

        irr::core::vector2df l_cVelocity2d = m_cVelocity2d;

        irr::core::line2df l_cCheck = irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), m_cVelocity2d.getLengthSQ() != 0.0f ? 250.0f * l_cVelocity2d.normalize() : irr::core::vector2df(0.0f, 0.0f));

        std::vector<SAiPathSection *> l_vCurrent;

        m_p2dPath = m_pCurrent->m_pAiPath->transformTo2d_Help(l_cMatrix, l_cCheck, nullptr);

        if (m_pDebugPathRTT != nullptr) {
          // If debugging is active we paint the two calculated lines the determine how we move
          m_pDrv->setRenderTarget(m_pDebugPathRTT, true, false);
        }

        SPathLine2d *l_pSpecial = nullptr;
        // Search for special path segments , i.e. jumps or block
        if (m_p2dPath != nullptr && m_p2dPath->m_vNext.size() > 0)
          l_pSpecial = findNextSpecial(*m_p2dPath->m_vNext.begin());

        if (m_p2dPath->m_cLines[1].getPointOrientation(irr::core::vector2df()) > 0 ==  m_p2dPath->m_cLines[2].getPointOrientation(irr::core::vector2df()) > 0) {
          // If we detected that we are off track (the orientation of both border lines of the segment to our position is the same, i.e. if we are on the track we
          // have one border on the left and the other one on the right) and we are currently not requesting a respawn we switch the mode to "off-track"
          if (m_eMode != enMarbleMode::Respawn && m_eMode != enMarbleMode::Respawn2)
            switchMarbleMode(enMarbleMode::OffTrack);
        }
        else {
          if (m_eMode == enMarbleMode::OffTrack || m_eMode == enMarbleMode::Respawn || m_eMode == enMarbleMode::Respawn2)
            switchMarbleMode(enMarbleMode::Cruise);

          if (l_pSpecial != nullptr)
            processNextSpecial(l_pSpecial, m_cVelocity2d.getLength());
        }

        irr::core::vector2df v1 = irr::core::vector2df(-50000.0f);
        irr::core::vector2df v2 = irr::core::vector2df(-50000.0f);

        m_fVCalc = -1.0f;
        calculateControlMessage(a_iCtrlX, a_iCtrlY, a_bBrake, a_bRearView, a_bRespawn, a_eMode, l_pSpecial, v1, v2);

        if (m_eMode == enMarbleMode::Default || m_eMode == enMarbleMode::Cruise || m_eMode == enMarbleMode::TimeAttack) {
          l_cVelocity2d = 1.15f * m_cVelocity2d;

          irr::core::vector2df l_cOut = irr::core::vector2df();
          bool b = false;

          if (m_p2dPath->getFirstCollisionLine(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), l_cVelocity2d), l_cOut, l_cVelocity2d.getLength(), true, b)) {
            l_cVelocity2d = l_cOut;
            b = true;
          }
          else b = false;

          if (m_pDebugPathRTT != nullptr) {
            draw2dDebugLine(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), l_cVelocity2d), m_fScale, b ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0xFF, 0), m_cOffset);
          }

          if (b) {
            a_bBrake = m_cVelocity2d.getLength() > 1.05f * l_cVelocity2d.getLength();
            a_iCtrlY = -127;
          }
          else {
            a_iCtrlY = 127;
          }
        }

        if (m_pDebugPathRTT != nullptr) {
          m_pDrv->setRenderTarget(nullptr);
        }

        a_eMode = m_eMode;
      }

      return true;
    }
  }
}