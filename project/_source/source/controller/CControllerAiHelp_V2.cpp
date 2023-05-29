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
    * Find the next collision (if any) of the velocity line with a border of the 2d path data
    * @param a_cVelocity the current velocity of the marble (with factor depending on the level of help)
    * @param a_pLine the current line to check
    * @param a_cOut the colliding point
    * @return true if the was a collision
    */
    bool CControllerAiHelp_V2::findNextCollision(const irr::core::line2df& a_cVelocity, SPathLine2d* a_pLine, irr::core::vector2df& a_cOut) {
      irr::core::vector2df v;
      if (a_pLine->m_cLines[1].intersectWith(a_cVelocity, v, true)) {
        a_cOut = v;
        return true;
      }
      else if (a_pLine->m_cLines[2].intersectWith(a_cVelocity, v, true)) {
        a_cOut = v;
        return true;
      }

      for (auto& a_cNext : a_pLine->m_vNext) {
        if (findNextCollision(a_cVelocity, a_cNext, a_cOut))
          return true;
      }

      return false;
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
          bool l_bNext = m_p2dPath->m_cLines[0].end.Y > 0.0f && m_p2dPath->m_cLines[1].end.Y > 0.0f && m_p2dPath->m_cLines[2].end.Y > 0.0f;
          if (l_bNext && m_p2dPath->m_vNext.size() > 0) {
            m_pCurrent = (*m_p2dPath->m_vNext.begin())->m_pParent->m_pParent;
          }
        }

        SPathLine2d *l_pSpecial = nullptr;
        // Search for special path segments , i.e. jumps or block
        if (m_p2dPath != nullptr && m_p2dPath->m_vNext.size() > 0)
          l_pSpecial = findNextSpecial(*m_p2dPath->m_vNext.begin());

        if (l_pSpecial != nullptr && l_pSpecial->m_pParent->m_pParent->m_eType != scenenodes::CAiPathNode::Jump)
          l_pSpecial = nullptr;

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

        irr::core::vector2df v1 = irr::core::vector2df(-50000.0f);
        irr::core::vector2df v2 = irr::core::vector2df(-50000.0f);

        calculateControlMessage(a_iCtrlX, a_iCtrlY, a_bBrake, a_bRearView, a_bRespawn, m_eMode, l_pSpecial, v1, v2);
        a_bBrake = false;

        if (m_pDebugPathRTT != nullptr) {
          // If debugging is active we paint the two calculated lines the determine how we move
          m_pDrv->setRenderTarget(m_pDebugPathRTT, true, false);
        }

        if (m_pDebugPathRTT != nullptr) {
          m_p2dPath->debugDraw(m_pDrv, m_cOffset, 2.0f);
        }

        bool l_bVelocity = false;

        l_cVelocity2d = m_cVelocity2d;
        irr::f32 l_fBreak = 1.5f * l_cVelocity2d.getLength();
        irr::f32 l_fDist  = 0.0f;

        SPathLine2d *l_p2dPath = m_p2dPath;
        while (l_p2dPath != nullptr && l_fDist < l_fBreak) {
          irr::core::vector2df v1;

          if (irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), 1.5f * l_cVelocity2d).intersectWith(l_p2dPath->m_cLines[1], v1, true)) {
            l_cVelocity2d = v1 / 1.5f;
            l_bVelocity = true;
            break;
          }
          else if (irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), 1.5f * l_cVelocity2d).intersectWith(l_p2dPath->m_cLines[2], v1, true)) {
            l_bVelocity = true;
            l_cVelocity2d = v1 / 1.5f;
            break;
          }

          if (l_p2dPath != m_p2dPath)
            l_fDist += l_p2dPath->m_cLines[0].getLength();

          if (l_p2dPath->m_vNext.size() > 0)
            l_p2dPath = *l_p2dPath->m_vNext.begin();
          else
            l_p2dPath = nullptr;
        }

        if (m_pDebugPathRTT != nullptr) {
          draw2dDebugLine(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), m_cVelocity2d), m_fScale, irr::video::SColor(0xFF, l_bVelocity ? 0xFF : 0, 0xFF, 0), m_cOffset);
          draw2dDebugLine(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), l_cVelocity2d), m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);
        }

        if (l_pSpecial != nullptr) {
          irr::core::line2df l_cSpecial = irr::core::line2df(l_pSpecial->m_cLines[1].start, l_pSpecial->m_cLines[2].start);

          if (m_pDebugDiceRTT != nullptr)
            draw2dDebugLine(l_cSpecial, m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);

          irr::core::vector2df v;
          if (l_cSpecial.intersectWith(irr::core::line2df(irr::core::vector2df(0.0f), l_cVelocity2d), v)) {
            if (l_cVelocity2d.getLength() < 0.95f * l_pSpecial->m_pParent->m_pParent->m_fBestVel) {
              a_iCtrlY = 127;
              a_bBrake = false;
            }
            else if (l_cVelocity2d.getLength() > 1.1f * l_pSpecial->m_pParent->m_pParent->m_fBestVel) {
              a_iCtrlY = -127;
              a_bBrake = true;
            }
            else if (l_cVelocity2d.getLength() > 1.05f * l_pSpecial->m_pParent->m_pParent->m_fBestVel) {
              a_iCtrlY = -127;
              a_bBrake = false;
            }
            else {
              a_iCtrlY = 0;
            }
            a_eMode  = enMarbleMode::Jump;
          }
        }

        if (a_eMode != enMarbleMode::Jump) {
          // If we detected that we are off track (the orientation of both border lines of the segment to our position is the same, i.e. if we are on the track we
          // have one border on the left and the other one on the right) and we are currently not requesting a respawn we switch the mode to "off-track"
          if (m_p2dPath->m_cLines[1].getPointOrientation(irr::core::vector2df()) > 0 == m_p2dPath->m_cLines[2].getPointOrientation(irr::core::vector2df()) > 0) {
            a_iCtrlY = 64;
          }
          else if (!l_bVelocity || m_cVelocity2d.getLength() < l_cVelocity2d.getLength())
            a_iCtrlY = 127;
          else {
            a_iCtrlY = -127;
            a_bBrake = true;
          }

          a_eMode = m_eMode;
        }

        if (m_pDebugPathRTT != nullptr) {
          m_pDrv->setRenderTarget(nullptr);
        }
      }

      return true;
    }
  }
}