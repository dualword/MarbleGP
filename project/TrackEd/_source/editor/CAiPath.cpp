// (w) 2024 by Dustbin::Games / Christian Keimel
#include <scenenodes/CAiPathNode.h>
#include <scenenodes/CSpeedNode.h>
#include <editor/ITrackPart.h>
#include <editor/CAiPath.h>

namespace dustbin {
  namespace tracked {
    CAiPath::CAiPath(tracked::ITrackPart *a_pTrack) : m_bLapFinished(false) {
      tracked::ITrackPart *l_pStart = a_pTrack;

      // Make sure that we start from the beginning
      // regardless of which track part was given
      while (l_pStart->getPrevious() != nullptr)
        l_pStart = l_pStart->getPrevious();

      do {
        std::vector<SAiPath> l_vPartRoute = l_pStart->getAiPathData();

        int l_iIndex = 0;

        // Let's fill the member m_vPath
        for (auto &l_cRoute: l_vPartRoute) {
          bool l_bAdd = true;

          for (auto &l_cPath: m_vPath) {
            if (l_cRoute.m_cPosition == l_cPath.m_cPosition) {
              l_cPath.m_pPart  = l_pStart;
              l_cPath.m_fWidth = l_pStart->getWidth();

              l_bAdd = false;
              break;
            }
          }

          if (l_bAdd) {
            m_vPath.push_back(SAiPath(l_cRoute));
          }

          l_iIndex++;
        }

        l_pStart = l_pStart->getNext();
      } 
      while (l_pStart != nullptr && l_pStart->getPrevious() != nullptr);

      m_bLapFinished = l_pStart != nullptr && l_pStart->getPrevious() == nullptr;

      // Next we check how far a marble can jump if there is a jump
      for (size_t i = 1; i < m_vPath.size(); i++) {
        if (m_vPath[i].m_eType == enPathType::Jump) {
          irr::core::line2df l_cLine = irr::core::line2df(
            irr::core::vector2df(m_vPath[i - 1].m_cPosition.X, m_vPath[i - 1].m_cPosition.Z),
            irr::core::vector2df(m_vPath[i    ].m_cPosition.X, m_vPath[i    ].m_cPosition.Z)
          );

          while (i < m_vPath.size()) {
            i++;

            if (i < m_vPath.size()) {
              irr::core::line2df l_cOther = irr::core::line2df(
                irr::core::vector2df(m_vPath[i - 1].m_cPosition.X, m_vPath[i - 1].m_cPosition.Z),
                irr::core::vector2df(m_vPath[i    ].m_cPosition.X, m_vPath[i    ].m_cPosition.Z)
              );

              if (l_cLine.getAngleWith(l_cOther) < 1.0f) {
                m_vPath[i].m_eType = enPathType::JumpArea;
              }
              else break;
            }
          }
        }
      }

      // Now we guess how fast a marble can jump depending on some data
      for (size_t i = 0; i < m_vPath.size(); i++) {
        if (m_vPath[i].m_eType == enPathType::Jump) {
          std::vector<irr::core::vector3df> l_vBest;
          irr::core::vector3df l_cEnd;

          for (size_t j = i + 1; j < m_vPath.size(); j++) {
            if (m_vPath[j].m_eType != enPathType::JumpArea)
              break;
            else
              l_cEnd = 50.0f * m_vPath[j].m_cPosition;
          }

          irr::f32 l_fVel = 10.0f;

          for (; l_fVel < 150.0f; l_fVel += 5.0f) {
            irr::core::vector3df l_cPos = 50.0f * m_vPath[i].m_cPosition + irr::core::vector3df(0.0f, 1.0f, 0.0f);
            irr::core::vector3df l_cDir = l_fVel * (m_vPath[i].m_cPosition - m_vPath[i - 1].m_cPosition + irr::core::vector3df(0.0f, 0.1f, 0.0f)).normalize();

            irr::core::vector3df l_cOld;

            std::vector<irr::core::vector3df> l_vThis;

            do {
              l_vThis.push_back(l_cPos);
              l_cOld = l_cPos;
              l_cPos += l_cDir / 120.0f;

              l_cDir.Y -= 12.5f / 120.0f;
            } 
            while (l_cEnd.getDistanceFromSQ(l_cPos) < l_cEnd.getDistanceFromSQ(l_cOld));

            irr::f32 l_fDist1 = l_vBest.size() > 0 ? l_vBest.back().getDistanceFromSQ(l_cEnd) : 10000.0f;
            irr::f32 l_fDist2 = l_vThis.back().getDistanceFromSQ(l_cEnd);

            if (l_vBest.size() == 0 || l_fDist1 > l_fDist2)
              l_vBest = l_vThis;
            else {
              l_fVel -= 10.0f;
              break;
            }
          }

          if (l_vBest.size() > 0)
            m_vJumpData.push_back(l_vBest);

          m_vPath[i].m_fJumpMinVel  = 10.0f;
          m_vPath[i].m_fJumpMaxVel  = l_fVel;
          m_vPath[i].m_fJumpBestVel = 0.8f * l_fVel;

          printf("Jump max Vel: %.2f, %.2f, %.2f\n", m_vPath[i].m_fJumpMinVel, m_vPath[i].m_fJumpMaxVel, m_vPath[i].m_fJumpBestVel);
        }
      }

      // Now we guess the minimal speed (if necessary)
      for (size_t i = 0; i < m_vPath.size() - 1; i++) {
        if (m_vPath[i].m_eType == enPathType::Jump && m_vPath[i].m_bCalMinVel) {
          std::vector<irr::core::vector3df> l_vBest;
          irr::core::vector3df l_cEnd = 50.0f * m_vPath[i + 1].m_cPosition;

          irr::f32 l_fVel = 10.0f;

          for (; l_fVel < 150.0f; l_fVel += 5.0f) {
            irr::core::vector3df l_cPos = 50.0f * m_vPath[i].m_cPosition + irr::core::vector3df(0.0f, 1.0f, 0.0f);
            irr::core::vector3df l_cDir = l_fVel * (m_vPath[i].m_cPosition - m_vPath[i - 1].m_cPosition).normalize();

            irr::core::vector3df l_cOld;

            std::vector<irr::core::vector3df> l_vThis;

            do {
              l_vThis.push_back(l_cPos);
              l_cOld = l_cPos;
              l_cPos += l_cDir / 120.0f;

              l_cDir.Y -= 12.5f / 120.0f;
            } 
            while (l_cEnd.getDistanceFromSQ(l_cPos) < l_cEnd.getDistanceFromSQ(l_cOld));

            irr::f32 l_fDist1 = l_vBest.size() > 0 ? l_vBest.back().getDistanceFromSQ(l_cEnd) : 10000.0f;
            irr::f32 l_fDist2 = l_vThis.back().getDistanceFromSQ(l_cEnd);

            if (l_vBest.size() == 0 || l_fDist1 > l_fDist2)
              l_vBest = l_vThis;
            else {
              l_fVel -= 10.0f;
              break;
            }
          }

          if (l_vBest.size() > 0)
            m_vJumpData.push_back(l_vBest);

          m_vPath[i].m_fJumpMinVel  = l_fVel;

          printf("Jump min Vel: %.2f, %.2f, %.2f\n", m_vPath[i].m_fJumpMinVel, m_vPath[i].m_fJumpMaxVel, m_vPath[i].m_fJumpBestVel);
        }
      }
    }

