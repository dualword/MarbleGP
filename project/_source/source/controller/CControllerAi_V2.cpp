// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerAi_V2.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CAiPathNode.h>
#include <CGlobal.h>
#include <cmath>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace controller {
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

      if (l_pCurrent != nullptr) {
        printf("Section selected: index = %i, checkpoint = %i, last cp = %i\n", l_pCurrent->m_iIndex, l_pCurrent->m_iCheckpoint, m_iLastCheckpoint);

        if (l_pCurrent->m_iIndex == 320)
          printf("*");
      }

      return l_pCurrent;
    }

    /**
    * Draw a debug line
    * @param a_pDrv the Irrlicht video driver
    * @param a_cLine the line to draw
    * @param a_cColor the color of the line
    */
    void CControllerAi_V2::draw2dDebugLine(irr::video::IVideoDriver *a_pDrv, const irr::core::line2di& a_cLine, const irr::video::SColor& a_cColor) {
      a_pDrv->draw2DLine(a_cLine.start, a_cLine.end, a_cColor);
    }

    /**
    * Draw a debug line with factor
    * @param a_pDrv the Irrlicht video driver
    * @param a_cLine the line to draw
    * @param a_fFactor the factor to scale the line
    * @param a_cColor the color of the line
    * @param a_cOffset offset of the line
    */
    void CControllerAi_V2::draw2dDebugLineFloat(irr::video::IVideoDriver* a_pDrv, const irr::core::line2df& a_cLine, irr::f32 a_fFactor, const irr::video::SColor& a_cColor, const irr::core::vector2di& a_cOffset) {
      draw2dDebugLine(a_pDrv, irr::core::line2di(
        (irr::s32)(a_fFactor * a_cLine.start.X) + a_cOffset.X, 
        (irr::s32)(a_fFactor * a_cLine.start.Y) + a_cOffset.Y, 
        (irr::s32)(a_fFactor * a_cLine.end  .X) + a_cOffset.X,
        (irr::s32)(a_fFactor * a_cLine.end  .Y) + a_cOffset.Y), a_cColor);
    }

    /**
    * The constructor
    * @param a_iMarbleId the marble ID for this controller
    * @param a_sControls details about the skills of the controller
    */
    CControllerAi_V2::CControllerAi_V2(int a_iMarbleId, const std::string& a_sControls) : m_iMarbleId(a_iMarbleId), m_iLastCheckpoint(-1), m_pCurrent(nullptr) {
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
              p->m_cLine3d     = irr::core::line3df((*l_itSection)->m_cPosition, (*l_itNext)->m_cPosition);
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
                (*l_itSection)->m_cPosition - (*l_itSection)->m_fWidth * (*l_itSection)->m_cSideVector,   // Start point 1
                (*l_itNext   )->m_cPosition - (*l_itNext   )->m_fWidth * (*l_itNext   )->m_cSideVector,   // End point 1
                (*l_itSection)->m_cPosition + (*l_itSection)->m_fWidth * (*l_itSection)->m_cSideVector,   // Start point 2
                (*l_itNext   )->m_cPosition + (*l_itNext   )->m_fWidth * (*l_itNext   )->m_cSideVector    // End point 2
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
          irr::core::vector3df l_cThis = (*l_itThis)->m_cLine3d.end;

          for (std::vector<SAiPathSection*>::iterator l_itNext = m_vAiPath.begin(); l_itNext != m_vAiPath.end(); l_itNext++) {
            irr::core::vector3df l_cNext = (*l_itNext)->m_cLine3d.start;

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
        irr::core::vector3df l_cClosest = m_pCurrent->m_cLine3d.getClosestPoint(m_cPosition);

        do {
          if (l_cClosest == m_pCurrent->m_cLine3d.end) {
            if (m_pCurrent->m_vNext.size() == 1)
              m_pCurrent = *m_pCurrent->m_vNext.begin();
            else
              m_pCurrent = selectClosest(m_cPosition, m_pCurrent->m_vNext, false);

            l_cClosest = m_pCurrent->m_cLine3d.getClosestPoint(m_cPosition);
          }
        }
        while (m_pCurrent != nullptr && l_cClosest == m_pCurrent->m_cLine3d.end);

        if (m_pCurrent->m_pAiPath != nullptr) {
          if (m_p2dPath != nullptr)
            delete m_p2dPath;

          irr::core::matrix4 l_cMatrix;
          l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_cPosition + m_pCurrent->m_cNormal, m_cPosition, m_cDirection);

          m_p2dPath = m_pCurrent->m_pAiPath->transformTo2d(l_cMatrix);
        }        
      }
    }

    /**
    * Notify the controller about a marble respawn
    * @param a_iMarbleId the respawning marble
    */
    void CControllerAi_V2::onMarbleRespawn(int a_iMarbleId) {
      m_pCurrent = nullptr;
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CControllerAi_V2::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (a_iMarbleId == m_iMarbleId) {
        printf("Checkpoint %i passed.\n", a_iCheckpoint);
        m_iLastCheckpoint = a_iCheckpoint;
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
      if (m_pCurrent != nullptr) {

      }

      return true;
    }

    /**
    * Set the controller to debug mode
    * @param a_bDebug the new debug flag
    */
    void CControllerAi_V2::setDebug(bool a_bDebug) {
    }

    /**
    * For debuggin purposes: Draw the data used to control the marble (2d)
    * @param a_pDrv the video driver
    */
    void CControllerAi_V2::drawDebugData2d(irr::video::IVideoDriver* a_pDrv) {
      if (m_pCurrent != nullptr) {
        irr::video::SMaterial l_cMaterial;
        l_cMaterial.AmbientColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.EmissiveColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.DiffuseColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.SpecularColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);

        l_cMaterial.Lighting        = false;
        l_cMaterial.Thickness       = 5.0f;
        l_cMaterial.Wireframe       = true;
        l_cMaterial.BackfaceCulling = false;

        irr::core::dimension2du l_cSize = a_pDrv->getScreenSize();
        irr::core::vector2di l_cOffset = irr::core::vector2di(l_cSize.Width / 2, l_cSize.Height / 2);

        a_pDrv->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

        if (m_p2dPath != nullptr)
          m_p2dPath->debugDraw(a_pDrv, l_cOffset, 2.0f);

        a_pDrv->draw2DRectangleOutline(irr::core::recti(l_cOffset - irr::core::vector2di(15, 15), l_cOffset + irr::core::vector2di(15, 15)), irr::video::SColor(0xFF, 0, 0, 0xFF));
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
    * @param a_cPlane the plane of the previous sections (0 == m_cLine3d, 1 == m_cEdges[0], 2 == m_cEdges[1])
    * @param a_pPrevious the previous line item
    */
    CControllerAi_V2::SPathLine3d *CControllerAi_V2::SAiPathSection::prepareTransformedData(irr::f32 a_fLength, std::vector<int> &a_vStack, SPathLine3d *a_pPrevious) {

      for (std::vector<int>::iterator it = a_vStack.begin(); it != a_vStack.end(); it++)
        if ((*it) == m_iIndex) {
          printf("Section %i already in.\n", m_iIndex);
          return nullptr;
        }

      a_vStack.push_back(m_iIndex);

      SPathLine3d *l_pThis = new SPathLine3d(m_cLine3d, m_cEdges[0], m_cEdges[1]);
      
      if (a_pPrevious != nullptr)
        a_pPrevious->m_vNext.push_back(l_pThis);

      // Only process if 750 meters are not yet exceeded
      if (a_fLength <= 750.0f) {
        // Add the length of this segment to the processed length
        a_fLength += m_cLine3d.getLength();

        // Now let all succeeding sections process their data with this section's plane
        for (std::vector<SAiPathSection*>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++) {
          bool l_bAdd = true;

          if (m_iCheckpoint == 23004)
            printf("*\n");

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
            SPathLine3d *l_pNew = (*l_itNext)->prepareTransformedData(a_fLength, a_vStack, l_pThis);
            if (l_pNew == nullptr)
              break;
          }
        }

        irr::core::plane3df l_cPlane = irr::core::plane3df(m_cLine3d.start, m_cNormal);

        // Now transform all points of the lines in the vector to lie in this section's plane
        for (std::vector<SPathLine3d *>::iterator it = l_pThis->m_vNext.begin(); it != l_pThis->m_vNext.end(); it++)
          (*it)->transformLinesToPlane(l_cPlane, m_cNormal);
      }

      return l_pThis;
    }

    /**
    * Fill the vectors of the points for the next 500+ meters
    * with Irrlicht vectors transformed to the section plane
    */
    void CControllerAi_V2::SAiPathSection::fillLineVectors() {
      std::vector<int> l_vStack;

      m_pAiPath = prepareTransformedData(0.0f, l_vStack, nullptr);
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d() {
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(const SPathLine2d& a_cOther) {
      for (int i = 0; i < 3; i++)
        m_cLines[i] = a_cOther.m_cLines[i];
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(irr::core::line2df& a_cLine1, irr::core::line2df& a_cLine2, irr::core::line2df& a_cLine3) {
      m_cLines[0] = a_cLine1;
      m_cLines[1] = a_cLine2;
      m_cLines[2] = a_cLine3;
    }

    CControllerAi_V2::SPathLine2d::~SPathLine2d() {
      for (std::vector<SPathLine2d*>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++) {
        delete *it;
      }
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
          i == 0 ? irr::video::SColor(0xFF, 0, 0, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF)
        );

      if (m_vNext.size() > 1)
        a_pDrv->draw2DRectangle(
          irr::video::SColor(0xFF, 0xFF, 0xFF, 0),
          irr::core::recti(
            a_fScale * m_cLines[0].end.X + a_cOffset.X - 15, 
            a_fScale * m_cLines[0].end.Y + a_cOffset.Y - 15,
            a_fScale * m_cLines[0].end.X + a_cOffset.X + 15,
            a_fScale * m_cLines[0].end.Y + a_cOffset.Y + 15
          )
        );

      for (std::vector<SPathLine2d *>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++)
        (*it)->debugDraw(a_pDrv, a_cOffset, a_fScale);
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d() {
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d(const SPathLine3d& a_cOther) {
      for (int i = 0; i < 3; i++)
        m_cLines[i] = a_cOther.m_cLines[i];
    }

    CControllerAi_V2::SPathLine3d::SPathLine3d(irr::core::line3df& a_cLine1, irr::core::line3df& a_cLine2, irr::core::line3df& a_cLine3) {
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
    CControllerAi_V2::SPathLine2d *CControllerAi_V2::SPathLine3d::transformTo2d(const irr::core::matrix4& a_cMatrix) {
      SPathLine2d *l_pRet = new SPathLine2d();

      for (std::vector<SPathLine3d*>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++) {
        SPathLine2d *l_pChild = (*it)->transformTo2d(a_cMatrix);
        l_pRet->m_vNext.push_back(l_pChild);
      }

      for (int i = 0; i < 3; i++) {
        irr::core::vector3df vs;
        irr::core::vector3df ve;

        a_cMatrix.transformVect(vs, m_cLines[i].start);
        a_cMatrix.transformVect(ve, m_cLines[i].end  );

        l_pRet->m_cLines[i] = irr::core::line2df(vs.X, vs.Y, ve.X, ve.Y);
      }

      return l_pRet;
    }

    CControllerAi_V2::SAiPathSection::SAiPathSection() : m_iIndex(-1), m_iCheckpoint(-1), m_bStartup(false), m_pAiPath(nullptr) {
    }

    CControllerAi_V2::SAiPathSection::~SAiPathSection() {
      if (m_pAiPath != nullptr)
        delete m_pAiPath;
    }

    int CControllerAi_V2::m_iInstances = 0;
    std::vector<CControllerAi_V2::SAiPathSection *> CControllerAi_V2::m_vAiPath = std::vector<CControllerAi_V2::SAiPathSection *>();
  }
}