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
      CControllerAi_V2(a_iMarbleId, a_sControls, a_pMarbles, a_pLuaScript, a_cViewport)
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

      a_iMarbleId = m_iMarbleId;
      a_iCtrlX    = 0;
      a_iCtrlY    = 0;
      a_bBrake    = false;
      a_bRearView = false;
      a_bRespawn  = false;
      a_eMode     = m_eMode;

      m_vDebugText.clear();


      // A vector with all AI path sections the
      // player's marble is currently in
      std::vector<SAiPathSection *> l_vCurrent;
      fillPathVector(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, l_vCurrent);

      if (l_vCurrent.size() == 0) {
        // switchMarbleMode(enMarbleMode::OffTrack);
        m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, m_iLastCheckpoint == -1, true);

        if (m_pCurrent == nullptr && m_iLastCheckpoint == -1)
          m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, false, true);

        if (m_pCurrent != nullptr)
          l_vCurrent.push_back(m_pCurrent);
      }

      // The potential control lines. We'll choose one later on (line1, line2, the AI path selection of the lines and the new marble mode)
      std::vector<std::tuple<irr::core::line2df, irr::core::line2df, SAiPathSection *, enMarbleMode>> l_vCtrlLines;

      // If we do not yet know where we are we have a look
      if (m_pCurrent == nullptr) {
        m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, m_iLastCheckpoint == -1, true);

        if (m_pCurrent == nullptr && m_iLastCheckpoint == -1)
          m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, false, true);
      }

      SPathLine2d *l_pSpecial = nullptr;

      // Clear the render target if debugging is active
      if (m_pDebugRTT != nullptr)
        m_pDrv->setRenderTarget(m_pDebugRTT, true, false);

      // Create a matrix to transform the ai data to 2d
      irr::core::matrix4 l_cMatrix;
      l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_aMarbles[m_iIndex].m_cPosition + m_pCurrent->m_cNormal, m_aMarbles[m_iIndex].m_cPosition, m_aMarbles[m_iIndex].m_cDirection);

      // Transform our velocity to 2d
      irr::core::vector3df l_vDummy = m_aMarbles[m_iIndex].m_cPosition + m_aMarbles[m_iIndex].m_cVelocity;
      l_cMatrix.transformVect(l_vDummy);

      m_cVelocity2d.X = 1.15f * l_vDummy.X;
      m_cVelocity2d.Y = 1.15f * l_vDummy.Y;

      irr::core::line2df l_cVelocity = irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), m_cVelocity2d);

      std::vector<const data::SMarblePosition *> l_vMarbles;
      for (int i = 0; i < 16; i++) {
        if (m_aMarbles[i].m_iMarbleId != -1 && m_aMarbles[i].m_iMarbleId != m_iMarbleId)
          l_vMarbles.push_back(&m_aMarbles[i]);
      }

      for (std::vector<SAiPathSection *>::iterator l_itCurrent = l_vCurrent.begin(); l_itCurrent != l_vCurrent.end(); l_itCurrent++) {
        // Update the current path section if necessary
        m_pCurrent = *l_itCurrent;

        std::vector<SPathLine2d *> l_vOptions;
        std::vector<std::tuple<int, irr::core::vector3df, irr::core::vector3df>> l_vMarblePosVel;
        getAllPathOptions(m_pCurrent, l_vOptions, l_vMarbles, l_vMarblePosVel);

        for (std::vector<SPathLine2d*>::iterator l_itOption = l_vOptions.begin(); l_itOption != l_vOptions.end(); l_itOption++) {
          m_p2dPath = *l_itOption;

          if (m_pDebugRTT != nullptr)
            m_p2dPath->debugDraw(m_pDrv, m_cOffset, m_fScale);

          irr::core::vector2df l_cIntersect;
          bool l_bCenter  = false;
          bool l_bCollide = false;

          if (m_p2dPath->getFirstCollisionLine(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), m_cVelocity2d), l_cIntersect, l_bCenter)) {
            if (!l_bCenter)
              l_cIntersect -= 0.15f * (l_cVelocity.end - l_cVelocity.start);

            l_cVelocity.end = l_cIntersect;
            l_bCollide = true;
          }
          draw2dDebugLine(m_pDrv, l_cVelocity, m_fScale, l_bCollide ? irr::video::SColor(0xFF, 0xFF, 0, 0) : irr::video::SColor(0xFF, 0, 0xFF, 0), m_cOffset);          

          SPathLine2d *l_pSpecial = findNextSpecial(m_p2dPath);

          m_fVCalc = 0.0f;

          if (l_pSpecial != nullptr) {
            if (l_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump) {
              irr::core::line2df l_cSpecialLine = irr::core::line2df(l_pSpecial->m_cLines[1].start, l_pSpecial->m_cLines[2].start);
              irr::core::vector2df v;

              if (l_cSpecialLine.intersectWith(l_cVelocity, v)) {
                if (l_pSpecial->m_pParent->m_pParent->m_fBestVel != -1) {
                  m_fVCalc = l_pSpecial->m_pParent->m_pParent->m_fBestVel;
                }
                else if (l_pSpecial->m_pParent->m_pParent->m_fMaxVel != -1) {
                  m_fVCalc = l_pSpecial->m_pParent->m_pParent->m_fMaxVel * 0.98f;
                }
                else if (l_pSpecial->m_pParent->m_pParent->m_fMinVel != -1) {
                  m_fVCalc = l_pSpecial->m_pParent->m_pParent->m_fMinVel * 1.05f;
                }
                else l_pSpecial = nullptr;

                if (l_pSpecial != nullptr) {
                  irr::f32 l_fVel = m_aMarbles[m_iIndex].m_cVelocity.getLength();

                  if (m_fVCalc < l_fVel) {
                    a_bBrake = l_fVel - m_fVCalc > 5.0f;
                    a_iCtrlY = -127;
                  }
                  else {
                    a_bBrake = false;
                    a_iCtrlY = 127;
                  }
                }

                draw2dDebugLine(m_pDrv, l_cSpecialLine, m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);
              }
              else l_pSpecial = nullptr;
            }
            else l_pSpecial = nullptr;
          }

          if (l_pSpecial == nullptr) {
            irr::core::vector2df l_cClosest = m_p2dPath->m_cLines[0].getClosestPoint(irr::core::vector2df(0.0f));
            irr::f32 l_fFactor = (irr::core::line2df(m_p2dPath->m_cLines[0].end, l_cClosest).getLengthSQ() / m_p2dPath->m_cLines[0].getLengthSQ());
            l_fFactor = std::max(0.15f, l_fFactor);

            // Now we search for the possible ends of the path
            std::vector<SPathLine2d *> l_vEnds;
            findEnds(l_vEnds, m_p2dPath, 0.0f, l_fFactor);

            irr::f64 l_fAngle1 = l_cVelocity.getAngleWith(irr::core::line2df(irr::core::vector2df(0.0f, 0.0f), irr::core::vector2df(0.0f, 1.0f)));

            for (std::vector<SPathLine2d *>::iterator l_itEnd = l_vEnds.begin(); l_itEnd != l_vEnds.end(); l_itEnd++) {
              irr::core::line2df l_cBest = irr::core::line2df(l_cVelocity.end, l_cVelocity.end);

              if (getBestLine(l_cBest, *l_itEnd, nullptr)) {
                draw2dDebugLine(m_pDrv, l_cBest, m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0), m_cOffset);

                irr::f64 l_fAngle2 = l_cVelocity.getAngleWith(l_cBest);

                irr::f64 l_fFactor = std::max(1.0, l_fAngle2 / 65.0f);

                irr::f32 l_fVel1 = m_aMarbles[m_iIndex].m_cVelocity.getLength();
                irr::f32 l_fVel2 = (irr::f32)l_fFactor * l_cVelocity.getLength();

                if (1.1f * l_fVel1 > l_fVel2 && l_bCollide) {
                  a_iCtrlY = -127;
                  a_bBrake = l_fVel1 > 1.15f * l_fVel2;
                }
                else {
                  a_iCtrlY = 127;
                  a_bBrake = false;
                }
              }
            }
          }
        }

        while (l_vOptions.size() > 0) {
          SPathLine2d *p = *l_vOptions.begin();
          l_vOptions.erase(l_vOptions.begin());
          p->deleteAllChildren();
          delete p;
        }
      }

      if (m_pDebugRTT != nullptr) {
        m_pDrv->setRenderTarget(nullptr);
      } 

      return true;
    }

    /**
    * Fill a vector with the possible path options for the AI help
    * @param a_cPosition position of the marble
    * @param a_vAiPath the AI path to iterate for the good options
    * @param a_vResult the filled output vector
    */
    void CControllerAiHelp_V2::fillPathVector(const irr::core::vector3df& a_cPosition, const std::vector<SAiPathSection*>& a_vAiPath, std::vector<SAiPathSection*>& a_vResult) {
      for (std::vector<SAiPathSection*>::const_iterator l_itPath = a_vAiPath.begin(); l_itPath != a_vAiPath.end(); l_itPath++) {
        SAiPathSection *l_pPath = *l_itPath;

        irr::f32 l_fDistSq = (l_pPath->m_cLine3d.getClosestPoint(a_cPosition) - a_cPosition).getLengthSQ();

        if (l_fDistSq <= (l_pPath->m_fWidthSq + 2.0f) && l_pPath->m_cLine3d.getMiddle().Y < a_cPosition.Y && l_pPath->m_cLine3d.getMiddle().Y > a_cPosition.Y - 10.0f) {
          a_vResult.push_back(l_pPath);
        }
      }

      // printf("Found %i path sections\n", (int)a_vResult.size());
    }

    void CControllerAiHelp_V2::iterateOptions(SPathLine3d* a_pInput, std::vector<SPathLine3d *> &a_vOutput) {
      if (a_pInput->m_vNext.size() > 1) {
        a_vOutput.push_back(a_pInput);
      }

      for (std::vector<SPathLine3d*>::iterator l_itNext = a_pInput->m_vNext.begin(); l_itNext != a_pInput->m_vNext.end(); l_itNext++) {
        iterateOptions(*l_itNext, a_vOutput);
      }
    }

    /**
    * Get all path options of an input, i.e. all splits and such
    * @param a_pInput the current path section
    * @param a_vOutput [out] the resulting vector
    * @param a_vMarbles data of all marbles but the player's
    * @param a_vMarblePosVel the resulting marble data (position and velocity)
    */
    void CControllerAiHelp_V2::getAllPathOptions(
      SAiPathSection* a_pInput, 
      std::vector<SPathLine2d*> &a_vOutput, 
      std::vector<const data::SMarblePosition *> &a_vMarbles, 
      std::vector<std::tuple<int, irr::core::vector3df, irr::core::vector3df>> &a_vMarblePosVel
    ) {
      std::vector<SPathLine3d *> l_vSelections;

      iterateOptions(a_pInput->m_pAiPath, l_vSelections);
      
      irr::core::matrix4 l_cMatrix;
      l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_aMarbles[m_iIndex].m_cPosition + m_pCurrent->m_cNormal, m_aMarbles[m_iIndex].m_cPosition, m_aMarbles[m_iIndex].m_cDirection);

      if (l_vSelections.size() > 0) {
        for (std::vector<SPathLine3d*>::iterator l_itLine = l_vSelections.begin(); l_itLine != l_vSelections.end(); l_itLine++) {
          for (std::vector<SPathLine3d *>::iterator l_itNext = (*l_itLine)->m_vNext.begin(); l_itNext != (*l_itLine)->m_vNext.end(); l_itNext++) {
            std::map<int, SPathLine3d *> l_mSplitSelections;
            std::map<int, SPathLine3d *> l_mOldSelections;

            l_mOldSelections[(*(*l_itLine)->m_vNext.begin())->m_pParent->m_iTag] = *l_itNext;
            a_vOutput.push_back(a_pInput->m_pAiPath->transformTo2d(l_cMatrix, l_mSplitSelections, l_mOldSelections, a_vMarbles, a_vMarblePosVel, nullptr)->clone());
          }
        }
      }
      else {
        std::map<int, SPathLine3d *> l_mSplitSelections;
        std::map<int, SPathLine3d *> l_mOldSelections;

        a_vOutput.push_back(a_pInput->m_pAiPath->transformTo2d(l_cMatrix, l_mSplitSelections, l_mOldSelections, a_vMarbles, a_vMarblePosVel, nullptr)->clone());
      }
    }
  }
}