    CAiPath::~CAiPath()  {
      for (auto l_pSign: m_vSpeedIndicators) {
        l_pSign->setVisible(false);
        l_pSign->getSceneManager()->addToDeletionQueue(l_pSign);
      }
    }

    /**
    * Add the AI data to the scene node storing the data
    * @param a_pAi the scene node to add the data to
    */
    void CAiPath::addAiToNode(scenenodes::CAiPathNode *a_pAi) {
      scenenodes::CAiPathNode::SAiPathSection *l_pLast  = nullptr;
      scenenodes::CAiPathNode::SAiPathSection *l_pFirst = nullptr;


      for (size_t i = 0; i < m_vPath.size(); i++) {
        scenenodes::CAiPathNode::SAiPathSection *p = new scenenodes::CAiPathNode::SAiPathSection();

        p->m_cPosition   = m_vPath[i].m_cPosition * 50.0f;
        p->m_cNormal     = m_vPath[i].m_cNormal;
        p->m_cSideVector = m_vPath[i].m_cSide.normalize();
        p->m_fWidth      = m_vPath[i].m_fWidth * 25.0f;

        if (m_vPath[i].m_eType == enPathType::Jump) {
          p->m_eType      = scenenodes::CAiPathNode::enSegmentType::Jump;
          p->m_fMinSpeed  = m_vPath[i].m_fJumpMinVel;
          p->m_fMaxSpeed  = m_vPath[i].m_fJumpMaxVel;
          p->m_fBestSpeed = m_vPath[i].m_fJumpBestVel;
        }
        else p->m_eType = scenenodes::CAiPathNode::enSegmentType::Default;

        if (l_pLast != nullptr) {
          l_pLast->m_vNextSegments.push_back(p);
          p->m_vPrevSegments.push_back(l_pLast);
        }

        if (l_pFirst == nullptr)
          l_pFirst = p;

        l_pLast = p;

        a_pAi->addPathSection(p);
      }

      if (l_pFirst != nullptr && l_pLast != nullptr) {
        l_pLast->m_vNextSegments.push_back(l_pFirst);
        l_pFirst->m_vPrevSegments.push_back(l_pLast);
      }
    }

