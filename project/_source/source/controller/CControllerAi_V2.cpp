// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaScript_ai.h>
#include <controller/CControllerAi_V2.h>
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
    * Draw a debug line with factor
    * @param a_cLine the line to draw
    * @param a_fFactor the factor to scale the line
    * @param a_cColor the color of the line
    * @param a_cOffset offset of the line
    */
    void CControllerAi_V2::draw2dDebugLine(const irr::core::line2df& a_cLine, irr::f32 a_fFactor, const irr::video::SColor& a_cColor, const irr::core::vector2di& a_cOffset) {
      m_pDrv->draw2DLine(
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
    * @param a_cPos the center position
    * @param a_cColor the rectangle color
    * @param a_iSize the size of the rectangle
    * @param a_fScale the scale factor
    * @param a_cOffset the offset of the rectangle
    */
    void CControllerAi_V2::draw2dDebugRectangle(const irr::core::vector2df& a_cPos, const irr::video::SColor& a_cColor, int a_iSize, irr::f32 a_fScale, const irr::core::vector2di& a_cOffset) {
      irr::core::vector2di l_cUpperLeft = irr::core::vector2di((irr::s32)(a_fScale * a_cPos.X) + a_cOffset.X - a_iSize / 2, (irr::s32)(a_fScale * a_cPos.Y) + a_cOffset.Y - a_iSize / 2);
      m_pDrv->draw2DRectangleOutline(irr::core::recti(l_cUpperLeft, irr::core::dimension2du(a_iSize, a_iSize)), a_cColor);
    }

    /**
    * Draw a debug text string
    * @param a_sText the text to render
    * @param a_pFont the font to use
    * @param a_cPosition the screen position to render to
    */
    void CControllerAi_V2::draw2dDebugText(const wchar_t* a_sText, irr::gui::IGUIFont* a_pFont, const irr::core::vector2df &a_cPosition) {
      irr::core::dimension2du l_cDim = a_pFont->getDimension(a_sText);
      l_cDim.Width  = 6 * l_cDim.Width  / 5;
      l_cDim.Height = 6 * l_cDim.Height / 5;

      irr::core::recti l_cRect = irr::core::recti(irr::core::vector2di((irr::s32)a_cPosition.X, (irr::s32)a_cPosition.Y), l_cDim);

      m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 192, 192, 192), l_cRect);
      a_pFont->draw(a_sText, l_cRect, irr::video::SColor(0xFF, 0, 0, 0), true, true);
    }

    /**
    * Find all AI path nodes in the scene tree
    * @param a_pNode the node to check
    * @param a_bAiHelp search for nodes for AI help?
    * @param a_vNodes [out] the vector of nodes that will be filled
    */
    void CControllerAi_V2::findAiPathNodes(const irr::scene::ISceneNode* a_pNode, bool a_bAiHelp, std::vector<const scenenodes::CAiPathNode *>& a_vNodes) {
      int l_iAiNodes = 0;
      int l_iAiHelp  = 0; // Number of AI nodes marked with the "AI Help" flag

      // Step one: count the number of AI node children
      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itNodes = a_pNode->getChildren().begin(); l_itNodes != a_pNode->getChildren().end(); l_itNodes++) {
        if ((*l_itNodes)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_AiPathNodeId) {
          l_iAiNodes++;

          if (reinterpret_cast<scenenodes::CAiPathNode *>(*l_itNodes)->isAiHelpNode())
            l_iAiHelp++;
        }
      }

      // Only one node or no specific AI help node found: add it
      if (l_iAiNodes == 1 || l_iAiHelp == 0) {
        for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itNodes = a_pNode->getChildren().begin(); l_itNodes != a_pNode->getChildren().end(); l_itNodes++) {
          if ((*l_itNodes)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_AiPathNodeId) {
            a_vNodes.push_back(reinterpret_cast<scenenodes::CAiPathNode *>(*l_itNodes));
            printf("Single AI node found, adding to list [%i, %i]\n", l_iAiHelp, (*l_itNodes)->getID());
            break;
          }
        }
      }
      else if (l_iAiNodes > 1) {
        for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itNodes = a_pNode->getChildren().begin(); l_itNodes != a_pNode->getChildren().end(); l_itNodes++) {
          if ((*l_itNodes)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_AiPathNodeId) {
            scenenodes::CAiPathNode *l_pNode = reinterpret_cast<scenenodes::CAiPathNode *>(*l_itNodes);

            if (l_pNode->isAiHelpNode() == a_bAiHelp) {
              a_vNodes.push_back(l_pNode);
              printf("Special AI help node found, adding.\n");
              break;
            }
          }
        }
      }

      
      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itNodes = a_pNode->getChildren().begin(); l_itNodes != a_pNode->getChildren().end(); l_itNodes++)
        findAiPathNodes(*l_itNodes, a_bAiHelp, a_vNodes);
    }

    /**
    * Select the closest AI path section to the position. Will be called
    * when the race is started, after respawn and stun
    * @param a_cPosition the position of the marble
    * @param a_bSelectStartupPath select a path marked as "startup"
    * @param a_bOverrideSelected ignore the selected flag if true
    */
    CControllerAi_V2::SAiPathSection *CControllerAi_V2::selectClosest(const irr::core::vector3df& a_cPosition, std::vector<SAiPathSection *> &a_vOptions, bool a_bSelectStartupPath, bool a_bOverrideSelected) {
      SAiPathSection *l_pCurrent = nullptr;

      irr::f32 l_fDistance = -1.0f;

      if (a_bSelectStartupPath) {
        for (std::vector<SAiPathSection*>::iterator l_itPath = a_vOptions.begin(); l_itPath != a_vOptions.end(); l_itPath++) {
          if ((*l_itPath)->m_eType == scenenodes::CAiPathNode::enSegmentType::Startup) {
            irr::core::vector3df l_cOption = (*l_itPath)->m_cLine3d.getClosestPoint(a_cPosition);
            irr::f32 l_fOption = l_cOption.getDistanceFromSQ(a_cPosition);

            if (l_fDistance == -1 || l_fOption < l_fDistance) {
              l_pCurrent = *l_itPath;
              l_fDistance = l_fOption;
            }
          } 
        }
      }

      if (l_pCurrent == nullptr) {
        l_fDistance = -1.0f;

        for (std::vector<SAiPathSection*>::iterator l_itPath = a_vOptions.begin(); l_itPath != a_vOptions.end(); l_itPath++) {
          irr::core::vector3df l_cOption = (*l_itPath)->m_cLine3d.getClosestPoint(a_cPosition);
          irr::f32 l_fOption = l_cOption.getDistanceFromSQ(a_cPosition);

          if (!a_bOverrideSelected && (*l_itPath)->m_bSelected) {
            l_pCurrent = *l_itPath;
            break;
          }

          if ((l_fDistance == -1.0f    || l_fOption < l_fDistance) && (m_iLastCheckpoint == -1 || (*l_itPath)->m_iCheckpoint == m_iLastCheckpoint || (*l_itPath)->m_iCheckpoint == -1)) {
            l_pCurrent = *l_itPath;
            l_fDistance = l_fOption;
          }
        }
      }

      if (a_bSelectStartupPath && l_pCurrent->m_eType == scenenodes::CAiPathNode::enSegmentType::Startup)
        printf("Startup path selected.\n");

      return l_pCurrent;
    }

    /**
    * Select the new current AI path section
    * @param a_cPosition the marble's position
    * @param a_pCurrent the current section
    * @return the new current AI path section
    */
    CControllerAi_V2::SAiPathSection *CControllerAi_V2::selectCurrentSection(const irr::core::vector3df& a_cPosition, SAiPathSection* a_pCurrent, bool a_bOverrideSelected) {
      irr::core::vector3df  l_cClosest  = a_pCurrent->m_cLine3d.getClosestPoint(a_cPosition);

      do {
        if (l_cClosest == a_pCurrent->m_cLine3d.end) {
          if (a_pCurrent->m_vNext.size() == 1)
            a_pCurrent = *a_pCurrent->m_vNext.begin();
          else
            a_pCurrent = selectClosest(a_cPosition, a_pCurrent->m_vNext, false, a_bOverrideSelected);

          if (a_pCurrent != nullptr)
            l_cClosest = a_pCurrent->m_cLine3d.getClosestPoint(a_cPosition);
        }
      }
      while (a_pCurrent != nullptr && l_cClosest == a_pCurrent->m_cLine3d.end);

      return a_pCurrent;
    }

    /**
    * The constructor
    * @param a_iMarbleId the marble ID for this controller
    * @param a_sControls details about the skills of the controller
    * @param a_pMarbles an array of the 16 possible marbles, ID of -1 is not used
    * @param a_pLuaScript an optional LUA script to help the C++ code make decirions
    * @param a_cViewport the viewport of the player, necessary for debug data output
    */
    CControllerAi_V2::CControllerAi_V2(int a_iMarbleId, const std::string& a_sControls, data::SMarblePosition *a_pMarbles, lua::CLuaScript_ai *a_pLuaScript, const irr::core::recti &a_cViewport) : 
      m_eAiMode        (enAiMode::Marble3),
      m_iMarbleId      (a_iMarbleId), 
      m_iIndex         (a_iMarbleId - 10000),
      m_iLastCheckpoint(-1), 
      m_iMyPosition    (0),
      m_iClassIndex    (0),
      m_iRespawn       (0),
      m_iSplitIndex    (0),
      m_fVCalc         (0.0f), 
      m_fScale         (1.0f),
      m_fJumpFact      (1.0f),
      m_fCtrlY         (0.0f),
      m_pCurrent       (nullptr), 
      m_pHUD           (nullptr), 
      m_fOldAngle      (0.0),
      m_eMode          (enMarbleMode::Cruise),
      m_pDrv           (CGlobal::getInstance()->getVideoDriver()),
      m_pDebugRTT      (nullptr),
      m_pFont          (CGlobal::getInstance()->getFont(dustbin::enFont::Small, irr::core::dimension2du(a_cViewport.getWidth(), a_cViewport.getHeight()))),
      m_pLuaScript     (a_pLuaScript),
      m_cViewport      (a_cViewport),
      m_aMarbles       (a_pMarbles),
      m_p2dPath        (nullptr)
    {
      m_cAiData = data::SMarbleAiData(a_sControls);

      if (m_cAiData.m_iMarbleClass == 0 || m_cAiData.m_iMarbleClass == 3)
        m_eAiMode = enAiMode::MarbleGP;
      else if (m_cAiData.m_iMarbleClass == 1 || m_cAiData.m_iMarbleClass == 4)
        m_eAiMode = enAiMode::Marble2;
      else if (m_cAiData.m_iMarbleClass == 2 || m_cAiData.m_iMarbleClass == 5)
        m_eAiMode = enAiMode::Marble3;

      m_bAiHelp = m_cAiData.m_iMarbleClass < 3;

      m_iClassIndex = m_cAiData.m_iMarbleClass % 2;

      CGlobal *l_pGlobal = CGlobal::getInstance();

      std::vector<const scenenodes::CAiPathNode *> l_vAiNodes;

      findAiPathNodes(l_pGlobal->getSceneManager()->getRootSceneNode(), m_bAiHelp, l_vAiNodes);

      printf("%i AI path nodes found.\n", (int)l_vAiNodes.size());

      int l_iIndex = 0;
      int l_iNoNxt = 0;

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

            p->m_eType = (*l_itSection)->m_eType;
            p->m_iTag  = (*l_itSection)->m_iTag;

            if (p->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump) {
              p->m_fMinVel  = (*l_itSection)->m_fMinSpeed;
              p->m_fMaxVel  = (*l_itSection)->m_fMaxSpeed;
              p->m_fBestVel = (*l_itSection)->m_fBestSpeed;
            }

            irr::core::vector3df l_cStart = (*l_itSection)->m_cPosition;
            irr::core::vector3df l_cEnd   = (*l_itNext   )->m_cPosition;

            irr::core::vector3df l_cDir = ((*l_itNext)->m_cPosition - (*l_itSection)->m_cPosition).normalize();

            l_cStart -= 0.5f * l_cDir;
            l_cEnd   += 0.5f * l_cDir;

            p->m_cRealLine   = irr::core::line3df((*l_itSection)->m_cPosition, (*l_itNext)->m_cPosition);
            p->m_cLine3d     = irr::core::line3df(l_cStart, l_cEnd);
            p->m_iIndex      = ++l_iIndex;
            p->m_iSectionIdx = (*l_itSection)->m_iIndex;
            p->m_iCheckpoint = -1;
            p->m_iRespawn    = (*l_itSection)->m_iRespawn;
            

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
              
              
            irr::f32 l_fClass = m_eAiMode == enAiMode::MarbleGP ? 1.0f : m_eAiMode == enAiMode::Marble2 ? 0.85f : 0.8f;// m_iClassIndex == 0 ? 1.0f : m_iClassIndex == 1 ? 0.85f : 0.8f;

            if (m_bAiHelp)
              l_fClass *= 1.25f;

            irr::core::vector3df l_cEdgePoints[] = {
              (*l_itSection)->m_cPosition - (*l_itSection)->m_fWidth * (*l_itSection)->m_fFactor * (*l_itSection)->m_cSideVector * l_fClass,   // Start point 1
              (*l_itNext   )->m_cPosition - (*l_itNext   )->m_fWidth * (*l_itNext   )->m_fFactor * (*l_itNext   )->m_cSideVector * l_fClass,   // End point 1
              (*l_itSection)->m_cPosition + (*l_itSection)->m_fWidth * (*l_itSection)->m_fFactor * (*l_itSection)->m_cSideVector * l_fClass,   // Start point 2
              (*l_itNext   )->m_cPosition + (*l_itNext   )->m_fWidth * (*l_itNext   )->m_fFactor * (*l_itNext   )->m_cSideVector * l_fClass    // End point 2
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
            
            p->m_cNormal   = (*l_itSection)->m_cNormal;
            p->m_fWidthSq  = (*l_itSection)->m_fWidth * (*l_itSection)->m_fFactor * l_fClass;
            p->m_fWidthSq *= p->m_fWidthSq;

            m_vAiPath.push_back(p);
          }
        }
      }

      // Now that we have a filled vector of the path sections we need to link them
      for (std::vector<SAiPathSection*>::iterator l_itThis = m_vAiPath.begin(); l_itThis != m_vAiPath.end(); l_itThis++) {
        irr::core::vector3df l_cThis = (*l_itThis)->m_cRealLine.end;

        std::tuple<irr::f32, SAiPathSection *> l_tNextOptions = std::tuple<irr::f32, SAiPathSection *>(0.0f, nullptr);

        for (std::vector<SAiPathSection*>::iterator l_itNext = m_vAiPath.begin(); l_itNext != m_vAiPath.end(); l_itNext++) {
          irr::core::vector3df l_cNext = (*l_itNext)->m_cRealLine.start;

          // Now we check if the end of this line matches the start of the next line.
          // We use a certain threshold as the serialization is not 100% accurate
          if (abs(l_cThis.X - l_cNext.X) < 0.02f && abs(l_cThis.Y - l_cNext.Y) < 0.02f && abs(l_cThis.Z - l_cNext.Z) < 0.02f) {
            bool l_bAdd = (*l_itThis)->m_iCheckpoint == (*l_itNext)->m_iCheckpoint;

            if (!l_bAdd) {
              for (std::vector<int>::iterator it = (*l_itThis)->m_vCheckpoints.begin(); it != (*l_itThis)->m_vCheckpoints.end(); it++) {
                if ((*it) == (*l_itNext)->m_iCheckpoint) {
                  l_bAdd = true;
                  break;
                }
              }
            }

            if (l_bAdd) {
              (*l_itThis)->m_vNext.push_back(*l_itNext);
              (*l_itNext)->m_pPrev = *l_itThis;
            }
          }
          else {
            if (std::get<1>(l_tNextOptions) == nullptr)
              l_tNextOptions = std::make_tuple(l_cThis.getDistanceFrom(l_cNext), * l_itNext);
            else {
              irr::f32 l_fDist = l_cThis.getDistanceFrom(l_cNext);
              if (l_fDist < std::get<0>(l_tNextOptions))
                l_tNextOptions = std::make_tuple(l_fDist, *l_itNext);
            }
          }
        }

        if ((*l_itThis)->m_vNext.size() == 0) {
          if (std::get<1>(l_tNextOptions) != nullptr) {
            (*l_itThis)->m_vNext.push_back(std::get<1>(l_tNextOptions));
          }
          else {
            printf("No next nodes found for AI path section %i (%i next checkpoints)\n", (*l_itThis)->m_iIndex, (int)(*l_itThis)->m_vCheckpoints.size());
            l_iNoNxt++;
          }
        }
      }

      printf("%i AI path sections found [%i sections without links].\n", (int)m_vAiPath.size(), l_iNoNxt);

      // Now we calculate the next 500+ meters for all AI path sections
      for (std::vector<SAiPathSection*>::iterator l_itThis = m_vAiPath.begin(); l_itThis != m_vAiPath.end(); l_itThis++) {
        (*l_itThis)->fillLineVectors(a_pMarbles, m_iMarbleId);
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
        }
      }

      printf("%i unlinked sections found.\n", l_iZeroLinks);
      printf("Ready.");

      for (int i = 0; i < (int)enSkill::Count; i++)
        m_iSkills[i] = 0;

      if (m_pLuaScript != nullptr)
        m_pLuaScript->marbleclass(m_eAiMode == enAiMode::MarbleGP ? 0 : m_eAiMode == enAiMode::Marble2 ? 1 : 2);
    }

    CControllerAi_V2::~CControllerAi_V2() {
      printf("Deleting AI data.\n");
      while (m_vAiPath.size() > 0) {
        SAiPathSection *p = *m_vAiPath.begin();
        m_vAiPath.erase(m_vAiPath.begin());
        delete p;
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
    * Notify the controller about a marble respawn
    * @param a_iMarbleId the respawning marble
    */
    void CControllerAi_V2::onMarbleRespawn(int a_iMarbleId) {
      if (a_iMarbleId == m_iMarbleId) {
        if (m_pCurrent != nullptr) {
          m_pCurrent->m_bSelected = false;

          for (std::vector<SAiPathSection*>::iterator l_itNext = m_pCurrent->m_vNext.begin(); l_itNext != m_pCurrent->m_vNext.end(); l_itNext++) {
            (*l_itNext)->m_bSelected = false;
          }
        }

        m_pCurrent = nullptr;
        m_eMode = enMarbleMode::Respawn2;
        
        for (auto& l_itPath : m_vAiPath) {
          if (l_itPath->m_eType == scenenodes::CAiPathNode::enSegmentType::Respawn && m_iLastCheckpoint == l_itPath->m_iRespawn) {
            m_pCurrent = l_itPath;
            printf("Found respawn section: %i (%.2f, %.2f, %.2f)\n", m_pCurrent->m_iIndex, m_pCurrent->m_cLine3d.start.X, m_pCurrent->m_cLine3d.start.Y, m_pCurrent->m_cLine3d.start.Z);
            break;
          }
        }

        if (m_pCurrent == nullptr)
          printf("No suitable respawn section found.\n");

        m_iRespawn++;
        rollDice();
      }
    }

    /**
    * Update the skill values using random number generation
    */
    void CControllerAi_V2::rollDice() {
      printf("%i (%s)", m_iMarbleId, m_eAiMode == enAiMode::MarbleGP ? "MarbleGP" : m_eAiMode == enAiMode::Marble2 ? "Marble2 " : "Marble3 ");

      for (int i = 0; i < (int)enSkill::Count; i++) {
        if (m_iRespawn < 5 && !m_bAiHelp) {
          int l_iQuot = m_iRespawn == 0 ? 100 : m_iRespawn == 1 ? 80 : m_iRespawn == 2 ? 60 : m_iRespawn == 3 ? 40 : 20;
          m_iSkills[i] = std::rand() % l_iQuot;
        }
        else m_iSkills[i] = 0;

        printf(":%i=%-3i[%s]", 
          i, 
          m_iSkills[i], 
          i == (int)enSkill::OtherMarbleMode ? m_iSkills[i] < m_cAiData.m_iAvoid       ? "avo" : m_iSkills[i] < m_cAiData.m_iOvertake ? "ovt" : "def" : 
          i == (int)enSkill::JumpModeSwitch  ? m_iSkills[i] < m_cAiData.m_iJumpMode    ? "jm+" : "jm-" :
          i == (int)enSkill::JumpDirection   ? m_iSkills[i] < m_cAiData.m_iJumpDir     ? "jd+" : "jd-" :
          i == (int)enSkill::JumpVelocity    ? m_iSkills[i] < m_cAiData.m_iJumpVel     ? "jv+" : "jv-" :
          i == (int)enSkill::PathSelection   ? m_iSkills[i] < m_cAiData.m_iPathSelect  ? "ps+" : "ps-" :
          i == (int)enSkill::RoadBlock       ? m_iSkills[i] < m_cAiData.m_iRoadBlock   ? "rb+" : "rb-" : 
          i == (int)enSkill::BestJumpVel     ? m_iSkills[i] < m_cAiData.m_iBestJumpVel ? "bv+" : "bv-" : ""
        );
      }

      m_fJumpFact = (irr::f32)(std::rand() % 100) / 100.0f;
      printf(" | jvf: %.2f\n", m_fJumpFact);
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CControllerAi_V2::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (a_iMarbleId == m_iMarbleId) {
        m_iLastCheckpoint = a_iCheckpoint;
        m_iRespawn = 0;
        rollDice();

        if (m_eMode == enMarbleMode::Respawn2)
          switchMarbleMode(enMarbleMode::Cruise);

        if (m_pLuaScript != nullptr)
          m_pLuaScript->oncheckpoint(a_iMarbleId, a_iCheckpoint);
      }
    }

    /**
    * Search for "special" path lines, i.e. jumps or blocks
    * @param a_pInput the path end
    * @return the first "special" path line in the list, nullptr if nothing special was found
    */
    CControllerAi_V2::SPathLine2d *CControllerAi_V2::findNextSpecial(SPathLine2d* a_pInput) {
      if (a_pInput->m_pParent->m_pParent->m_eType != scenenodes::CAiPathNode::Default) {
        return a_pInput;
      }

      if (a_pInput->m_vNext.size() > 0) {
        SPathLine2d *l_pRet = findNextSpecial(*a_pInput->m_vNext.begin());
        if (l_pRet != nullptr)
          return l_pRet;
        else
          return nullptr;
      }
      else return nullptr;
    }

    /**
    * Get the processes the next special path line, i.e. a jump or block. It calculates if
    * the line is in reach and changes the marble mode if appropriate
    * @param a_pSpecial the special path line
    * @param a_fVel the current calculated velocity
    */
    void CControllerAi_V2::processNextSpecial(SPathLine2d *a_pSpecial, irr::f32 a_fVel) {
      if (a_pSpecial != nullptr) {
        // We have found a jump
        if (a_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump || a_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Loop) {
          // Now we check for the segment after the jump, to see if we need to switch to jump mode
          // we create two lines from our position to the borders of the end of the segment and see
          // if these lines do not cross any border (therefore the 0.1 * limitation) we are ready to jump
          irr::core::line2df l_cLine1 = irr::core::line2df(irr::core::vector2df(), a_pSpecial->m_cLines[1].end);
          irr::core::line2df l_cLine2 = irr::core::line2df(irr::core::vector2df(), a_pSpecial->m_cLines[2].end);

          l_cLine1.end = l_cLine1.end - 0.1f * (l_cLine1.end - l_cLine1.start);
          l_cLine2.end = l_cLine2.end - 0.1f * (l_cLine2.end - l_cLine2.start);

          bool l_bCollide1 = doesLineCollide(l_cLine1, m_p2dPath);
          bool l_bCollide2 = doesLineCollide(l_cLine2, m_p2dPath);

          // Mark: we also need to check if the jump if in front of use, i.e. the Y part of the 2d coordinate is negatve
          if (l_cLine1.end.Y < 0.0f && l_cLine2.end.Y < 0.0f && !doLinesCollide(l_cLine1, l_cLine2, m_p2dPath)) {
            switchMarbleMode(a_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump ? enMarbleMode::Jump : enMarbleMode::Loop);
          }
          else 
            if (m_eMode == enMarbleMode::Jump)
              switchMarbleMode(enMarbleMode::Default);
        }
        else {
          // No jump ahead but still jumping? Switch to default
          if (m_eMode == enMarbleMode::Jump)
            switchMarbleMode(enMarbleMode::Default);
        }

        // A blocker is ahead
        if (a_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Block) {
          // First we check if we would reach the blocker within a second
          irr::f32 l_fDist = a_pSpecial->m_cLines[0].start.getDistanceFrom(irr::core::vector2df());
          if (l_fDist < a_fVel) {
            printf("%s (%i)\n", m_pLuaScript != nullptr ? "Script" : "Nope", a_pSpecial->m_pParent->m_pParent->m_iTag);
            // If so we need to ask the LUA script on whether or not the blocker is currently blocking the road and...
            if (m_pLuaScript != nullptr && (m_cAiData.m_iRoadBlock >= 100 || m_iSkills[(int)enSkill::RoadBlock] < m_cAiData.m_iRoadBlock)) {
              int l_bBlock = m_pLuaScript->decide_blocker(m_iMarbleId, a_pSpecial->m_pParent->m_pParent->m_iTag);

              // ... if so we set the veclcity to zero, overriding any other calculation
              if (!l_bBlock) {
                printf("Stop!\n");
                m_fVCalc = 0.0f;
              }
            }
          }
        }
      }
      else {
        // No jump ahead but still in jump or loop mood? Switch to default
        if (m_eMode == enMarbleMode::Jump || m_eMode == enMarbleMode::Loop)
          switchMarbleMode(enMarbleMode::Default);
      }
    }

    /**
    * Determine whether or not a line collides with an AI path line
    * @param a_cLine the line to verify
    * @param a_pPath the path to check against
    * @return true if there is a collision, false otherwise
    */
    bool CControllerAi_V2::doesLineCollide(const irr::core::line2df& a_cLine, SPathLine2d* a_pPath) {
      irr::core::vector2df v;

      if (a_pPath->m_cLines[1].intersectWith(a_cLine, v) || a_pPath->m_cLines[2].intersectWith(a_cLine, v))
        return true;

      if (a_pPath->m_vNext.size() > 0) {
        return doesLineCollide(a_cLine, *a_pPath->m_vNext.begin());
      }

      return false;
    }

    /**
    * Check if one of the two given lines collide with the AI path borders
    * @param a_cLine1 the first line to verify
    * @param a_cLine2 the second line to verify
    * @param a_pPath the path to check against
    * @return true if there is a collision, false otherwise
    */
    bool CControllerAi_V2::doLinesCollide(const irr::core::line2df& a_cLine1, const irr::core::line2df& a_cLine2, SPathLine2d* a_pPath) {
      irr::core::vector2df v;

      if (a_pPath->m_cLines[1].intersectWith(a_cLine1, v) || a_pPath->m_cLines[2].intersectWith(a_cLine1, v) ||
          a_pPath->m_cLines[1].intersectWith(a_cLine2, v) || a_pPath->m_cLines[2].intersectWith(a_cLine2, v))
        return true;

      if (a_pPath->m_vNext.size() > 0) {
        return doLinesCollide(a_cLine1, a_cLine2, *a_pPath->m_vNext.begin());
      }

      return false;
    }

    /**
    * Switch this AI marble to another mode
    * @param a_eMode the new mode
    */
    void CControllerAi_V2::switchMarbleMode(enMarbleMode a_eMode) {
      if ((m_cAiData.m_iModeMap & (int)data::SMarbleAiData::enAiMode::TimeAttack) == 0) {
        if (a_eMode == enMarbleMode::TimeAttack) {
          switchMarbleMode(enMarbleMode::Default);
          return;
        }
      }

      if ((m_cAiData.m_iModeMap & (int)data::SMarbleAiData::enAiMode::Cruise) == 0) {
        if (a_eMode == enMarbleMode::Default) {
          m_eMode = enMarbleMode::Cruise;
          return;
        }
      }

      if (a_eMode != m_eMode) {
        bool a_bSucceed = true;

        if (a_eMode == enMarbleMode::Jump) {
          a_bSucceed = m_iSkills[(int)enSkill::JumpModeSwitch] < m_cAiData.m_iJumpMode;
        }

        if (a_eMode == enMarbleMode::OffTrack) {
          // If we are in a loop we ignore off-track
          a_bSucceed = m_eMode != enMarbleMode::Loop;
        }

        if (a_bSucceed)
          m_eMode = a_eMode;
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
    * A marble has finished
    * @param a_iMarbleId ID of the finished marble
    */
    void CControllerAi_V2::onPlayerFinished(irr::s32 a_iMarbleId) {
      m_vFinished.push_back(a_iMarbleId);
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
    bool CControllerAi_V2::getControlMessage(
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

      // If we do not yet know where we are we have a look
      if (m_pCurrent == nullptr) {
        m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, m_iLastCheckpoint == -1, true);

        if (m_pCurrent == nullptr && m_iLastCheckpoint == -1)
          m_pCurrent = selectClosest(m_aMarbles[m_iIndex].m_cPosition, m_vAiPath, false, true);
      }

      if (m_pCurrent != nullptr) {
        // Update the current path section if necessary
        m_pCurrent = selectCurrentSection(m_aMarbles[m_iIndex].m_cPosition, m_pCurrent, false);

        if (m_pCurrent != nullptr) {
          irr::f32 l_fVel = m_cVelocity2d.getLength();
          m_fVCalc = -1.0f;

          if (m_eMode == enMarbleMode::Loop && l_fVel < 30.0f)
            switchMarbleMode(enMarbleMode::Respawn);

          // transformed positions (tuple index 1) and velocities (tuple index 2) with the IDs (tuple index 0) of the marbles
          std::vector<std::tuple<int, irr::core::vector3df, irr::core::vector3df>> l_vMarblePosVel;

          SPathLine2d *l_pSpecial = nullptr;

          if (m_pCurrent->m_pAiPath != nullptr) {
            // Create a matrix to transform the ai data to 2d
            irr::core::matrix4 l_cMatrix;
            l_cMatrix = l_cMatrix.buildCameraLookAtMatrixRH(m_aMarbles[m_iIndex].m_cPosition + m_pCurrent->m_cNormal, m_aMarbles[m_iIndex].m_cPosition, m_aMarbles[m_iIndex].m_cDirection);

            std::vector<const data::SMarblePosition *> l_vMarbles;
            for (int i = 0; i < 16; i++) {
              if (m_aMarbles[i].m_iMarbleId != -1 && m_aMarbles[i].m_iMarbleId != m_iMarbleId)
                l_vMarbles.push_back(&m_aMarbles[i]);
            }

            // Transform the data
            m_p2dPath = m_pCurrent->m_pAiPath->transformTo2d(
              l_cMatrix, 
              m_mSplitSelections[m_iSplitIndex],
              m_mSplitSelections[m_iSplitIndex == 0 ? 1 : 0],
              l_vMarbles, 
              l_vMarblePosVel,
              m_cAiData.m_iPathSelect >= 100 || m_iSkills[(int)enSkill::PathSelection] < m_cAiData.m_iPathSelect ? m_pLuaScript : nullptr
            );

            m_iSplitIndex = m_iSplitIndex == 0 ? 1 : 0;
            m_mSplitSelections[m_iSplitIndex].clear();

            // Transform our velocity to 2d
            irr::core::vector3df l_vDummy = m_aMarbles[m_iIndex].m_cPosition + m_aMarbles[m_iIndex].m_cVelocity;
            l_cMatrix.transformVect(l_vDummy);

            m_cVelocity2d.X = l_vDummy.X;
            m_cVelocity2d.Y = l_vDummy.Y;

            // Search for special path segments , i.e. jumps or block
            if (m_p2dPath->m_vNext.size() > 0)
              l_pSpecial = findNextSpecial(*m_p2dPath->m_vNext.begin());

            if (m_p2dPath->m_cLines[1].getPointOrientation(irr::core::vector2df()) > 0 ==  m_p2dPath->m_cLines[2].getPointOrientation(irr::core::vector2df()) > 0) {
              // If we detected that we are off track (the orientation of both border lines of the segment to our position is the same, i.e. if we are on the track we
              // have one border on the left and the other one on the right) and we are currently not requesting a respawn we switch the mode to "off-track"
              if (m_eMode != enMarbleMode::Respawn && m_eMode != enMarbleMode::Respawn2)
                switchMarbleMode(enMarbleMode::OffTrack);
            }
            else {
              if (l_pSpecial != nullptr) {
                processNextSpecial(l_pSpecial, l_fVel);
              }
              else {
                // No jump ahead but still in jump mood? Switch to default
                if (m_eMode == enMarbleMode::Jump || m_eMode == enMarbleMode::Loop)
                  switchMarbleMode(enMarbleMode::Default);
              }

              // If we are still in off-track mood but have detected that we are
              // back on track we switch the mode to "default"
              if (m_eMode == enMarbleMode::OffTrack)
                switchMarbleMode(enMarbleMode::Cruise);
              else if (m_eMode != enMarbleMode::Jump && m_eMode != enMarbleMode::Loop) {
                // if we are not currently jumping we use the ranking to see if we
                // can switch the mode. To do: make this dependent of the class and marble

                if (m_iMyPosition > 1) {
                  // If we are in lead we just use our advantage on the next
                  // marble to switch the ode
                  if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead > 0) {
                    if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead < 360)
                      switchMarbleMode(enMarbleMode::TimeAttack);
                    else if (m_aRacePositions[m_iMyPosition - 1].m_iDeficitAhead < 480)
                      switchMarbleMode(enMarbleMode::Default);
                    else
                      switchMarbleMode(enMarbleMode::Cruise);
                  }
                }
                else if (m_iMyPosition == 1) {
                  // If we are not in lead we just look at the marble ahead
                  if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead > 0) {
                    if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead < 240)
                      switchMarbleMode(enMarbleMode::TimeAttack);
                    else if (m_aRacePositions[m_iMyPosition].m_iDeficitAhead < 360)
                      switchMarbleMode(enMarbleMode::Default);
                    else
                      switchMarbleMode(enMarbleMode::Cruise);
                  }
                }
              }
            }
          }        

          irr::f32             l_fClosest = 0.0f;
          irr::core::vector2df l_cCloseMb;
          irr::core::vector2df l_cCloseSp;

          for (auto& l_cMarble : l_vMarblePosVel) {
            if (std::find(m_vFinished.begin(), m_vFinished.end(), std::get<0>(l_cMarble)) == m_vFinished.end()) {
              irr::core::vector2df v = irr::core::vector2df(std::get<1>(l_cMarble).X, std::get<1>(l_cMarble).Y);

              if (v.Y < 0.0f) {
                irr::f32 f = v.getLength();
                if (l_fClosest == 0.0f || f < l_fClosest) {
                  l_cCloseMb = v;
                  l_fClosest = f;
                  l_cCloseSp = irr::core::vector2df(std::get<2>(l_cMarble).X, std::get<2>(l_cMarble).Y);
                }
              }
            }
          }

          if (m_pDebugRTT != nullptr) {
            // If debugging is active we paint the two calculated lines the determine how we move
            m_pDrv->setRenderTarget(m_pDebugRTT, true, false);
            if (l_pSpecial != nullptr) {
              if (l_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump || l_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Loop) {
                irr::core::line2df l_cLine1 = irr::core::line2df(irr::core::vector2df(), l_pSpecial->m_cLines[1].end);
                irr::core::line2df l_cLine2 = irr::core::line2df(irr::core::vector2df(), l_pSpecial->m_cLines[2].end);

                l_cLine1.end = l_cLine1.end - 0.1f * (l_cLine1.end - l_cLine1.start);
                l_cLine2.end = l_cLine2.end - 0.1f * (l_cLine2.end - l_cLine2.start);

                bool l_bCollide1 = doesLineCollide(l_cLine1, m_p2dPath);
                bool l_bCollide2 = doesLineCollide(l_cLine2, m_p2dPath);

                draw2dDebugLine(l_cLine1, m_fScale, l_bCollide1 ? irr::video::SColor(0xFF, 0xFF, 0x80, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);
                draw2dDebugLine(l_cLine2, m_fScale, l_bCollide2 ? irr::video::SColor(0xFF, 0xFF, 0x80, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);
              }
            }
            if (l_fClosest != 0.0f && m_pDebugRTT != nullptr) {
              draw2dDebugRectangle(l_cCloseMb, irr::video::SColor(0xFF, 0, 0xFF, 0), 15, m_fScale, m_cOffset);
            }
          }

          

          if (m_p2dPath != nullptr) {
            calculateControlMessage(a_iCtrlX, a_iCtrlY, a_bBrake, a_bRearView, a_bRespawn, a_eMode, l_pSpecial, l_cCloseMb, l_cCloseSp);
          }

          if (m_pDebugRTT != nullptr) {
            irr::core::line2df l_cVelocityLine = irr::core::line2df(irr::core::vector2df(), m_cVelocity2d);

            draw2dDebugLine(l_cVelocityLine, m_fScale, irr::video::SColor(0xFF, 0, 0xFF, 0), m_cOffset);
            draw2dDebugRectangle(irr::core::vector2df(0.0f), irr::video::SColor(0xFF, 0, 0, 0xFF), 10, m_fScale, m_cOffset);

            std::wstring s; 

            switch (m_eMode) {
              case enMarbleMode::OffTrack:
                s = L"Ai Mode: Off Track";
                break;

              case enMarbleMode::Default:
                s = L"Ai Mode: Default";
                break;

              case enMarbleMode::Cruise:
                s = L"Ai Mode: Cruise";
                break;

              case enMarbleMode::TimeAttack:
                s = L"Ai Mode: Time Attack";
                break;
     
              case enMarbleMode::Jump:
                s = L"Ai Mode: Jump";
                break;

              case enMarbleMode::Loop:
                s = L"Ai Mode: Loop";
                break;

              case enMarbleMode::Respawn:
                s = L"Ai Mode: Respawn";
                break;

              case enMarbleMode::Respawn2:
                s = L"Ai Mode: Respawn2";
                break;
            }
            draw2dDebugText(s.c_str(), m_pFont, irr::core::vector2df());
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
        m_cRttSize = irr::core::dimension2du(m_cViewport.getWidth(), m_cViewport.getHeight());

        m_cRttSize.Width  /= 3;
        m_cRttSize.Height /= 3;

        m_fScale = ((irr::f32)m_cRttSize.Height) / 1000.0f;
        m_fScale = 1.0f / m_fScale;

        m_pDebugRTT = m_pDrv->getTexture((std::string("ai_debug_rtt_") + std::to_string(m_iMarbleId)).c_str());

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
    * @param a_cPoint [out] the 3d point of the AI calculation
    */
    bool CControllerAi_V2::getBestLine(irr::core::line2df& a_cLine, SPathLine2d* a_pEnd, SPathLine2d **a_pCollide) {
      while (a_pEnd != nullptr) {
        bool l_bReturn = true;
        a_cLine.end = a_pEnd->m_cLines[0].end;

        SPathLine2d *l_pOther = a_pEnd;
        while (l_pOther != nullptr) {
          // In every step we move the end of the line to check
          // one section back until no collision happens
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

      a_cLineOne.end = m_p2dPath->m_cLines[0].getClosestPoint(irr::core::vector2df());

      a_cLineTwo = irr::core::line2df(a_cLineOne.end, a_cLineOne.end);
      if (getBestLine(a_cLineTwo, a_pPath, nullptr)) {
        l_iRet++;
      }

      return l_iRet;
    }

    /**
    * Do the actual control message calculation from the 2d path
    * @param a_iCtrlX [out] the steering value 
    * @param a_iCtrlY [out] the throttle value
    * @param a_bBrake [out] is the brake active?
    * @param a_bRearView [out] does the marble look to the back?
    * @param a_bRespawn [out] does the marble want a manual respawn?
    * @param a_eMode [out] the AI mode the marble is currently in
    */
    void CControllerAi_V2::calculateControlMessage(
      irr::s8& a_iCtrlX,
      irr::s8& a_iCtrlY,
      bool& a_bBrake,
      bool& a_bRearView,
      bool& a_bRespawn,
      enMarbleMode& a_eMode,
      SPathLine2d *a_pSpecial,
      const irr::core::vector2df &a_cCloseMb,
      const irr::core::vector2df &a_cCloseSp
    ) {
      if (m_pDebugRTT != nullptr)
        m_p2dPath->debugDraw(m_pDrv, m_cOffset, 2.0f);

      irr::core::vector2df l_cClosest = m_p2dPath->m_cLines[0].getClosestPoint(irr::core::vector2df(0.0f));

      irr::f32 l_fFactor = (irr::core::line2df(m_p2dPath->m_cLines[0].end, l_cClosest).getLengthSQ() / m_p2dPath->m_cLines[0].getLengthSQ());

      l_fFactor = std::max(0.15f, l_fFactor);

      // Now we search for the possible ends of the path
      std::vector<SPathLine2d *> l_vEnds;
      findEnds(l_vEnds, m_p2dPath, 0.0f, l_fFactor);

      int l_iLines = 0;

      irr::core::line2df l_cVelocityLine = irr::core::line2df(irr::core::vector2df(), m_cVelocity2d);
      irr::f32 l_fVel = m_cVelocity2d.getLength();

      for (std::vector<SPathLine2d*>::iterator l_itEnd = l_vEnds.begin(); l_itEnd != l_vEnds.end(); l_itEnd++) {
        irr::core::line2df l_cLine  = irr::core::line2df(irr::core::vector2df(), irr::core::vector2df());
        irr::core::line2df l_cOther;

        // Now we calculate the control lines depending on the mode we are in
        switch (m_eMode) {
          case enMarbleMode::OffTrack: {
            // Off-track: get back on the track as fast as possible
            l_iLines = getControlLines_Offtrack(l_cLine, l_cOther, nullptr);
            irr::core::vector2df v = m_p2dPath->m_cLines[0].getClosestPoint(l_cLine.start);
            irr::f32 l_fDist      = v.getDistanceFrom(l_cLine.start);
            irr::f32 l_fThreshold = m_eMode == enMarbleMode::Jump ? 0.25f : 2.5f;
            if (l_fDist > l_fThreshold * m_p2dPath->m_fWidth)
              m_eMode = enMarbleMode::Respawn;
            break;
          }

          case enMarbleMode::Default:
            l_iLines = getControlLines_Default(l_cLine, l_cOther, *l_itEnd);
            break;

          case enMarbleMode::Respawn2:
            l_iLines = getControlLines_Offtrack(l_cLine, l_cOther, *l_itEnd);
            if (l_cLine.getLength() < 0.75f * m_p2dPath->m_fWidth)
              switchMarbleMode(enMarbleMode::Cruise);
            break;

          case enMarbleMode::Cruise:
            l_iLines = getControlLines_Cruise(l_cLine, l_cOther, *l_itEnd);
            break;

          case enMarbleMode::TimeAttack:
          case enMarbleMode::Loop:
            l_iLines = getControlLines_TimeAttack(l_cLine, l_cOther, *l_itEnd);
            break;

          case enMarbleMode::Jump: {
            // In jump mode we just use the direction the jump is pointing
            // to and use that together with the (possibly) defined speed for controls
            l_iLines = 2;
            l_cLine.start = irr::core::vector2df();

            if (a_pSpecial != nullptr && a_pSpecial->m_pParent->m_pParent->m_eType == scenenodes::CAiPathNode::enSegmentType::Jump) {
              irr::core::vector2df l_cDirection = a_pSpecial->m_cLines[0].end - a_pSpecial->m_cLines[0].start;
              l_cDirection = l_cDirection.normalize();

              irr::f32 l_fMin = a_pSpecial->m_pParent->m_pParent->m_fMinVel;
              irr::f32 l_fMax = a_pSpecial->m_pParent->m_pParent->m_fMaxVel;
              irr::f32 l_fBst = a_pSpecial->m_pParent->m_pParent->m_fBestVel;

              if (l_fMin <= 0.0f) l_fMin = l_fVel;
              if (l_fMax <= 0.0f) l_fMax = l_fVel * 1.25f;

              l_cLine .end   = l_fMin * l_cDirection;
              l_cOther.start = l_cLine.end;
              l_cOther.end   = l_cOther.start + l_fMax * l_cDirection;

              irr::core::line2df l_cJumpLine = irr::core::line2df(a_pSpecial->m_cLines[1].start, a_pSpecial->m_cLines[2].start);

              irr::core::vector2df v;

              // To do: some randomness and skill for the speed we are going to use
              if (l_fMax > 0.0f && l_cJumpLine.intersectWith(irr::core::line2df(irr::core::vector2df(), m_cVelocity2d), v) && (m_cAiData.m_iJumpVel >= 100 || m_iSkills[(int)enSkill::JumpVelocity] < m_cAiData.m_iJumpVel)) {

                if ((l_fBst != -1.0f && m_iSkills[(int)enSkill::BestJumpVel] < m_cAiData.m_iBestJumpVel) || m_cAiData.m_iBestJumpVel == 100)
                  m_fVCalc = l_fBst;
                else {
                  if (l_fMin == -1.0f)
                    m_fVCalc = l_fMax;
                  else if (l_fMax == -1.f)
                    m_fVCalc = l_fMin;
                  else {
                    m_fVCalc = l_fMin + ((l_fMax - l_fMin) * m_fJumpFact);
                  }
                }

                if (m_pDebugRTT != nullptr) {
                  draw2dDebugLine(l_cJumpLine, m_fScale, irr::video::SColor(0xFF, 0xFF, 0x80, 0), m_cOffset);
                }
              }
              else {
                l_cLine .end   = l_fVel * l_cDirection;
                l_cOther.start = l_cLine.end;
                l_cOther.end   = l_cOther.start + l_fMax * l_cDirection;

                if (m_pDebugRTT != nullptr) {
                  draw2dDebugLine(l_cJumpLine, m_fScale, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), m_cOffset);
                }
              }

              break;
            }
            else switchMarbleMode(enMarbleMode::Default);

            break;
          }

          case enMarbleMode::Respawn:
            // If we are in respawn mode we
            // just set the "respawn" flag to true
            a_bRespawn = true;
            break;
        }

        bool l_bOvertake = false;
        bool l_bCollide  = false;
        bool l_bOTBrake  = true;

        if (m_eMode != enMarbleMode::Jump) {
          if (m_iSkills[(int)enSkill::OtherMarbleMode] < m_cAiData.m_iAvoid) {
            // Avoid collisions
            irr::core::vector2df l_cCollision = l_cVelocityLine.getClosestPoint(a_cCloseMb);

            if (l_cCollision.Y < 0.0f) {
              irr::f32 l_fSideVal  = m_eAiMode == enAiMode::MarbleGP ? 1.75f : m_eAiMode == enAiMode::Marble2 ? 2.5f  : 3.5f;
              irr::f32 l_fVelFact  = m_eAiMode == enAiMode::MarbleGP ? 1.15f : m_eAiMode == enAiMode::Marble2 ? 1.65f : 2.0f;
              irr::f32 l_fColFact  = m_eAiMode == enAiMode::MarbleGP ?  0.8f : m_eAiMode == enAiMode::Marble2 ? 0.9f  : 1.0f;
              irr::f32 l_fMultiply = m_eAiMode == enAiMode::MarbleGP ?  0.9f : m_eAiMode == enAiMode::Marble2 ? 0.6f  : 0.5f;

              if ((l_cCollision - a_cCloseMb).getLength() <= l_fSideVal && a_cCloseSp.getLength() < l_fVelFact * l_fVel && l_cCollision.getLength() < l_fColFact * l_fVel) {
                m_fVCalc = l_fMultiply * a_cCloseSp.getLength();
                l_bCollide = true;
              }
              else if (abs(l_cCollision.X) < abs(l_cCollision.Y) && a_cCloseSp.getLength() < l_fVel) {
                m_fVCalc = l_fMultiply * a_cCloseSp.getLength();
                l_bCollide = true;
              }
            }
          }
          else if (m_iSkills[(int)enSkill::OtherMarbleMode] < m_cAiData.m_iOvertake) {
            irr::f32 l_fClosest = a_cCloseMb.getLength();

            // Overtake attempt
            if (l_fClosest != 0.0f && l_fClosest < 16.0f) {
              bool l_bPath = l_cLine.getPointOrientation(l_cOther.end) < 0.0f;
              bool l_bMrbl = l_cLine.getPointOrientation(l_cClosest  ) < 0.0f;

              l_bOvertake = true;
              l_cLine.end.X += l_bPath ? -5.0f : 5.0f;
              l_cOther.start = l_cLine.end;
              switchMarbleMode(enMarbleMode::TimeAttack);

              l_bOTBrake = !((l_bMrbl == l_bPath) && l_fClosest < 5.0f && l_bMrbl == l_bPath);
            }
          }
        }

        if (m_pDebugRTT != nullptr) {
          draw2dDebugLine(l_cLine, 2.0f, irr::video::SColor(255, 255, 0, 0), m_cOffset);

          if (l_iLines > 1)
            draw2dDebugLine(l_cOther, 2.0f, irr::video::SColor(0xFF, 0xFF, 0xFF, 0), m_cOffset);

          if (l_bCollide || l_bOvertake) {
            std::wstring s = l_bCollide ? L"C" : l_bOTBrake ? L"OB" : L"O";
            irr::core::dimension2du l_cDim = m_pFont->getDimension(s.c_str());
            irr::core::position2di  l_cPos = irr::core::position2di(0, m_pDebugRTT->getOriginalSize().Height - l_cDim.Height);
            m_pDrv->draw2DRectangle(l_bCollide ? irr::video::SColor(0xFF, 0xFF, 0, 0) : l_bOTBrake ? irr::video::SColor(0xFF, 0, 0xFF, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0), irr::core::recti(l_cPos, l_cDim));
            draw2dDebugText(s.c_str(), m_pFont, irr::core::vector2df((irr::f32)l_cPos.X, (irr::f32)l_cPos.Y));
          }
        }

        if (l_fVel > 5.0f) {
          // A reasonable speed has been calculated or we are in jump mode
          irr::f32 l_fCtrlLen = l_cLine.getLength();

          // Get the angle between the veclocity and the first control line,
          // use the angle to calculate a factor between 0.15 (angle == 90) and 1.15 (angle == 0)
          // to do: maybe change the "0.15" depending on the AI class and marble
          irr::f64 l_fAngle1 = l_cVelocityLine.getAngleWith(l_cLine);
          irr::f64 l_fFactor = 1.0 - ((l_fAngle1) / 90.0) + 0.15;

          // Calculate a point in the calculated distance on the first line
          irr::core::vector2df l_cPoint1 = (l_cLine.end - l_cLine.start).normalize() * l_fVel * (irr::f32)l_fFactor;

          irr::f64 l_fAngle2  = l_cLine.getAngleWith(l_cOther);
          irr::f64 l_fFactor2 = 1.0 - (l_fAngle2 / 90.0) + 0.15;

          // If the calculated point is the end of the line we move the point
          // by the remaining speed in the direction of the second control line
          if (l_cLine.getClosestPoint(l_cPoint1) == l_cLine.end) {
            l_cPoint1 = l_cOther.start + (l_cOther.end - l_cOther.start).normalize() * (irr::f32)l_fFactor2 * (l_fVel - l_fCtrlLen) * 1.25;
          }

          irr::f64 l_fAngle3 = l_cVelocityLine.getAngleWith(irr::core::line2df(irr::core::vector2df(), l_cPoint1));

          l_fFactor = l_fAngle3 / 80.0;

          if (m_fVCalc == -1.0f) {
            // We only calculate the speed if it has not yet been set, i.e.
            // we are not approaching a blocker
            irr::f64 l_fSpeedFactor = (1.0 - l_fFactor);

            // First we calculate a speed of up to 75, and ..
            m_fVCalc = (irr::f32)(l_fSpeedFactor * l_fSpeedFactor * 75.0);

            // .. if the speed factor is high enough we increase the speed
            // to up to 160. To do: maybe the 0.85 can be marble-class dependent
            if (l_fSpeedFactor > 0.85) {
              m_fVCalc += (irr::f32)(m_cAiData.m_fSpeedFactor1 * (l_fSpeedFactor / 0.85));
            }

            // Now we calculate another possible speed using
            // the second control line, same procedure as above
            irr::f64 l_fSpeedFact2 = 1.0 - l_fFactor2;
            irr::f64 l_fSpeed2 = std::max(15.0, l_fSpeedFact2 * l_fSpeedFact2 * m_cAiData.m_fSpeedFactor2);

            if (l_fSpeedFact2 > 0.85) {
              l_fSpeed2 += m_cAiData.m_fSpeedFactor2 * (l_fSpeedFact2 / (m_eAiMode == enAiMode::MarbleGP ? 0.85f : m_eAiMode == enAiMode::Marble2 ? 0.95f  : 1.0f));
            }

            // if we are fast enough we use this second
            // calculated speed
            if (l_fVel > m_cAiData.m_fSpeedThreshold * l_cLine.getLength()) {
              m_fVCalc = (irr::f32)l_fSpeed2;
            }

            if (l_bOvertake)
              m_fVCalc *= 1.05f;
          }

          if (m_fVCalc > l_fVel) {
            // if the calculated speed is greater than
            // our current speed we accelerate ..
            if (m_eMode == enMarbleMode::Loop)
              // If we are in a loop we do not take the throttle add parameter into account
              m_fCtrlY = 1.0f;
            else
              m_fCtrlY = std::min(1.0f, m_fCtrlY + m_cAiData.m_fThrottleAdd);

            a_iCtrlY = (irr::s8)(127.0f * m_fCtrlY);
          }
          else {
            // .. otherwise we decelerate and if the
            // wanted speed is either 5.0 below the
            // current speed or the wanted speed is
            // zero we activate the brake
            a_iCtrlY = -127;
            a_bBrake = l_bOTBrake && (std::abs(l_fVel - m_fVCalc) > 5.0f || m_fVCalc == 0.0f);
            m_fCtrlY = 0.0f;
          }

          irr::core::vector2df l_cPos = irr::core::vector2df();

          // Modify thresholds for marble classes??
          irr::f64 l_fThreshold = m_eMode == enMarbleMode::Jump ? 0.25 : 2.5;

          if (m_eMode == enMarbleMode::Jump) {
            int l_iSkillJump = m_iSkills[(int)enSkill::JumpDirection];

            if (m_cAiData.m_iJumpDir >= 100 || l_iSkillJump < m_cAiData.m_iJumpDir)
              l_fThreshold = 0.25;
            else {
              irr::f32 l_fFactor = (irr::f32)(m_cAiData.m_iJumpDir - l_iSkillJump) / (irr::f32)m_cAiData.m_iJumpDir;
              l_fThreshold = 0.25 + 2.0f * abs(l_fFactor);
            }
          }

          irr::s16 l_iCtrlX = 0;

          // If the factor and the angle calculated above to get the actual steering. If the current angle is small (2.5) and the velocity line is not on the same
          // side as the end of the second control line we do not steer to avoind chattering
          l_iCtrlX = (std::abs(l_fAngle3) > l_fThreshold || (l_cOther.end.X > 0.0f != m_cVelocity2d.X > 0.0f)) ? l_fFactor > 0.05 ? 127 : (irr::s8)(127.0 * (l_fFactor + 0.25)) : 0;

          // Not determine if we need to steer left or right
          if (l_cPoint1.X < 0.0f)
            l_iCtrlX = -l_iCtrlX;

          // Again do something agains chattering by using the
          // angular velocity to see if we can stop steering
          if (m_fOldAngle != 0.0) {
            irr::f64 l_fTurnSpeed = m_fOldAngle - l_fAngle3;
            if (l_fAngle3 - 3.0 * l_fTurnSpeed < 0.0)
              l_iCtrlX = 0;
          }

          // Fill the parameter for steering input
          a_iCtrlX = (irr::s8)(std::max((irr::s16)-127, std::min((irr::s16)127, l_iCtrlX)));

          if (m_eMode == enMarbleMode::Jump) {
            // We limit our steering amount if we are currently jumping
            if (!m_aMarbles[m_iIndex].m_bContact) {
              if (doesLineCollide(l_cVelocityLine, m_p2dPath)) {
                a_iCtrlX = a_iCtrlX > 64 ? 64 : a_iCtrlX < -64 ? -64 : a_iCtrlX;
              }
              else {
                a_iCtrlX = 0;
              }
            }
          }

          // If we are going the wrong direction
          // we do full acceleration back
          if (l_cPoint1.Y > 0.0)
            a_iCtrlY = -127;

          m_fOldAngle = l_fAngle3;
        }
        else if (m_fVCalc == 0.0f) {
          // We are approaching a blocker?
          // just stop
          a_iCtrlY = 0;
          a_bBrake = true;
        }
        else {
          // If we have a low
          // speed we do some
          // acceleration
          a_iCtrlY = 127;
        }
      }
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

      irr::core::vector3df m1 = l1.getMiddle();
      irr::core::vector3df m2 = l2.getMiddle();
      irr::core::vector3df m3 = l3.getMiddle();

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
    * @param a_pMarbles the marble data (for debugging)
    * @param a_iMarbleId ID of the AI controlled marble (for debugging)
    */
    CControllerAi_V2::SPathLine3d *CControllerAi_V2::SAiPathSection::prepareTransformedData(irr::f32 a_fLength, SPathLine3d *a_pPrevious, data::SMarblePosition *a_pMarbles, int a_iMarbleId) {
      SPathLine3d *p = a_pPrevious;

      while (p != nullptr) {
        if (m_iIndex == p->m_iSectionIndex) {
          return nullptr;
        }
        p = p->m_pPrevious;
      }

      SPathLine3d *l_pThis = new SPathLine3d(m_cLine3d, m_cEdges[0], m_cEdges[1]);

      l_pThis->m_cBBox.reset(m_cLine3d.start);
      l_pThis->m_cBBox.addInternalPoint(m_cLine3d.end);
      l_pThis->m_cBBox.addInternalPoint(m_cLine3d.start + 1.0f * m_cNormal);
      l_pThis->m_cBBox.addInternalPoint(m_cLine3d.end   + 1.0f * m_cNormal);

      for (int i = 0; i < 2; i++) {
        l_pThis->m_cBBox.addInternalPoint(m_cEdges[i].start);
        l_pThis->m_cBBox.addInternalPoint(m_cEdges[i].end);
        l_pThis->m_cBBox.addInternalPoint(m_cEdges[i].start + 1.0f * m_cNormal);
        l_pThis->m_cBBox.addInternalPoint(m_cEdges[i].end   + 1.0f * m_cNormal);
      }
      
      if (a_pPrevious != nullptr)
        a_pPrevious->m_vNext.push_back(l_pThis);

      l_pThis->m_pPrevious     = a_pPrevious;
      l_pThis->m_iSectionIndex = m_iIndex;
      l_pThis->m_pParent       = this;

      l_pThis->m_cPathLine.m_cOriginal = m_cLine3d;
      l_pThis->m_cPathLine.m_pMarbles  = a_pMarbles;
      l_pThis->m_cPathLine.m_iMarbleId = a_iMarbleId;

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
            SPathLine3d *l_pNew = (*l_itNext)->prepareTransformedData(a_fLength, l_pThis, a_pMarbles, a_iMarbleId);
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
    * @param a_pMarbles the marble data (for debugging)
    * @param a_iMarbleId ID of the AI controlled marble (for debugging)
    */
    void CControllerAi_V2::SAiPathSection::fillLineVectors(data::SMarblePosition *a_pMarbles, int a_iMarbleId) {
      m_pAiPath = prepareTransformedData(0.0f, nullptr, a_pMarbles, a_iMarbleId);
    }

    bool CControllerAi_V2::SPathLine2d::doesLineIntersectBorder(const irr::core::line2df& a_cLine) {
      irr::core::vector2df v;
      for (int i = 1; i < 3; i++)
        if (m_cLines[i].intersectWith(a_cLine, v))
          return true;

      return false;
    }

    /**
    * Create a clone of this path line and all follow-ups
    */
    CControllerAi_V2::SPathLine2d* CControllerAi_V2::SPathLine2d::clone() {
      SPathLine2d *l_pClone = new SPathLine2d(*this);

      for (std::vector<SPathLine2d *>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++) {
        SPathLine2d *l_pNext = (*l_itNext)->clone();
        l_pNext->m_pPrevious = l_pClone;
        l_pClone->m_vNext.push_back(l_pNext);
      }

      return l_pClone;
    }

    /**
    * Delete all children
    */
    void CControllerAi_V2::SPathLine2d::deleteAllChildren() {
      while (m_vNext.size() > 0) {
        (*m_vNext.begin())->deleteAllChildren();
        SPathLine2d *p = *m_vNext.begin();
        m_vNext.erase(m_vNext.begin());
        delete p;
      }
    }

    bool CControllerAi_V2::SPathLine2d::getFirstCollisionLine(const irr::core::line2df& a_cLine, irr::core::vector2df& a_cOutput, bool a_bBordersOnly, bool& a_bIsCenterLine) {
      bool l_bRet = false;
      a_bIsCenterLine = false;
      
      irr::core::vector2df l_cIntersect[3] = {
        irr::core::vector2df(-1.0f, -1.0f),
        irr::core::vector2df(-1.0f, -1.0f),
        irr::core::vector2df(-1.0f, -1.0f)
      };

      for (int i = 1; i < 3; i++) {
        if (!m_cLines[i].intersectWith(a_cLine, l_cIntersect[i])) {
          l_cIntersect[i].X = -1.0f;
          l_cIntersect[i].Y = -1.0f;
        }
        else l_bRet = true;
      }

      if (l_bRet) {
        int l_iIndex = 0;
        irr::f32 l_fDist = 0.0f;

        for (int i = a_bBordersOnly ? 1 : 0; i < 3; i++) {
          if (l_cIntersect[i].X != -1.0f && l_cIntersect[i].Y != -1.0f) {
            if (l_fDist == 0.0f || l_fDist > l_cIntersect[i].getLengthSQ()) {
              l_iIndex = i;
              l_fDist = l_cIntersect[i].getLengthSQ();
            }
          }
        }

        a_bIsCenterLine = l_iIndex == 0;

        a_cOutput = l_cIntersect[l_iIndex];
      }
      else {
        for (std::vector<SPathLine2d*>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end() && !l_bRet; l_itNext++) {
          l_bRet = (*l_itNext)->getFirstCollisionLine(a_cLine, a_cOutput, a_bBordersOnly, a_bIsCenterLine);
        }
      }

      return l_bRet;
    }



    CControllerAi_V2::SPathLine2d::SPathLine2d() : m_fWidth(0.0f), m_iMarbleId(-1), m_pMarbles(nullptr), m_pParent(nullptr), m_pPrevious(nullptr) {
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(const SPathLine2d& a_cOther) : m_fWidth(a_cOther.m_fWidth), m_iMarbleId(a_cOther.m_iMarbleId), m_pMarbles(a_cOther.m_pMarbles), m_pParent(a_cOther.m_pParent), m_pPrevious(nullptr) {
      for (int i = 0; i < 3; i++)
        m_cLines[i] = a_cOther.m_cLines[i];
    }

    CControllerAi_V2::SPathLine2d::SPathLine2d(irr::core::line2df& a_cLine1, irr::core::line2df& a_cLine2, irr::core::line2df& a_cLine3) : m_fWidth((a_cLine2.start - a_cLine3.start).getLength()), m_iMarbleId(-1), m_pMarbles(nullptr), m_pParent(nullptr), m_pPrevious(nullptr) {
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
      irr::core::vector2df l_cOffset = irr::core::vector2df((irr::f32)a_cOffset.X, (irr::f32)a_cOffset.Y);

      for (int i = 0; i < 3; i++)
        a_pDrv->draw2DLine(
          irr::core::vector2di((irr::s32)(a_fScale * m_cLines[i].start.X) + a_cOffset.X, (irr::s32)(a_fScale * m_cLines[i].start.Y) + a_cOffset.Y),
          irr::core::vector2di((irr::s32)(a_fScale * m_cLines[i].end  .X) + a_cOffset.X, (irr::s32)(a_fScale * m_cLines[i].end  .Y) + a_cOffset.Y),
          i == 0 ? irr::video::SColor(0xFF, 128, 128, 128) : irr::video::SColor(0xFF, 0, 0, 0xFF)
        );

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
          m_cLines[i].start = l_cOut - 0.05f * (m_cLines[i].end - m_cLines[i].start).normalize();

        if (a_cPlane.getIntersectionWithLine(m_cLines[i].end, a_cNormal, l_cOut))
          m_cLines[i].end = l_cOut + 0.05f * (m_cLines[i].end - m_cLines[i].start).normalize();
      }

      for (std::vector<SPathLine3d *>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++)
        (*l_itNext)->transformLinesToPlane(a_cPlane, a_cNormal);
    }

    /**
    * Create 2d path lines out of the list of 3d path lines
    * @param a_cMatrix the camera matrix to use for the transformation
    * @param a_mSplitSelections a map with all the already selected directions on road splits
    * @param a_mOldSelections the map with the selections of the previous step
    * @param a_vMarbles the current positions of the marbles
    * @param a_vMarblePosVel [out] transformed positions (tuple index 0) and velocities (tuple index 1) of the marbles
    */
    CControllerAi_V2::SPathLine2d *CControllerAi_V2::SPathLine3d::transformTo2d(
      const irr::core::matrix4 &a_cMatrix, 
      std::map<int, SPathLine3d *> &a_mSplitSelections, 
      std::map<int, SPathLine3d *> &a_mOldSelections,
      std::vector<const data::SMarblePosition *> &a_vMarbles,
      std::vector<std::tuple<int, irr::core::vector3df, irr::core::vector3df>> &a_vMarblePosVel,
      lua::CLuaScript_ai *a_pLuaScript
    ) {
      m_cPathLine.m_vNext.clear();

      for (int i = 0; i < 3; i++) {
        irr::core::vector3df vs;
        irr::core::vector3df ve;

        a_cMatrix.transformVect(vs, m_cLines[i].start);
        a_cMatrix.transformVect(ve, m_cLines[i].end  );

        m_cPathLine.m_cLines[i] = irr::core::line2df(vs.X, vs.Y, ve.X, ve.Y);
      }

      for (std::vector<const data::SMarblePosition *>::iterator l_itMarble = a_vMarbles.begin(); l_itMarble != a_vMarbles.end(); l_itMarble++) {
        if (m_cBBox.isPointInside((*l_itMarble)->m_cPosition)) {
          irr::core::vector3df l_cPos = (*l_itMarble)->m_cPosition;
          irr::core::vector3df l_cVel = (*l_itMarble)->m_cVelocity + l_cPos;
          
          a_cMatrix.transformVect(l_cPos);
          a_cMatrix.transformVect(l_cVel);

          a_vMarblePosVel.push_back(std::make_tuple((*l_itMarble)->m_iMarbleId, l_cPos, l_cVel));
        }
      }

      m_cPathLine.m_fWidth  = (m_cPathLine.m_cLines[1].start - m_cPathLine.m_cLines[2].start).getLength();
      m_cPathLine.m_cMatrix = a_cMatrix;

      if (m_vNext.size() == 1) {
        SPathLine2d *l_pChild = (*m_vNext.begin())->transformTo2d(a_cMatrix, a_mSplitSelections, a_mOldSelections, a_vMarbles, a_vMarblePosVel, a_pLuaScript);
        l_pChild->m_pPrevious = &m_cPathLine;
        m_cPathLine.m_vNext.push_back(l_pChild);
      }
      else if (m_vNext.size() > 1) {
        int l_iTag = (*this->m_pParent->m_vNext.begin())->m_iTag;
        int l_iSplit = -1;

        if (a_pLuaScript != nullptr) {
          l_iSplit = a_pLuaScript->decide_roadsplit(-1, (*this->m_pParent->m_vNext.begin())->m_iTag);

          if (l_iSplit != -1 && l_iSplit < m_vNext.size()) {
            a_mSplitSelections[l_iTag] = m_vNext[l_iSplit];
          }
        }

        if (l_iSplit == -1) {
          if (a_mOldSelections.find(l_iTag) == a_mOldSelections.end()) {
            if (m_vNext.size() > 0)
              a_mSplitSelections[l_iTag] = m_vNext[std::rand() % m_vNext.size()];
          }
          else {
            a_mSplitSelections[l_iTag] = a_mOldSelections[l_iTag];
          }
        }

        for (std::vector<SPathLine3d*>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++) {
          (*l_itNext)->m_pParent->m_bSelected = false;
        }

        if (a_mSplitSelections.find(l_iTag) != a_mSplitSelections.end()) {
          SPathLine3d *l_pNext = a_mSplitSelections[l_iTag];
          l_pNext->m_pParent->m_bSelected = true;

          SPathLine2d *l_pChild = l_pNext->transformTo2d(a_cMatrix, a_mSplitSelections, a_mOldSelections, a_vMarbles, a_vMarblePosVel, a_pLuaScript);
          l_pChild->m_pPrevious = &m_cPathLine;
          m_cPathLine.m_vNext.push_back(l_pChild);
        }
      }

      return &m_cPathLine;
    }
    /**
    * Create 2d path lines out of the list of 3d path lines
    * @param a_cMatrix the camera matrix to use for the transformation
    * @param a_cVeclocity 2d velocity to pick the correct path in case of a split
    */
    CControllerAi_V2::SPathLine2d *CControllerAi_V2::SPathLine3d::transformTo2d_Help(
      const irr::core::matrix4 &a_cMatrix,
      const irr::core::line2df &a_cVelocity,
      SPathLine2d *a_pRoot
    ) {
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
        SPathLine2d *l_pChild = (*m_vNext.begin())->transformTo2d_Help(a_cMatrix, a_cVelocity, a_pRoot != nullptr ? a_pRoot : &m_cPathLine);
        l_pChild->m_pPrevious = &m_cPathLine;
        m_cPathLine.m_vNext.push_back(l_pChild);
      }
      else if (m_vNext.size() > 0) {
        std::vector<SPathLine2d *> l_vOptions;

        for (std::vector<SPathLine3d*>::iterator l_itNext = m_vNext.begin(); l_itNext != m_vNext.end(); l_itNext++) {
          SPathLine2d *l_pChild = (*l_itNext)->transformTo2d_Help(a_cMatrix, a_cVelocity, a_pRoot != nullptr ? a_pRoot : &m_cPathLine);
          l_pChild->m_pPrevious = &m_cPathLine;
          l_vOptions.push_back(l_pChild);
        }

        SPathLine2d *l_pSelected = nullptr;
        irr::f32 l_fLength = -1.0f;

        irr::core::line2df l_cCheck = irr::core::line2df(irr::core::vector2df(0.0f), irr::core::vector2df(0.0f, 150.0f));

        for (std::vector<SPathLine2d*>::iterator l_itOptions = l_vOptions.begin(); l_itOptions != l_vOptions.end(); l_itOptions++) {
          SPathLine2d *p = *l_itOptions;

          bool b = false;
          irr::core::vector2df l_cOther = p->m_cLines[0].end + 1.5f * p->m_fWidth * (p->m_cLines[0].end - p->m_cLines[0].start).normalize();
          irr::core::vector2df l_cPoint = a_cVelocity.getClosestPoint(l_cOther);

          irr::f32 l_fDist = (l_cPoint - l_cOther).getLengthSQ();

          if (l_fDist < l_fLength || l_fLength < 0.0f) {
            l_fLength = l_fDist;
            l_pSelected = *l_itOptions;
          }
        }

        if (l_pSelected != nullptr) {
          m_cPathLine.m_vNext.push_back(l_pSelected);
        }
      }

      return &m_cPathLine;
    }

    CControllerAi_V2::SAiPathSection::SAiPathSection() : 
      m_iIndex     (-1),
      m_iSectionIdx(-1),
      m_iTag       (0), 
      m_iCheckpoint(-1), 
      m_bSelected  (false), 
      m_fMinVel    (-1.0f), 
      m_fMaxVel    (-1.0f), 
      m_fBestVel   (-1.0f), 
      m_fWidthSq   (0.0f),
      m_pPrev      (nullptr),
      m_pAiPath    (nullptr) 
    {
    }

    CControllerAi_V2::SAiPathSection::~SAiPathSection() {
      if (m_pAiPath != nullptr)
        delete m_pAiPath;
    }

    CControllerAi_V2::SRacePosition::SRacePosition() : m_iMarble(0), m_iPosition(0), m_iDeficitAhead(0), m_iDeficitLeader(0) {
    }
  }
}