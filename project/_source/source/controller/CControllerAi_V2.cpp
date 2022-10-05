// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerAi_V2.h>
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
    */
    CControllerAi_V2::SAiPathSection *CControllerAi_V2::selectClosest(const irr::core::vector3df& a_cPosition, std::vector<SAiPathSection *> &a_vOptions) {
      SAiPathSection *l_pCurrent = nullptr;

      irr::f32 l_fDistance = -1.0f;

      for (std::vector<SAiPathSection*>::iterator l_itPath = a_vOptions.begin(); l_itPath != a_vOptions.end(); l_itPath++) {
        irr::core::vector3df l_cOption = (*l_itPath)->m_cLine3d.getClosestPoint(a_cPosition);
        irr::f32 l_fOption = l_cOption.getDistanceFromSQ(a_cPosition);

        if (l_fDistance == -1.0f || l_fOption < l_fDistance) {
          l_pCurrent = *l_itPath;
          l_fDistance = l_fOption;
        }
      }

      return l_pCurrent;
    }

    /**
    * The constructor
    * @param a_iMarbleId the marble ID for this controller
    * @param a_sControls details about the skills of the controller
    */
    CControllerAi_V2::CControllerAi_V2(int a_iMarbleId, const std::string& a_sControls) : m_iMarbleId(a_iMarbleId), m_pCurrent(nullptr) {
      CGlobal *l_pGlobal = CGlobal::getInstance();

      std::vector<const scenenodes::CAiPathNode *> l_vAiNodes;

      findAiPathNodes(l_pGlobal->getSceneManager()->getRootSceneNode(), l_vAiNodes);

      printf("%i AI path nodes found.\n", (int)l_vAiNodes.size());

      // Iterate over all found AI Path Nodes
      for (std::vector<const scenenodes::CAiPathNode*>::iterator l_itPath = l_vAiNodes.begin(); l_itPath != l_vAiNodes.end(); l_itPath++) {
        // Iterate the AI Path sections stored in the AI path nodes
        for (std::vector<scenenodes::CAiPathNode::SAiPathSection*>::const_iterator l_itSection = (*l_itPath)->m_vSections.begin(); l_itSection != (*l_itPath)->m_vSections.end(); l_itSection++) {
          // Create one SAiPathSection for each link between the current section and it's successors
          for (std::vector<scenenodes::CAiPathNode::SAiPathSection*>::const_iterator l_itNext = (*l_itSection)->m_vNextSegments.begin(); l_itNext != (*l_itSection)->m_vNextSegments.end(); l_itNext++) {
            SAiPathSection *p = new SAiPathSection();
            p->m_cLine3d = irr::core::line3df((*l_itSection)->m_cPosition, (*l_itNext)->m_cPosition);

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

      if (m_vAiPath.size() > 0) {
        SAiPathSection *l_pSection = *m_vAiPath.begin();

        irr::f32 l_fLength = 0.0f;

        std::vector<SAiPathSection *> l_vStack;

        while (l_fLength < 500.0f) {
          l_vStack.push_back(l_pSection);
          l_fLength += l_pSection->m_cLine3d.getLength();
          l_pSection = *(l_pSection->m_vNext.begin());
        }

        printf("%i section necessary for 500.0f meters.\n", (int)l_vStack.size());
      }
    }

    CControllerAi_V2::~CControllerAi_V2() {
      while (m_vAiPath.size() > 0) {
        SAiPathSection *p = *m_vAiPath.begin();
        m_vAiPath.erase(m_vAiPath.begin());
        delete p;
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
        m_pCurrent = selectClosest(m_cPosition, m_vAiPath);

        if (m_pCurrent != nullptr)
          printf("AI Path section selected: %.2f, %.2f, %.2f\n", m_pCurrent->m_cLine3d.start.X, m_pCurrent->m_cLine3d.start.Y, m_pCurrent->m_cLine3d.start.Z);
        else
          printf("No section found.\n");
      }

      if (m_pCurrent != nullptr) {
        irr::core::vector3df l_cClosest = m_pCurrent->m_cLine3d.getClosestPoint(m_cPosition);

        do {
          if (l_cClosest == m_pCurrent->m_cLine3d.end) {
            printf("Next section: %.2f, %.2f, %.2f.\n", m_pCurrent->m_cLine3d.start.X, m_pCurrent->m_cLine3d.start.Y, m_pCurrent->m_cLine3d.start.Z);
            m_pCurrent = selectClosest(m_cPosition, m_pCurrent->m_vNext);

            if (m_pCurrent != nullptr)
              l_cClosest = m_pCurrent->m_cLine3d.getClosestPoint(m_cPosition);
          }
        }
        while (m_pCurrent != nullptr && l_cClosest == m_pCurrent->m_cLine3d.end);
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

        // a_pDrv->setMaterial(l_cMaterial);
        // a_pDrv->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());
        // a_pDrv->draw3DLine(m_pCurrent->m_cLine3d.start + 2.0f * m_pCurrent->m_cNormal, m_pCurrent->m_cLine3d.end + 2.0f * m_pCurrent->m_cNormal, irr::video::SColor(0xFF, 0xFF, 0xFF, 0));


        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_cPosition + m_pCurrent->m_cNormal, m_cPosition - m_pCurrent->m_cNormal, m_cDirection);

        irr::core::vector3df l_cLast;

        irr::core::dimension2du l_cSize = a_pDrv->getScreenSize();
        irr::core::vector2di l_cOffset = irr::core::vector2di(l_cSize.Width / 2, l_cSize.Height / 2);

        for (std::vector<irr::core::vector3df>::iterator l_itPoint = m_pCurrent->m_vLinePoints.begin(); l_itPoint != m_pCurrent->m_vLinePoints.end(); l_itPoint++) {
          irr::core::vector3df v;
          l_cMatrix.transformVect(v, *l_itPoint);
          
          if (l_itPoint != m_pCurrent->m_vLinePoints.begin()) {
            a_pDrv->draw2DLine(irr::core::vector2di((irr::s32)(2.0f * v.X + l_cOffset.X), (irr::s32)(2.0f * v.Y + l_cOffset.Y)), irr::core::vector2di((irr::s32)(2.0f * l_cLast.X + l_cOffset.X), (irr::s32)(2.0f * l_cLast.Y + l_cOffset.Y)), irr::video::SColor(0xFF, 0, 0, 0));
          }

          l_cLast = v;
        }

        std::vector<irr::core::line2di> l_vEdgeLines[2];

        for (int i = 0; i < 2; i++) {
          for (std::vector<irr::core::vector3df>::iterator l_itPoint = m_pCurrent->m_vEdgePoints[i].begin(); l_itPoint != m_pCurrent->m_vEdgePoints[i].end(); l_itPoint++) {
            irr::core::vector3df v;
            l_cMatrix.transformVect(v, *l_itPoint);

            if (l_itPoint != m_pCurrent->m_vEdgePoints[i].begin()) {
              l_vEdgeLines[i].push_back(irr::core::line2di(irr::core::vector2di((irr::s32)(2.0f * v.X + l_cOffset.X), (irr::s32)(2.0f * v.Y + l_cOffset.Y)), irr::core::vector2di((irr::s32)(2.0f * l_cLast.X + l_cOffset.X), (irr::s32)(2.0f * l_cLast.Y + l_cOffset.Y))));
            }

            l_cLast = v;
          }
        }

        std::vector<irr::core::line2di>::iterator l_itLine1 = l_vEdgeLines[0].begin();
        std::vector<irr::core::line2di>::iterator l_itLine2 = l_vEdgeLines[1].begin();

        while (l_itLine1 != l_vEdgeLines[0].end() && l_itLine2 != l_vEdgeLines[1].end()) {
          irr::core::vector2di v;

          a_pDrv->draw2DLine((*l_itLine1).start, (*l_itLine2).end, irr::video::SColor(0xFF, 0, 0, 0xFF));
          a_pDrv->draw2DLine((*l_itLine2).start, (*l_itLine1).end, irr::video::SColor(0xFF, 0, 0, 0xFF));

          l_itLine1++;
          l_itLine2++;
        }

        for (int i = 0; i < 2; i++) {
          irr::core::vector2di v1 = l_vEdgeLines[i].back().start;
          a_pDrv->draw2DRectangleOutline(irr::core::recti(v1.X - 15, v1.Y - 15, v1.X + 15, v1.Y + 15));
        }

        a_pDrv->draw2DRectangleOutline(irr::core::recti(l_cSize.Width / 2 - 15, l_cSize.Height / 2 - 15, l_cSize.Width / 2 + 15, l_cSize.Height / 2 + 15), irr::video::SColor(0xFF, 0, 0, 0xFF));
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
    * Fill the vectors of the points for the next 500+ meters
    * with Irrlicht vectors transformed to the section plane
    */
    void CControllerAi_V2::SAiPathSection::fillLineVectors() {
      irr::f32 l_fLength = 0.0f;
      SAiPathSection *l_pSection = this;
      std::vector<SAiPathSection *> l_vStack;

      // We fill a vector as stack with all section
      // necessary for at least 500 meters
      while (l_fLength < 500.0f) {
        l_vStack.push_back(l_pSection);
        l_fLength += l_pSection->m_cLine3d.getLength();
        l_pSection = *l_pSection->m_vNext.begin();
      }

      std::vector<irr::core::vector3df> l_vPath;
      std::vector<irr::core::vector3df> l_vTranslatedPath;

      std::vector<irr::core::vector3df> l_vEdges[2];
      std::vector<irr::core::vector3df> l_vTranslatedEdges[2];

      SAiPathSection *l_pLast = l_vStack.back();
      l_vStack.pop_back();

      bool l_bFirst = true;
      irr::core::vector3df l_cLast[2];

      // In this step we go through the stack from it's end,
      // all points are transformed to be in the plane of
      // the topmost section of the stack. This way we can
      // get all points transformed to be in the plane
      // of this section
      while (l_vStack.size() > 0) {
        l_vTranslatedPath.clear();

        for (int i = 0; i < 2; i++)
          l_vTranslatedEdges[i].clear();

        SAiPathSection *l_pThis = l_vStack.back();

        irr::core::plane3df l_cPlane = irr::core::plane3df(l_pThis->m_cLine3d.start, l_pThis->m_cNormal);
        irr::core::vector3df l_cOut;

        l_vPath.push_back(l_pLast->m_cLine3d.end);

        if (l_bFirst) {
          l_vEdges[0].push_back(l_pLast->m_cEdges[0].end);
          l_vEdges[1].push_back(l_pLast->m_cEdges[1].end);

          l_cLast[0] = l_pLast->m_cEdges[0].end;
          l_cLast[1] = l_pLast->m_cEdges[1].end;
        }
        else {
          if (!doLinesOverlap(irr::core::line3df(l_cLast[0], l_pLast->m_cEdges[0].end), irr::core::line3df(l_cLast[1], l_pLast->m_cEdges[1].end))) {
            l_cLast[0] = l_pLast->m_cEdges[0].end;
            l_cLast[1] = l_pLast->m_cEdges[1].end;

          }
          else {
            l_cLast[0] = l_pLast->m_cEdges[1].end;
            l_cLast[1] = l_pLast->m_cEdges[0].end;
          }
          l_vEdges[0].push_back(l_cLast[0]);
          l_vEdges[1].push_back(l_cLast[1]);
        }

        for (std::vector<irr::core::vector3df>::iterator l_itPoint = l_vPath.begin(); l_itPoint != l_vPath.end(); l_itPoint++) {
          if (l_cPlane.getIntersectionWithLine(*l_itPoint, l_pLast->m_cNormal, l_cOut)) {
            l_vTranslatedPath.push_back(l_cOut);
          }
        }

        for (int i = 0; i < 2; i++) {
          for (std::vector<irr::core::vector3df>::iterator l_itPoint = l_vEdges[i].begin(); l_itPoint != l_vEdges[i].end(); l_itPoint++) {
            if (l_cPlane.getIntersectionWithLine(*l_itPoint, l_pLast->m_cNormal, l_cOut)) {
              l_vTranslatedEdges[i].push_back(l_cOut);
            }
          }
        }

        l_pLast = l_pThis;
        l_vPath = l_vTranslatedPath;

        for (int i = 0; i < 2; i++)
          l_vEdges[i] = l_vTranslatedEdges[i];

        l_vStack.pop_back();
        l_bFirst = false;
      }

      l_vTranslatedPath.push_back(m_cLine3d.start);
      m_vLinePoints = l_vTranslatedPath;

      if (!doLinesOverlap(irr::core::line3df(l_cLast[0], m_cEdges[0].start), irr::core::line3df(l_cLast[1], m_cEdges[1].start))) {
        l_vTranslatedEdges[0].push_back(m_cEdges[0].start);
        l_vTranslatedEdges[1].push_back(m_cEdges[1].start);
      }
      else {
        l_vTranslatedEdges[0].push_back(m_cEdges[1].start);
        l_vTranslatedEdges[1].push_back(m_cEdges[0].start);
      }

      for (int i = 0; i < 2; i++) {
        m_vEdgePoints[i] = l_vTranslatedEdges[i];
      }

    }
  }
}