    /**
    * Add and initialize the speed indicators before jumps
    * @param a_pSmgr the Irrlicht scene manager
    * @param a_pWorld the parent node to add the indicators to
    */
    void CAiPath::addSpeedIndicators(irr::scene::ISceneNode *a_pWorld, irr::scene::ISceneManager *a_pSmgr) {
      for (auto l_pSpeed: m_vSpeedIndicators) {
        l_pSpeed->setVisible(false);
        a_pSmgr->addToDeletionQueue(l_pSpeed);
      }
      
      m_vSpeedIndicators.clear();

      for (size_t i = 1; i < m_vPath.size(); i++) {
        if (m_vPath[i].m_eType == enPathType::Jump) {
          scenenodes::CSpeedNode *l_pNode = reinterpret_cast<scenenodes::CSpeedNode *>(a_pSmgr->addSceneNode(scenenodes::g_SpeedNodeName, a_pWorld));
          if (l_pNode != nullptr) {
            m_vSpeedIndicators.push_back(l_pNode);

            irr::io::IAttributes *l_pAttr = a_pSmgr->getFileSystem()->createEmptyAttributes();
            if (l_pAttr) {
              l_pNode->serializeAttributes(l_pAttr);

              l_pAttr->setAttribute("minspeed" , m_vPath[i].m_fJumpMinVel );
              l_pAttr->setAttribute("maxspeed" , m_vPath[i].m_fJumpMaxVel );
              l_pAttr->setAttribute("bestspeed", m_vPath[i].m_fJumpBestVel);

              l_pNode->deserializeAttributes(l_pAttr);

              l_pAttr->drop();
            }

            l_pNode->fillTextureMaps();
            l_pNode->setRotation((m_vPath[i].m_cPosition - m_vPath[i - 1].m_cPosition).getHorizontalAngle());
            l_pNode->setPosition(50.0f * m_vPath[i].m_cPosition + irr::core::vector3df(0.0f, 10.0f, 0.0f));

            for (int i = 0; i < 2; i++) {
              irr::f32 l_fPosX = 50.0f * (3.0f * m_vPath[i].m_fWidth / 5.0f);

              irr::scene::ISceneNode *l_pChild = a_pSmgr->addMeshSceneNode(
                a_pSmgr->getMesh("data/objects/speedlimit.3ds"),
                l_pNode,
                -1,
                irr::core::vector3df(i == 0 ? -l_fPosX : l_fPosX, 0.0f, 0.0f),
                irr::core::vector3df(0.0f),
                irr::core::vector3df(4.0f)
              );

              l_pChild->getMaterial(0).Lighting = false;
              l_pChild->getMaterial(0).setTexture(0, a_pSmgr->getVideoDriver()->getTexture("data/textures/speedlimit.png"));
              l_pChild->getMaterial(1).setTexture(0, a_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));
            }

            l_pNode->setMarbleSpeed(l_pNode->getBestSpeed(), 60);
          }
        }
      }
    }
    
    /**
    * Draw debug data
    * @param a_pDrv the video driver
    * @param a_pGui the GUI environment
    */
    void CAiPath::drawDebugData(irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui) {
      if (m_vPath.size() > 0) {
        for (size_t i = 0; i < m_vPath.size(); i++) {
          if (i < m_vPath.size() - 1 || m_bLapFinished) {
            a_pDrv->draw3DLine(
              50.0f * m_vPath[i].m_cPosition + 3.0f * m_vPath[i].m_cNormal,
              i < m_vPath.size() - 1 ? (50.0f * m_vPath[i + 1].m_cPosition + 3.0f * m_vPath[i + 1].m_cNormal) : (50.0f * m_vPath[0].m_cPosition + 3.0f * m_vPath[0].m_cNormal),
              m_vPath[i].m_eType == enPathType::Standard ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF) :
              m_vPath[i].m_eType == enPathType::Incline  ? irr::video::SColor(0xFF, 0xFF,    0,    0) :
              m_vPath[i].m_eType == enPathType::Decline  ? irr::video::SColor(0xFF,    0, 0xFF,    0) :
              m_vPath[i].m_eType == enPathType::Jump     ? irr::video::SColor(0xFF,    0,    0, 0xFF) :
              m_vPath[i].m_eType == enPathType::JumpArea ? irr::video::SColor(0xFF,    0,    0,    0) :
                                                           irr::video::SColor(0xFF, 0xFF,    0, 0xFF)
            );
            }
        }
      }

      for (auto l_cJump: m_vJumpData) {
        irr::core::vector3df l_cOld;

        for (auto l_cPos: l_cJump) {
          if (l_cPos != l_cJump.front()) {
            a_pDrv->draw3DLine(l_cOld, l_cPos, irr::video::SColor(0xFF, 0, 0xFF, 0));
          }

          l_cOld = l_cPos;
        }
      }
    }
  }
}