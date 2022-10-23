// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerAi_V2.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CAiPathNode.h>
#include <gui/CGameHUD.h>
#include <CGlobal.h>
#include <cmath>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace controller {
    /**
    * Draw a debug line with factor
    * @param a_pDrv the Irrlicht video driver
    * @param a_cLine the line to draw
    * @param a_fFactor the factor to scale the line
    * @param a_cColor the color of the line
    * @param a_cOffset offset of the line
    */
    void draw2dDebugLine(irr::video::IVideoDriver* a_pDrv, const irr::core::line2df& a_cLine, irr::f32 a_fFactor, const irr::video::SColor& a_cColor, const irr::core::vector2di& a_cOffset) {
      a_pDrv->draw2DLine(
        irr::core::vector2di(
          (irr::s32)(a_fFactor * a_cLine.start.X) + a_cOffset.X, 
          (irr::s32)(a_fFactor * a_cLine.start.Y) + a_cOffset.Y
        ),
        irr::core::vector2di(
          (irr::s32)(a_fFactor * a_cLine.end  .X) + a_cOffset.X,
          (irr::s32)(a_fFactor * a_cLine.end  .Y) + a_cOffset.Y
        ),
        a_cColor
      );
    }

    /**
    * Draws a debug rectangle
    * @param a_pDrv the Irrlicht video driver
    * @param a_cPos the center position
    * @param a_cColor the rectangle color
    * @param a_iSize the size of the rectangle
    * @param a_fScale the scale factor
    * @param a_cOffset the offset of the rectangle
    */
    void draw2dDebugRectangle(irr::video::IVideoDriver* a_pDrv, const irr::core::vector2df& a_cPos, const irr::video::SColor& a_cColor, int a_iSize, irr::f32 a_fScale, const irr::core::vector2di& a_cOffset) {
      irr::core::vector2di l_cUpperLeft = irr::core::vector2di((irr::s32)(a_fScale * a_cPos.X) + a_cOffset.X - a_iSize / 2, (irr::s32)(a_fScale * a_cPos.Y) + a_cOffset.Y - a_iSize / 2);
      a_pDrv->draw2DRectangleOutline(irr::core::recti(l_cUpperLeft, irr::core::dimension2du(a_iSize, a_iSize)), a_cColor);
    }

    void draw2dDebugText(irr::video::IVideoDriver *a_pDrv, const wchar_t* a_sText, irr::gui::IGUIFont* a_pFont, const irr::core::vector2df &a_cPosition, const irr::core::vector2di &a_cOffset, irr::f32 a_fFactor) {
      irr::core::dimension2du l_cDim = a_pFont->getDimension(a_sText);
      l_cDim.Width  = 6 * l_cDim.Width  / 5;
      l_cDim.Height = 6 * l_cDim.Height / 5;

      irr::core::recti l_cRect = irr::core::recti(
        irr::core::vector2di(
          (irr::s32)(a_fFactor * a_cPosition.X) + a_cOffset.X,
          (irr::s32)(a_fFactor * a_cPosition.Y) + a_cOffset.Y
        ),
        l_cDim
      );

      a_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 192, 192, 192), l_cRect);
      a_pFont->draw(a_sText, l_cRect, irr::video::SColor(0xFF, 0, 0, 0));
    }

    /**
    * Find all AI path nodes in the scene tree
    * @param a_pNode the node to check
    * @param a_vNodes [out] the vector of nodes that will be filled
    */
    void findAiPathNodes(const irr::scene::ISceneNode* a_pNode, std::vector<const scenenodes::CAiPathNode *>& a_vNodes) {
      if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_AiPathNodeId)
        a_vNodes.push_back(reinterpret_cast<const scenenodes::CAiPathNode *>(a_pNode));

      
      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itNodes = a_pNode->getChildren().begin(); l_itNodes != a_pNode->getChildren().end(); l_itNodes++)
        findAiPathNodes(*l_itNodes, a_vNodes);
    }

    /**
    * Select the closest AI path section to the position. Will be called
    * when the race is started, after respawn and stun
    * @param a_cPosition the position of the marble
    * @param a_bSelectStartupPath select a path marked as "startup"
    */
    CControllerAi_V2::SAiPathSection *CControllerAi_V2::selectClosest(const irr::core::vector3df& a_cPosition, std::vector<SAiPathSection *> &a_vOptions, bool a_bSelectStartupPath) {
      SAiPathSection *l_pCurrent = nullptr;

      irr::f32 l_fDistance = -1.0f;

      for (std::vector<SAiPathSection*>::iterator l_itPath = a_vOptions.begin(); l_itPath != a_vOptions.end(); l_itPath++) {
        irr::core::vector3df l_cOption = (*l_itPath)->m_cLine3d.getClosestPoint(a_cPosition);
        irr::f32 l_fOption = l_cOption.getDistanceFromSQ(a_cPosition);

        if ((l_fDistance == -1.0f || l_fOption < l_fDistance) && (!a_bSelectStartupPath || (*l_itPath)->m_bStartup) && (m_iLastCheckpoint == -1 || (*l_itPath)->m_iCheckpoint == m_iLastCheckpoint)) {
          l_pCurrent = *l_itPath;
          l_fDistance = l_fOption;
        }
      }

      return l_pCurrent;
    }

    /**
    * Select the new current AI path section
    * @param a_cPosition the marble's position
    * @param a_pCurrent the current section
    * @return the new current AI path section
    */
    CControllerAi_V2::SAiPathSection *CControllerAi_V2::selectCurrentSection(const irr::core::vector3df& a_cPosition, SAiPathSection* a_pCurrent) {
      irr::core::vector3df l_cClosest = a_pCurrent->m_cLine3d.getClosestPoint(a_cPosition);

      do {
        if (l_cClosest == a_pCurrent->m_cLine3d.end) {
          if (a_pCurrent->m_vNext.size() == 1)
            a_pCurrent = *a_pCurrent->m_vNext.begin();
          else
            a_pCurrent = selectClosest(m_cPosition, a_pCurrent->m_vNext, false);

          if (a_pCurrent != nullptr)
            l_cClosest = a_pCurrent->m_cLine3d.getClosestPoint(m_cPosition);
        }
      }
      while (a_pCurrent != nullptr && l_cClosest == a_pCurrent->m_cLine3d.end);

      return a_pCurrent;
    }

    /**
    * The constructor
    * @param a_iMarbleId the marble ID for this controller
    * @param a_sControls details about the skills of the controller
    */
    CControllerAi_V2::CControllerAi_V2(int a_iMarbleId, const std::string& a_sControls) : 
      m_iMarbleId      (a_iMarbleId), 
      m_iLastCheckpoint(-1), 
      m_iMyPosition    (0),
      m_fVCalc         (0.0f), 
      m_fScale         (1.0f),
      m_pCurrent       (nullptr), 
      m_pHUD           (nullptr), 
      m_fOldAngle      (0.0),
      m_eMode          (enMarbleMode::Default),
      m_pDrv           (CGlobal::getInstance()->getVideoDriver()),
      m_pDebugRTT      (nullptr),
      m_p2dPath        (nullptr)
    {
      if (m_iInstances == 0) {
        CGlobal *l_pGlobal = CGlobal::getInstance();

        std::vector<const scenenodes::CAiPathNode *> l_vAiNodes;

        findAiPathNodes(l_pGlobal->getSceneManager()->getRootSceneNode(), l_vAiNodes);

        printf("%i AI path nodes found.\n", (int)l_vAiNodes.size());

        int l_iIndex = 0;

        // Iterate over all found AI Path Nodes
        for (std::vector<const scenenodes::CAiPathNode*>::iterator l_itPath = l_vAiNodes.begin(); l_itPath != l_vAiNodes.end(); l_itPath++) {
          scenenodes::CCheckpointNode *l_pParent = nullptr;

          if ((*l_itPath)->getParent()->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_CheckpointNodeId)
            l_pParent = reinterpret_cast<scenenodes::CCheckpointNode *>((*l_itPath)->getParent());
          else
            printf("Parent is not a checkpoint!\n");

          // Iterate the AI Path sections stored in the AI path nodes
          for (std::vector<scenenodes::CAiPathNode::SAiPathSection*>::const_iterator l_itSection = (*l_itPath)->m_vSections.begin(); l_itSection != (*l_itPath)->m_vSections.end(); l_itSection++) {
            // Create one SAiPathSection for each link between the current section and it's successors
            for (std::vector<scenenodes::CAiPathNode::SAiPathSection*>::const_iterator l_itNext = (*l_itSection)->m_vNextSegments.begin(); l_itNext != (*l_itSection)->m_vNextSegments.end(); l_itNext++) {
              SAiPathSection *p = new SAiPathSection();

              irr::core::vector3df l_cStart = (*l_itSection)->m_cPosition;
              irr::core::vector3df l_cEnd   = (*l_itNext   )->m_cPosition;

              irr::core::vector3df l_cDir = ((*l_itNext)->m_cPosition - (*l_itSection)->m_cPosition).normalize();

              l_cStart -= 0.5f * l_cDir;
              l_cEnd   += 0.5f * l_cDir;

              p->m_cRealLine   = irr::core::line3df((*l_itSection)->m_cPosition, (*l_itNext)->m_cPosition);
              p->m_cLine3d     = irr::core::line3df(l_cStart, l_cEnd);
              p->m_iIndex      = ++l_iIndex;
              p->m_iCheckpoint = -1;
              p->m_bStartup    = (*l_itPath)->isStartupPath();

              if (l_pParent != nullptr) {
                p->m_iCheckpoint = l_pParent->getID();
              
                for (std::vector<int>::iterator l_itLinks = l_pParent->m_vLinks.begin(); l_itLinks != l_pParent->m_vLinks.end(); l_itLinks++) {
                  bool b = true;

                  for (std::vector<int>::iterator it = p->m_vCheckpoints.begin(); it != p->m_vCheckpoints.end(); it++)
                    if (*it == *l_itLinks)
                      b = false;

                  if (b)
                    p->m_vCheckpoints.push_back(*l_itLinks);
                }
              }
            

              irr::core::vector3df l_cEdgePoints[] = {
                (*l_itSection)->m_cPosition - (*l_itSection)->m_fWidth * (*l_itSection)->m_fFactor * (*l_itSection)->m_cSideVector,   // Start point 1
                (*l_itNext   )->m_cPosition - (*l_itNext   )->m_fWidth * (*l_itNext   )->m_fFactor * (*l_itNext   )->m_cSideVector,   // End point 1
                (*l_itSection)->m_cPosition + (*l_itSection)->m_fWidth * (*l_itSection)->m_fFactor * (*l_itSection)->m_cSideVector,   // Start point 2
                (*l_itNext   )->m_cPosition + (*l_itNext   )->m_fWidth * (*l_itNext   )->m_fFactor * (*l_itNext   )->m_cSideVector    // End point 2
              };

              irr::core::line3df l_cEdges[] = {
                irr::core::line3df(l_cEdgePoints[0], l_cEdgePoints[1]),   // Line start 1 --> end 1
                irr::core::line3df(l_cEdgePoints[2], l_cEdgePoints[3]),   // Line start 2 --> end 2
                irr::core::line3df(l_cEdgePoints[0], l_cEdgePoints[3]),   // Line start 1 --> end 2
                irr::core::line3df(l_cEdgePoints[2], l_cEdgePoints[1])    // Line start 2 --> end 1
              };

              irr::core::vector3df l_cMiddle[] = {
                l_cEdges[0].getMiddle(),
                l_cEdges[1].getMiddle(),
                l_cEdges[2].getMiddle(),
                l_cEdges[3].getMiddle()
              };

              if (l_cMiddle[0].getDistanceFromSQ(l_cMiddle[1]) > l_cMiddle[2].getDistanceFromSQ(l_cMiddle[3])) {
                p->m_cEdges[0] = l_cEdges[0];
                p->m_cEdges[1] = l_cEdges[1];
              }
              else {
                p->m_cEdges[0] = l_cEdges[2];
                p->m_cEdges[1] = l_cEdges[3];
              }
            
              p->m_cNormal = (*l_itSection)->m_cNormal;

              m_vAiPath.push_back(p);
            }
          }
        }

        // Now that we have a filled vector of the path sections we need to link them
        for (std::vector<SAiPathSection*>::iterator l_itThis = m_vAiPath.begin(); l_itThis != m_vAiPath.end(); l_itThis++) {
          irr::core::vector3df l_cThis = (*l_itThis)->m_cRealLine.end;

          for (std::vector<SAiPathSection*>::iterator l_itNext = m_vAiPath.begin(); l_itNext != m_vAiPath.end(); l_itNext++) {
            irr::core::vector3df l_cNext = (*l_itNext)->m_cRealLine.start;

            // Now we check if the end of this line matches the start of the next line.
            // We use a certain threshold as the serialization is not 100% accurate
            if (abs(l_cThis.X - l_cNext.X) < 0.01f && abs(l_cThis.Y - l_cNext.Y) < 0.01f && abs(l_cThis.Z - l_cNext.Z) < 0.01f) {
              bool l_bAdd = (*l_itThis)->m_iCheckpoint == (*l_itNext)->m_iCheckpoint;

              if (!l_bAdd) {
                for (std::vector<int>::iterator it = (*l_itThis)->m_vCheckpoints.begin(); it != (*l_itThis)->m_vCheckpoints.end(); it++) {
                  if ((*it) == (*l_itNext)->m_iCheckpoint) {
                    l_bAdd = true;
                    break;
                  }
                }
              }

              if (l_bAdd)
                (*l_itThis)->m_vNext.push_back(*l_itNext);
            }
          }
        }

        printf("%i AI path sections found.\n", (int)m_vAiPath.size());

        // Now we calculate the next 500+ meters for all AI path sections
        for (std::vector<SAiPathSection*>::iterator l_itThis = m_vAiPath.begin(); l_itThis != m_vAiPath.end(); l_itThis++) {
          (*l_itThis)->fillLineVectors();
        }

        int l_iZeroLinks = 0;

        // For debugging: show how many links each section has
        for (std::vector<SAiPathSection*>::iterator l_itThis = m_vAiPath.begin(); l_itThis != m_vAiPath.end(); l_itThis++) {
          if ((*l_itThis)->m_vNext.size() == 0)
            l_iZeroLinks++;
          else {
            std::vector<SAiPathSection *>::iterator l_itNext = (*l_itThis)->m_vNext.begin();

            irr::core::plane3df  l_cPlane = irr::core::plane3df((*l_itThis)->m_cLine3d.start, (*l_itThis)->m_cNormal);
            irr::core::vector3df l_cOther;
            irr::core::vector3df l_cThis;

            l_cPlane.getIntersectionWithLine((*l_itThis)->m_cLine3d.end  , (*l_itThis)->m_cNormal, l_cThis );
            l_cPlane.getIntersectionWithLine((*l_itNext)->m_cLine3d.start, (*l_itNext)->m_cNormal, l_cOther);

            irr::core::vector3df l_cV1 = (*l_itThis)->m_cLine3d.end;
            irr::core::vector3df l_cV2 = (*l_itThis)->m_cLine3d.end + ((*l_itThis)->m_cLine3d.start - (*l_itThis)->m_cLine3d.end);
            irr::core::vector3df l_cV3 = (*l_itThis)->m_cLine3d.end + (l_cOther - (*l_itThis)->m_cLine3d.end);
          }
        }

        printf("%i unlinked sections found.\n", l_iZeroLinks);
        printf("Ready.");
      }
      m_iInstances++;
    }

    CControllerAi_V2::~CControllerAi_V2() {
      m_iInstances--;

      if (m_iInstances == 0) {
        printf("Deleting AI data.\n");
        while (m_vAiPath.size() > 0) {
          SAiPathSection *p = *m_vAiPath.begin();
          m_vAiPath.erase(m_vAiPath.begin());
          delete p;
        }
      }

      if (m_pHUD != nullptr) {
        m_pHUD->setAiController(nullptr);
      }

      if (m_pDebugRTT != nullptr) {
        m_pDebugRTT = nullptr;
      }
    }

    /**
    * Update the controller with the Irrlicht event
    * @param a_cEvent the Irrlicht event
    */
    void CControllerAi_V2::update(const irr::SEvent& a_cEvent) {
    }

    /**
    * Notify the controller about marble movement
    * @param a_iMarbleId ID of the moving marble
    * @param a_cNewPos the new position of the marble
    * @param a_cVelocity the velocity of the marble
    * @param a_cCameraPos the position of the camera
    * @param a_cCameraUp the up-vector of the camera
    */
    void CControllerAi_V2::onMarbleMoved(int a_iMarbleId, const irr::core::vector3df& a_cNewPos, const irr::core::vector3df& a_cVelocity, const irr::core::vector3df& a_cCameraPos, const irr::core::vector3df& a_cCameraUp) {
      int l_iIndex = a_iMarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16) {
        m_aMarbles[l_iIndex].m_iMarbleId = a_iMarbleId;
        m_aMarbles[l_iIndex].m_cPosition = a_cNewPos;
        m_aMarbles[l_iIndex].m_cVelocity = a_cVelocity;
      }

      if (a_iMarbleId == m_iMarbleId) {
        m_cPosition  = a_cNewPos;
        m_cVelocity  = a_cVelocity;
        m_cDirection = a_cCameraPos - m_cPosition;
        m_cContact   = m_cPosition - a_cCameraUp;
        m_cCameraUp  = a_cCameraUp;

        if (m_pCurrent == nullptr) {
          m_pCurrent = selectClosest(m_cPosition, m_vAiPath, m_iLastCheckpoint == -1);

          if (m_pCurrent == nullptr && m_iLastCheckpoint == -1)
            m_pCurrent = selectClosest(m_cPosition, m_vAiPath, false);

          // if (m_pCurrent != nullptr)
          //   printf("AI Path section selected: %.2f, %.2f, %.2f\n", m_pCurrent->m_cLine3d.start.X, m_pCurrent->m_cLine3d.start.Y, m_pCurrent->m_cLine3d.start.Z);
        }

        if (m_pCurrent != nullptr) {
          m_pCurrent = selectCurrentSection(m_cPosition, m_pCurrent);

          if (m_pCurrent != nullptr && m_pCurrent->m_pAiPath != nullptr) {
            irr::core::matrix4 l_cMatrix;
            l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_cPosition + m_pCurrent->m_cNormal, m_cPosition, m_cDirection);

            m_p2dPath = m_pCurrent->m_pAiPath->transformTo2d(l_cMatrix, m_mSplitSelections);
            irr::core::vector3df l_vDummy = m_cPosition + m_cVelocity;
            l_cMatrix.transformVect(l_vDummy);

            m_cVelocity2d.X = l_vDummy.X;
            m_cVelocity2d.Y = l_vDummy.Y;

            if (m_p2dPath->m_cLines[1].getPointOrientation(irr::core::vector2df()) > 0 ==  m_p2dPath->m_cLines[2].getPointOrientation(irr::core::vector2df()) > 0)
              switchMarbleMode(enMarbleMode::OffTrack, 1);
            else {
              if (m_eMode == enMarbleMode::OffTrack)
                switchMarbleMode(enMarbleMode::Default, 1);
              else {
                if (m_iMyPosition > 1) {
                  if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead > 0) {
                    if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead < 360)
                      switchMarbleMode(enMarbleMode::TimeAttack, 2);
                    else if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead < 480)
                      switchMarbleMode(enMarbleMode::Default, 2);
                    else
                      switchMarbleMode(enMarbleMode::Cruise, 2);
                  }
                }
                else if (m_iMyPosition == 1) {
                  if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead > 0) {
                    if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead < 240)
                      switchMarbleMode(enMarbleMode::TimeAttack, 2);
                    else if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead < 360)
                      switchMarbleMode(enMarbleMode::Default, 2);
                    else
                      switchMarbleMode(enMarbleMode::Cruise, 2);
                  }
                }
              }
            }
          }        
        }
      }
    }

    /**
    * Notify the controller about a marble respawn
    * @param a_iMarbleId the respawning marble
    */
    void CControllerAi_V2::onMarbleRespawn(int a_iMarbleId) {
      if (a_iMarbleId == m_iMarbleId) {
        m_pCurrent = nullptr;
        switchMarbleMode(enMarbleMode::OffTrack, 0);
      }
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CControllerAi_V2::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (a_iMarbleId == m_iMarbleId) {
        m_iLastCheckpoint = a_iCheckpoint;
      }
    }

    /**
    * Switch this AI marble to another mode
    * @param a_eMode the new mode
    */
    void CControllerAi_V2::switchMarbleMode(enMarbleMode a_eMode, int a_iCall) {
      if (a_eMode != m_eMode) {
        m_eMode = a_eMode;

        switch (a_eMode) {
          case enMarbleMode::OffTrack:
            // printf("New Mode: Off Track (%i)\n", a_iCall);
            break;

          case enMarbleMode::Default:
            // printf("New Mode: Default (%i)\n", a_iCall);
            break;

          case enMarbleMode::Cruise:
            // printf("New Mode: Cruise (%i)\n", a_iCall);
            break;

          case enMarbleMode::TimeAttack:
            // printf("New Mode: Time Attack (%i)\n", a_iCall);
            break;
        }
      }
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
    * @param a_DeficitLeader Deficit of the marble on the leader in steps
    */
    void CControllerAi_V2::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
      if (a_Position > 0 && a_Position <= 16) {
        m_aRacePositions[a_Position - 1].m_iMarble        = a_MarbleId;
        m_aRacePositions[a_Position - 1].m_iPosition      = a_Position;
        m_aRacePositions[a_Position - 1].m_iDeficitAhead  = a_DeficitAhead;
        m_aRacePositions[a_Position - 1].m_iDeficitLeader = a_DeficitLeader;

        if (a_MarbleId == m_iMarbleId)
          m_iMyPosition = a_Position;
      }
    }

    /**
    * Get the control values for the marble
    * @param a_iMarbleId [out] ID of the marble this controller controls
    * @param a_iCtrlX [out] the steering value 
    * @param a_iCtrlY [out] the throttle value
    * @param a_bBrake [out] is the brake active?
    * @param a_bRearView [out] does the marble look to the back?
    * @param a_bRespawn [out] does the marble want a manual respawn?
    */
    bool CControllerAi_V2::getControlMessage(irr::s32& a_iMarbleId, irr::s8& a_iCtrlX, irr::s8& a_iCtrlY, bool& a_bBrake, bool& a_bRearView, bool& a_bRespawn) {
      a_iMarbleId = m_iMarbleId;
      a_iCtrlX    = 0;
      a_iCtrlY    = 0;
      a_bBrake    = false;
      a_bRearView = false;
      a_bRespawn  = false;

      irr::s16 l_iCtrlX = 0;

      if (m_pDebugRTT != nullptr) {
        m_pDrv->setRenderTarget(m_pDebugRTT, true, false);
      }

      if (m_pCurrent != nullptr) {
        irr::core::line2df l_cVelocityLine = irr::core::line2df(irr::core::vector2df(), m_cVelocity2d);

        if (m_p2dPath != nullptr) {
          if (m_pDebugRTT != nullptr)
            m_p2dPath->debugDraw(m_pDrv, m_cOffset, 2.0f);

          irr::core::vector2df l_cClosest = m_p2dPath->m_cLines[0].getClosestPoint(irr::core::vector2df(0.0f));

          irr::f32 l_fFactor = (irr::core::line2df(m_p2dPath->m_cLines[0].end, l_cClosest).getLengthSQ() / m_p2dPath->m_cLines[0].getLengthSQ());

          std::vector<SPathLine2d *> l_vEnds;
          findEnds(l_vEnds, m_p2dPath, 0.0f, l_fFactor);

          irr::video::SColor l_cColors[] = {
            irr::video::SColor(0xFF, 0xFF, 0, 0),
            irr::video::SColor(0xFF, 0xFF, 0xFF, 0),
            irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF)
          };

          for (std::vector<SPathLine2d*>::iterator l_itEnd = l_vEnds.begin(); l_itEnd != l_vEnds.end(); l_itEnd++) {
            irr::core::line2df l_cLine  = irr::core::line2df(irr::core::vector2df(), irr::core::vector2df());
            irr::core::line2df l_cOther;

            int l_iLines = 0;

            switch (m_eMode) {
              case enMarbleMode::OffTrack:
                l_iLines = getControlLines_Offtrack(l_cLine, l_cOther, nullptr);
                break;
                
              case enMarbleMode::Default:
                l_iLines = getControlLines_Default(l_cLine, l_cOther, *l_itEnd);
                break;

              case enMarbleMode::Cruise:
                l_iLines = getControlLines_Cruise(l_cLine, l_cOther, *l_itEnd);
                break;

              case enMarbleMode::TimeAttack:
                l_iLines = getControlLines_TimeAttack(l_cLine, l_cOther, *l_itEnd);
                break;
            }

            if (m_pDebugRTT != nullptr)
              draw2dDebugLine(m_pDrv, l_cLine, 2.0f, irr::video::SColor(255, 255, 0, 0), m_cOffset);

            if (l_iLines > 1 && m_pDebugRTT != nullptr)
              draw2dDebugLine(m_pDrv, l_cOther, 2.0f, irr::video::SColor(0xFF, 0xFF, 0xFF, 0), m_cOffset);

            irr::f32 l_fVel = m_cVelocity2d.getLength();
            if (l_fVel > 10.0f) {
              irr::f32 l_fCtrlLen = l_cLine.getLength();

              irr::f64 l_fAngle1 = l_cVelocityLine.getAngleWith(l_cLine);
              irr::f64 l_fFactor = 1.0 - ((l_fAngle1) / 90.0) + 0.15;

              irr::core::vector2df l_cPoint1 = (l_cLine.end - l_cLine.start).normalize() * l_fVel * (irr::f32)l_fFactor;

              irr::f64 l_fAngle2  = l_cLine.getAngleWith(l_cOther);
              irr::f64 l_fFactor2 = 1.0 - (l_fAngle2 / 90.0) + 0.15;

              if (l_cLine.getClosestPoint(l_cPoint1) == l_cLine.end) {
                l_cPoint1 = l_cOther.start + (l_cOther.end - l_cOther.start).normalize() * (irr::f32)l_fFactor2 * (l_fVel - l_fCtrlLen);
              }

              irr::f64 l_fAngle3 = l_cVelocityLine.getAngleWith(irr::core::line2df(irr::core::vector2df(), l_cPoint1));

              l_fFactor = l_fAngle3 / 90.0;

              irr::f64 l_fSpeedFactor = (1.0 - l_fFactor);

              m_fVCalc = (irr::f32)(l_fSpeedFactor * l_fSpeedFactor * 75.0);

              if (l_fSpeedFactor > 0.85) {
                m_fVCalc += (irr::f32)(80.0 * (l_fSpeedFactor / 0.85));
              }

              irr::f64 l_fSpeedFact2 = 1.0 - l_fFactor2;
              irr::f64 l_fSpeed2 = std::max(15.0, l_fSpeedFact2 * l_fSpeedFact2 * 75.0f);

              if (l_fSpeedFact2 > 0.85) {
                l_fSpeed2 += 80.0 * (l_fSpeedFact2 / 0.85);
              }
              
              if (l_fVel > 1.8 * l_cLine.getLength()) {
                m_fVCalc = (irr::f32)l_fSpeed2;
              }

              if (m_fVCalc > l_fVel)
                a_iCtrlY = 127;
              else {
                a_iCtrlY = -127;
                a_bBrake = std::abs(l_fVel - m_fVCalc) > 5.0f;
              }

              irr::core::vector2df l_cPos = irr::core::vector2df();

              draw2dDebugRectangle(m_pDrv, l_cOther.end, irr::video::SColor(0xFF, 0xFF, 0, 0), 20, 2.0f, m_cOffset);

              l_iCtrlX = (std::abs(l_fAngle3) > 2.5 || (l_cOther.end.X > 0.0f != m_cVelocity2d.X > 0.0f)) ? l_fFactor > 0.05 ? 127 : (irr::s8)(127.0 * (l_fFactor + 0.25)) : 0;

              if (l_cPoint1.X < 0.0f)
                l_iCtrlX = -l_iCtrlX;

              if (m_fOldAngle != 0.0) {
                irr::f64 l_fTurnSpeed = m_fOldAngle - l_fAngle3;
                if (l_fAngle3 - 3.0 * l_fTurnSpeed < 0.0)
                  l_iCtrlX = 0;
              }

              // Maybe re-add this depending on the class or time mode
              /* for (int i = 1; i < 3; i++) {
                irr::core::vector2df p = m_p2dPath->m_cLines[i].getClosestPoint(l_cPos);
                irr::f32 f = p.getLengthSQ();
                if (f < 4.0f) {
                  l_iCtrlX += (irr::s8)(-100.0 * f / 2.0f);
                }
              } */

              a_iCtrlX = (irr::s8)(std::max((irr::s16)-127, std::min((irr::s16)127, l_iCtrlX)));

              if (l_cPoint1.Y > 0.0)
                a_iCtrlY = -127;

              if (m_pDebugRTT != nullptr)
                draw2dDebugRectangle(m_pDrv, l_cPoint1, irr::video::SColor(0xFF, 0xFF, 0, 0xFF), 20, 2.0f, m_cOffset);

              m_fOldAngle = l_fAngle3;
            }
            else a_iCtrlY = 127;
          }
        }

        if (m_pDebugRTT != nullptr) {
          draw2dDebugLine(m_pDrv, l_cVelocityLine, 2.0f, irr::video::SColor(0xFF, 0, 0xFF, 0), m_cOffset);
          draw2dDebugRectangle(m_pDrv, irr::core::vector2df(0.0f), irr::video::SColor(0xFF, 0, 0, 0xFF), 30, 2.0f, m_cOffset);

          for (int i = 0; i < 16; i++) {
            if (m_aMarbles[i].m_iMarbleId != -1 && m_aMarbles[i].m_iMarbleId != m_iMarbleId) {
              SPathLine2d *l_pLine  = m_p2dPath;
              SPathLine2d *l_pOther = nullptr;

              irr::core::vector3df l_cPoint;

              irr::f32 l_fDist = 0.0f;

              while (l_pLine != nullptr) {
                irr::core::vector3df l_cOption = l_pLine->m_cOriginal.getClosestPoint(m_aMarbles[i].m_cPosition);
                irr::f32 l_fNewDist = l_cOption.getDistanceFromSQ(m_aMarbles[i].m_cPosition);

                if (l_fDist == 0.0f || l_fNewDist < l_fDist) {
                  l_fDist  = l_fNewDist;
                  l_cPoint = l_cOption;
                  l_pOther = l_pLine;
                }

                if (l_pLine->m_vNext.size() == 0)
                  l_pLine = nullptr;
                else {
                  l_pLine = *(l_pLine->m_vNext).begin();
                }
              }

              if (l_pOther != nullptr && l_fDist < l_pOther->m_fWidth * l_pOther->m_fWidth + 0.5f) {
                irr::core::vector3df l_cPos = m_aMarbles[i].m_cPosition;
                irr::core::vector3df l_cVel = m_aMarbles[i].m_cPosition + m_aMarbles[i].m_cVelocity;

                l_pLine = l_pOther;
                while (l_pLine != nullptr) {
                  l_pLine->m_cPlane.getIntersectionWithLine(l_cPos, l_pLine->m_cNormal, l_cPos);
                  l_pLine->m_cPlane.getIntersectionWithLine(l_cVel, l_pLine->m_cNormal, l_cVel);
                  l_pLine = l_pLine->m_pPrevious;
                }

                l_pOther->m_cMatrix.transformVect(l_cPos);
                l_pOther->m_cMatrix.transformVect(l_cVel);

                irr::core::vector2df l_cPos2d = irr::core::vector2df(l_cPos.X, l_cPos.Y);
                irr::core::vector2df l_cVel2d = irr::core::vector2df(l_cVel.X, l_cVel.Y);

                bool l_bIncoming = l_cPos2d.Y < 0.0f && l_cVelocityLine.start.getDistanceFromSQ(l_cPos2d) > l_cVelocityLine.start.getDistanceFromSQ(l_cVel2d);

                irr::video::SColor l_cColor = l_bIncoming ? irr::video::SColor(0xFF, 0xFF, 0, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);

                draw2dDebugRectangle(m_pDrv, l_cPos2d, l_cColor, 20, m_fScale, m_cOffset);
                draw2dDebugLine(m_pDrv, irr::core::line2df(l_cPos2d, l_cVel2d), m_fScale, l_cColor, m_cOffset);
              }
            }
          }
        }
      }

      if (m_pDebugRTT != nullptr) {
        m_pDrv->setRenderTarget(nullptr);
      }

      return true;
    }

    /**
    * Set the controller to debug mode
    * @param a_bDebug the new debug flag
    */
    void CControllerAi_V2::setDebug(bool a_bDebug) {
      if (a_bDebug) {
        m_cRttSize = m_pDrv->getScreenSize();

        m_cRttSize.Width  /= 3;
        m_cRttSize.Height /= 3;

        m_fScale = ((irr::f32)m_cRttSize.Height) / 1000.0f;
        m_fScale = 1.0f / m_fScale;

        m_pDebugRTT = m_pDrv->getTexture("ai_debug_rtt");

        if (m_pDebugRTT == nullptr)
          m_pDebugRTT = m_pDrv->addRenderTargetTexture(m_cRttSize, "ai_debug_rtt");

        m_cOffset.X = m_cRttSize.Width / 2;
        m_cOffset.Y = m_cRttSize.Height;
      }
    }

    /**
    * Tell the controller about it's HUD
    * @param a_pHUD the HUD
    */
    void CControllerAi_V2::setHUD(gui::CGameHUD* a_pHUD) {
      if (m_pHUD != a_pHUD) {
        m_pHUD = a_pHUD;
        if (m_pHUD != nullptr)
          m_pHUD->setAiController(this);
      }
    }

    /**
    * Get the render target texture for debugging
    * @return the render target texture for debugging
    */
    irr::video::ITexture* CControllerAi_V2::getDebugTexture() {
      return m_pDebugRTT;
    }

    /**
    * Get the best, i.e. not colliding with a border line, line in the path starting
    * from the startpoint of the "a_cLine" parameter
    * @param a_cLine [out] the best line, start is used as input
    * @param a_pEnd the 2d path lines to search
    */
    bool CControllerAi_V2::getBestLine(irr::core::line2df& a_cLine, SPathLine2d* a_pEnd, SPathLine2d **a_pCollide) {
      while (a_pEnd != nullptr) {
        bool l_bReturn = true;
        a_cLine.end = a_pEnd->m_cLines[0].end;

        SPathLine2d *l_pOther = a_pEnd;
        while (l_pOther != nullptr) {
          if (l_pOther->doesLineIntersectBorder(a_cLine)) {
            l_bReturn = false;
            break;
          }

          l_pOther = l_pOther->m_pPrevious;
        }
        a_pEnd = a_pEnd->m_pPrevious;

        if (l_bReturn) {
          if (a_pCollide != nullptr)
            *a_pCollide = a_pEnd;

          return true;
        }
      }
      return false;
    }

    /**
    * Get the 2d lines for calculating the marble controls for Default mode
    * @param a_cLineOne [out] the first control line
    * @param a_cLineTwo [out] the second control line
    * @param a_pPath the AI path data for calculating the two lines
    * @return the number of calculated lines (1 or 2)
    */
    int CControllerAi_V2::getControlLines_Default(irr::core::line2df& a_cLineOne, irr::core::line2df& a_cLineTwo, SPathLine2d* a_pPath) {
      int l_iRet = 0;

      if (getBestLine(a_cLineOne, a_pPath, nullptr))
        l_iRet++;

      a_cLineTwo = irr::core::line2df(a_cLineOne.end, a_cLineOne.end);
      if (getBestLine(a_cLineTwo, a_pPath, nullptr))
        l_iRet++;
      
      return l_iRet;
    }

    /**
    * Get the 2d lines for calculating the marble controls for Cruise mode
    * @param a_cLineOne [out] the first control line
    * @param a_cLineTwo [out] the second control line
    * @param a_pPath the AI path data for calculating the two lines
    * @return the number of calculated lines (1 or 2)
    */
    int CControllerAi_V2::getControlLines_Cruise(irr::core::line2df& a_cLineOne, irr::core::line2df& a_cLineTwo, SPathLine2d* a_pPath) {
      int l_iRet = 0;

      if (getBestLine(a_cLineOne, a_pPath, nullptr))
        l_iRet++;


      SPathLine2d *l_pPath = nullptr;

      a_cLineTwo = irr::core::line2df(a_cLineOne.end, a_cLineOne.end);
      if (getBestLine(a_cLineTwo, a_pPath, &l_pPath)) {
        l_iRet++;

        if (l_pPath != nullptr) {
          irr::core::vector2df l_cDirection = (l_pPath->m_cLines[0].end - l_pPath->m_cLines[0].start).normalize();

          a_cLineOne.end -= (l_pPath->m_fWidth / 2.0f) * (a_cLineTwo.end - a_cLineTwo.start).normalize();
          a_cLineTwo.start = a_cLineOne.end;
        }
      }

      return l_iRet;
    }

    /**
    * Get the 2d lines for calculating the marble controls for Cruise mode
    * @param a_cLineOne [out] the first control line
    * @param a_cLineTwo [out] the second control line
    * @param a_pPath the AI path data for calculating the two lines
    * @return the number of calculated lines (1 or 2)
    */
    int CControllerAi_V2::getControlLines_TimeAttack(irr::core::line2df& a_cLineOne, irr::core::line2df& a_cLineTwo, SPathLine2d* a_pPath) {
      int l_iRet = 0;

      if (getBestLine(a_cLineOne, a_pPath, nullptr))
        l_iRet++;

      a_cLineOne.end += 15.0f * (a_cLineOne.end - a_cLineOne.start).normalize();

      a_cLineTwo = irr::core::line2df(a_cLineOne.end, a_cLineOne.end);
      if (getBestLine(a_cLineTwo, a_pPath, nullptr)) {
        l_iRet++;
      }
      
      return l_iRet;
    }

    /**
    * Get the 2d lines for calculating the marble controls for Cruise mode
    * @param a_cLineOne [out] the first control line
    * @param a_cLineTwo [out] the second control line
    * @param a_pPath the AI path data for calculating the two lines
    * @return the number of calculated lines (1 or 2)
    */
    int CControllerAi_V2::getControlLines_Offtrack(irr::core::line2df& a_cLineOne, irr::core::line2df& a_cLineTwo, SPathLine2d* a_pPath) {
      int l_iRet = 0;

      a_cLineOne.end =  m_p2dPath->m_cLines[0].getClosestPoint(irr::core::vector2df());

      a_cLineTwo = irr::core::line2df(a_cLineOne.end, a_cLineOne.end);
      if (getBestLine(a_cLineTwo, a_pPath, nullptr)) {
        l_iRet++;
      }
      
      return l_iRet;
    }

    /**
    * Find all ends of a path and store them in the given vector. If a path splits this will produce two
    * ends, not matter if the two paths are re-united within the look-ahead distance
    * @param a_vEnds [out] the vector to store the ends in
    * @param a_pLine the line to check
    * @param a_fLength the length that has already been processed
    * @param a_fFactor factor to multiply the length of the line (a_pLine) with. Necessary for the first call as a portion of the line 
    * is already done, all later calls just get "1.0"
    */
    void CControllerAi_V2::findEnds(std::vector<SPathLine2d *>& a_vEnds, SPathLine2d* a_pLine, irr::f32 a_fLength, irr::f32 a_fFactor) {
      a_fLength += a_pLine->m_cLines[0].getLength() * a_fFactor;

      if (a_pLine->m_vNext.size() == 0) {
        a_vEnds.push_back(a_pLine);
      }
      else {
        if (a_fLength < 300.0f) {
          for (std::vector<SPathLine2d*>::iterator it = a_pLine->m_vNext.begin(); it != a_pLine->m_vNext.end(); it++) {
            findEnds(a_vEnds, *it, a_fLength, 1.0f);
          }
        }
        else a_vEnds.push_back(a_pLine);
      }
    }

    /**
    * Check whether or not the two passed lines (for edges) may overlap
    * @param a_cLine1 the first line
    * @param a_cLine2 the second line
    * @return true if the lines may overlap, false otherwise
    */
    bool CControllerAi_V2::SAiPathSection::doLinesOverlap(const irr::core::line3df& a_cLine1, const irr::core::line3df& a_cLine2) {
      // we create two line pairs and check the first line agains line two and three
      // so that we can make sure that the lines will not cross each other
      irr::core::line3df l1 = irr::core::line3df(a_cLine1.start, a_cLine1.end);
      irr::core::line3df l2 = irr::core::line3df(a_cLine2.start, a_cLine2.end);
      irr::core::line3df l3 = irr::core::line3df(a_cLine1.start, a_cLine2.end);
      irr::core::line3df l4 = irr::core::line3df(a_cLine2.start, a_cLine1.end);

      irr::core::vector3df m1 = l1.getMiddle();
      irr::core::vector3df m2 = l2.getMiddle();
      irr::core::vector3df m3 = l3.getMiddle();
      irr::core::vector3df m4 = l4.getMiddle();

      return m1.getDistanceFrom(m2) > m1.getDistanceFrom(m3);
    }

    /**
    * Prepare the 3d line data for storing them in the next and border line vectors. To do
    * so we need to transform the nodes to lie in a plane defined by the position and normal
    * of this section. Starting from the end (when a distance of 500 meters is exceeded) we
    * need to traverse backwards and transform the points of the next line to lie in the plane
    * of the previous section. Complicated but it somehow works
    * @param a_fLength the length that has alreaddy been exceeded
    * @param a_pPrevious the previous line item
    */
    CControllerAi_V2::SPathLine3d *CControllerAi_V2::SAiPathSection::prepareTransformedData(irr::f32 a_fLength, SPathLine3d *a_pPrevious) {
      SPathLine3d *p = a_pPrevious;

      while (p != nullptr) {
        if (m_iIndex == p->m_iSectionIndex) {
          return nullptr;
        }
        p = p->m_pPrevious;
      }

      SPathLine3d *l_pThis = new SPathLine3d(m_cLine3d, m_cEdges[0], m_cEdges[1]);
      
      if (a_pPrevious != nullptr)
        a_pPrevious->m_vNext.push_back(l_pThis);

      l_pThis->m_pPrevious     = a_pPrevious;
      l_pThis->m_iSectionIndex = m_iIndex;
      l_pThis->m_pParent       = this;

      l_pThis->m_cPathLine.m_cOriginal = m_cLine3d;

      // Only process if 750 meters are not yet exceeded
      if (a_fLength <= 750.0f) {
        // Add the length of this segment to the processed length
        a_fLength += m_cLine3d.getLength();

        // Now let all succeeding sections process their data with this section's plane
        for (std::vector<SAiPathSection*>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++) {
          bool l_bAdd = true;

          if ((*l_itNext)->m_iCheckpoint != m_iCheckpoint) {
            l_bAdd = false;
            for (std::vector<int>::iterator it = m_vCheckpoints.begin(); it != m_vCheckpoints.end(); it++) {
              if (*it == (*l_itNext)->m_iCheckpoint) {
                l_bAdd = true;
                break;
              }
            }
          }

          if (l_bAdd) {
            SPathLine3d *l_pNew = (*l_itNext)->prepareTransformedData(a_fLength, l_pThis);
            if (l_pNew == nullptr)
              break;
          }
        }

        m_cPlane = irr::core::plane3df(m_cLine3d.start, m_cNormal);
        l_pThis->m_cPathLine.m_cPlane  = m_cPlane;
        l_pThis->m_cPathLine.m_cNormal = m_cNormal;

        // Now transform all points of the lines in the vector to lie in this section's plane
        for (std::vector<SPathLine3d *>::iterator it = l_pThis->m_vNext.begin(); it != l_pThis->m_vNext.end(); it++)
          (*it)->transformLinesToPlane(m_cPlane, m_cNormal);
      }

      return l_pThis;
    }

    /**
    * Fill the vectors of the points for the next 500+ meters
    * with Irrlicht vectors transformed to the section plane
    */
    void CControllerAi_V2::SAiPathSection::fillLineVectors() {
      m_pAiPath = prepareTransformedData(0.0f, nullptr);
    }

    bool CControllerAi_V2::SPathLine2d::doesLineIntersectBorder(const irr::core::line2df& a_cLine) {
      irr::core::vector2df v;
      for (int i = 1; i < 3; i++)
        if (m_cLines[i].intersectWith(a_cLine, v))
          return true;

      return false;
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d() : m_fWidth(0.0f), m_pParent(nullptr), m_pPrevious(nullptr) {
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(const SPathLine2d& a_cOther) : m_fWidth(a_cOther.m_fWidth), m_pParent(a_cOther.m_pParent), m_pPrevious(nullptr) {
      for (int i = 0; i < 3; i++)
        m_cLines[i] = a_cOther.m_cLines[i];
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(irr::core::line2df& a_cLine1, irr::core::line2df& a_cLine2, irr::core::line2df& a_cLine3) : m_fWidth((a_cLine2.start - a_cLine3.start).getLength()), m_pParent(nullptr), m_pPrevious(nullptr) {
      m_cLines[0] = a_cLine1;
      m_cLines[1] = a_cLine2;
      m_cLines[2] = a_cLine3;
    }

    CControllerAi_V2::SPathLine2d::~SPathLine2d() {
      m_vNext.clear();
    }

    /**
    * Debug draw this 2d line instance
    * @param a_pDrv the Irrlicht video driver
    * @param a_cOffset the offset to draw
    * @param a_fScale the scale to use for drawing
    */
    void CControllerAi_V2::SPathLine2d::debugDraw(irr::video::IVideoDriver* a_pDrv, const irr::core::vector2di& a_cOffset, irr::f32 a_fScale) {
      for (int i = 0; i < 3; i++)
        a_pDrv->draw2DLine(
          irr::core::vector2di((irr::s32)(a_fScale * m_cLines[i].start.X) + a_cOffset.X, (irr::s32)(a_fScale * m_cLines[i].start.Y) + a_cOffset.Y),
          irr::core::vector2di((irr::s32)(a_fScale * m_cLines[i].end  .X) + a_cOffset.X, (irr::s32)(a_fScale * m_cLines[i].end  .Y) + a_cOffset.Y),
          i == 0 ? irr::video::SColor(0xFF, 128, 128, 128) : irr::video::SColor(0xFF, 0, 0, 0xFF)
        );

      if (m_vNext.size() > 1)
        draw2dDebugRectangle(a_pDrv, m_cLines[0].end, irr::video::SColor(0xFF, 0xFF, 0xFF, 0), 20, 2.0f, a_cOffset);

      for (std::vector<SPathLine2d *>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++)
        (*it)->debugDraw(a_pDrv, a_cOffset, a_fScale);
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d() : m_iSectionIndex(-1), m_pPrevious(nullptr), m_pParent(nullptr) {
      m_cPathLine.m_pParent = this;
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d(const SPathLine3d& a_cOther) : m_iSectionIndex(-1), m_pPrevious(nullptr), m_pParent(a_cOther.m_pParent) {
      m_cPathLine.m_pParent = this;

      for (int i = 0; i < 3; i++)
        m_cLines[i] = a_cOther.m_cLines[i];
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d(irr::core::line3df& a_cLine1, irr::core::line3df& a_cLine2, irr::core::line3df& a_cLine3) : m_iSectionIndex(-1), m_pPrevious(nullptr), m_pParent(nullptr) {
      m_cPathLine.m_pParent = this;

      m_cLines[0] = a_cLine1;
      m_cLines[1] = a_cLine2;
      m_cLines[2] = a_cLine3;
    }

    CControllerAi_V2::SPathLine3d::~SPathLine3d() {
      for (std::vector<SPathLine3d*>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++) {
        delete *it;
      }
      m_vNext.clear();
    }

    /**
    * Transform the lines to lie in the given plane
    * @param a_cPlane the plane to place the points on
    */
    void CControllerAi_V2::SPathLine3d::transformLinesToPlane(const irr::core::plane3df& a_cPlane, const irr::core::vector3df &a_cNormal) {
      for (int i = 0; i < 3; i++) {
        irr::core::vector3df l_cOut;

        if (a_cPlane.getIntersectionWithLine(m_cLines[i].start, a_cNormal, l_cOut))
          m_cLines[i].start = l_cOut;

        if (a_cPlane.getIntersectionWithLine(m_cLines[i].end, a_cNormal, l_cOut))
          m_cLines[i].end = l_cOut;
      }

      for (std::vector<SPathLine3d *>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++)
        (*l_itNext)->transformLinesToPlane(a_cPlane, a_cNormal);
    }

    /**
    * Create 2d path lines out of the list of 3d path lines
    * @param a_cMatrix the camera matrix to use for the transformation
    */
    CControllerAi_V2::SPathLine2d *CControllerAi_V2::SPathLine3d::transformTo2d(const irr::core::matrix4& a_cMatrix, std::map<irr::core::vector3df, int> &a_mSplitSelections) {
      m_cPathLine.m_vNext.clear();

      for (int i = 0; i < 3; i++) {
        irr::core::vector3df vs;
        irr::core::vector3df ve;

        a_cMatrix.transformVect(vs, m_cLines[i].start);
        a_cMatrix.transformVect(ve, m_cLines[i].end  );

        m_cPathLine.m_cLines[i] = irr::core::line2df(vs.X, vs.Y, ve.X, ve.Y);
      }

      m_cPathLine.m_fWidth  = (m_cPathLine.m_cLines[1].start - m_cPathLine.m_cLines[2].start).getLength();
      m_cPathLine.m_cMatrix = a_cMatrix;

      if (m_vNext.size() == 1) {
        SPathLine2d *l_pChild = (*m_vNext.begin())->transformTo2d(a_cMatrix, a_mSplitSelections);
        l_pChild->m_pPrevious = &m_cPathLine;
        m_cPathLine.m_vNext.push_back(l_pChild);
      }
      else if (m_vNext.size() > 0) {
        irr::core::vector3df v = m_cLines[0].start;

        if (a_mSplitSelections.find(v) == a_mSplitSelections.end() || a_mSplitSelections[v] < 0 || a_mSplitSelections[v] >= m_vNext.size()) {
          a_mSplitSelections[v] = std::rand() % m_vNext.size();
        }

        SPathLine2d *l_pChild = m_vNext[a_mSplitSelections[v]]->transformTo2d(a_cMatrix, a_mSplitSelections);
        l_pChild->m_pPrevious = &m_cPathLine;
        m_cPathLine.m_vNext.push_back(l_pChild);
      }

      return &m_cPathLine;
    }

    CControllerAi_V2::SAiPathSection::SAiPathSection() : m_iIndex(-1), m_iCheckpoint(-1), m_bStartup(false), m_pAiPath(nullptr) {
    }

    CControllerAi_V2::SAiPathSection::~SAiPathSection() {
      if (m_pAiPath != nullptr)
        delete m_pAiPath;
    }

    CControllerAi_V2::SRacePosition::SRacePosition() : m_iMarble(0), m_iPosition(0), m_iDeficitAhead(0), m_iDeficitLeader(0) {
    }

    CControllerAi_V2::SMarblePosition::SMarblePosition() : m_iMarbleId(-1) {
    }

    int CControllerAi_V2::m_iInstances = 0;
    std::vector<CControllerAi_V2::SAiPathSection *> CControllerAi_V2::m_vAiPath = std::vector<CControllerAi_V2::SAiPathSection *>();
  }
}