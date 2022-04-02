/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CAiGameData.h>
#include <scenenodes/CRespawnNode.h>
#include <helpers/CStringHelpers.h>
#include <messages/CSerializer64.h>
#include <scenenodes/CDustbinId.h>
#include <scenenodes/CWorldNode.h>
#include <scenenodes/CAiNode.h>
#include <algorithm>

namespace dustbin {
  namespace scenenodes {
    CAiNode::CAiNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : 
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId)
    {
    }

    CAiNode::~CAiNode() {
      for (std::vector<SAiPathNode *>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++)
        delete *it;

      m_vPathData.clear();
    }

    void CAiNode::render() {
      if (IsVisible) {
        irr::video::SMaterial l_cMaterial;
        l_cMaterial.AmbientColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.EmissiveColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.DiffuseColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        l_cMaterial.SpecularColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);

        l_cMaterial.Lighting        = false;
        l_cMaterial.Thickness       = 5.0f;
        l_cMaterial.Wireframe       = true;
        l_cMaterial.BackfaceCulling = false;

        irr::video::IVideoDriver *l_pDrv = SceneManager->getVideoDriver();

        l_pDrv->setMaterial(l_cMaterial);
        l_pDrv->setTransform(irr::video::ETS_WORLD, irr::core::matrix4());

        for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
          for (std::vector<SAiLink *>::iterator it2 = (*it)->m_vNext.begin(); it2 != (*it)->m_vNext.end(); it2++) {
            irr::video::SColor l_cColor;

            switch ((*it2)->m_eType) {
              case enLinkType::Default     : l_cColor = irr::video::SColor(0xFF, 0x80, 0x80, 0xFF); break;
              case enLinkType::Cruise      : l_cColor = irr::video::SColor(0xFF, 0x80, 0xFF, 0x80); break;
              case enLinkType::TimeAttack  : l_cColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF); break;
              case enLinkType::Offensive   : l_cColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0x80); break;
              case enLinkType::Defensive   : l_cColor = irr::video::SColor(0xFF, 0xFF, 0x80, 0x80); break;
              case enLinkType::MayBeBlocked: l_cColor = irr::video::SColor(0xFF, 0x00, 0x00, 0x00); break;
            }

            l_pDrv->draw3DLine((*it2)->m_cLinkLine.start, (*it2)->m_cLinkLine.end, l_cColor);
          }
        }

        for (std::map<irr::s32, irr::core::line3df>::iterator it = m_mClosest.begin(); it != m_mClosest.end(); it++) {
          l_pDrv->draw3DLine(it->second.start, it->second.end, irr::video::SColor(0xFF, 0, 0, 0xFF));
        }

        for (std::map<irr::s32, irr::core::line3df>::iterator it = m_mLookAhead.begin(); it != m_mLookAhead.end(); it++) {
          l_pDrv->draw3DLine(it->second.start, it->second.end, irr::video::SColor(0xFF, 0, 0xFF, 0));
        }
      }
    }

    const irr::core::aabbox3d<irr::f32>& CAiNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CAiNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId;
    }

    irr::scene::ISceneNode* CAiNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      CAiNode *p = new CAiNode(Parent, SceneManager, getNextSceneNodeId());

      return p;
    }

    void CAiNode::OnRegisterSceneNode() {
      if (IsVisible) {
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_AUTOMATIC);
      }

      ISceneNode::OnRegisterSceneNode();
    }

    void CAiNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::scene::ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      int l_iIndex = 0;
      std::string l_sName = "AiPathData_0";

      while (a_pIn->existsAttribute(l_sName.c_str())) {
        std::string l_sData = a_pIn->getAttributeAsString(l_sName.c_str()).c_str();

        SAiPathNode *p = new SAiPathNode(l_sData);
        
        irr::scene::IMeshSceneNode *l_pNode = SceneManager->addMeshSceneNode(SceneManager->getMesh("data/objects/box.3ds"), this, -1, p->m_cPos + 2.5f * p->m_cNormal);
        l_pNode->setScale(irr::core::vector3df(1.5f));
        l_pNode->getMaterial(0).TextureLayer[0].Texture = SceneManager->getVideoDriver()->getTexture("data/textures/blue.png");
        l_pNode->setIsDebugObject(true);
        
        p->m_pBox = l_pNode;

        m_vPathData.push_back(p);

        l_iIndex++;
        l_sName = "AiPathData_" + std::to_string(l_iIndex);
      }

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        (*it)->fillNextVector(m_vPathData);
      }
    }

    void CAiNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::scene::ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      int l_iIndex = 0;
      for (std::vector<SAiPathNode*>::const_iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        std::string l_sName = "AiPathData_" + std::to_string(l_iIndex);
        a_pOut->addString(l_sName.c_str(), (*it)->serialize().c_str());

        l_iIndex++;
      }
    }

    void CAiNode::setClosest(irr::s32 a_iMarble, const irr::core::line3df& a_cLine) {
      m_mClosest[a_iMarble] = a_cLine;
    }

    void CAiNode::setLookAhead(irr::s32 a_iMarble, const irr::core::line3df& a_cLine) {
      m_mLookAhead[a_iMarble] = a_cLine;
    }

    void CAiNode::updatePathIndices() {
      int l_iIndex = 0;

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        (*it)->m_iIndex = l_iIndex++;
      }
    }

    /**
    * Get the defined path from the AI scene-node.
    * Ownership is passed to the calling object
    * @return the defined path
    */
    std::vector<scenenodes::CAiNode::SAiPathNode*>& CAiNode::getPath() {

      return m_vPathData;
    }

    CAiNode::SAiPathNode::SAiPathNode() :
      m_iIndex (-1),
      m_bJump  (false),
      m_fMinVel(-1.0f),
      m_fMaxVel(-1.0f),
      m_pBox   (nullptr)
    {
    }

    CAiNode::SAiPathNode::SAiPathNode(const std::string& a_sData) : m_iIndex(-1), m_bJump(false), m_fMinVel(-1.0f), m_fMaxVel(-1.0f), m_pBox(nullptr) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      if (l_cSerializer.getS32() == 11) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iNext = l_cSerializer.getS32();

          switch (l_iNext) {
            case 34:  
              m_iIndex = l_cSerializer.getS32();
              break;

            case 26:
              m_cPos = l_cSerializer.getVector3df();
              break;

            case 42:
              m_cNormal = l_cSerializer.getVector3df();
              break;

            case 46: {
              int l_iSize = l_cSerializer.getS32();

              for (int i = 0; i < l_iSize; i++) {
                SAiLink *l_pLink = new SAiLink(l_cSerializer.getString(), this);
                if (l_pLink->m_iNext != m_iIndex)
                  m_vNext.push_back(l_pLink);
                else {
                  printf("Can't link node to itself!\n");
                  delete l_pLink;
                }
              }

              break;
            }

            case 28:
              m_bJump = true;
              break;

            case 96:
              m_fMinVel = l_cSerializer.getF32();
              break;

            case 99:
              m_fMaxVel = l_cSerializer.getF32();
              break;

            default: {
              std::wstring s = L"Unknown next indicator #" + std::to_wstring(l_iNext);
              // IMainClass::getInstance()->log(s.c_str(), irr::ELL_WARNING, true);
            }
          }
        }
      }
    }

    CAiNode::SAiPathNode::~SAiPathNode() {
      for (std::vector<SAiLink *>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++)
        delete *it;

      m_vNext.clear();
    }

    std::string CAiNode::SAiPathNode::serialize() {
      messages::CSerializer64 l_cSerializer;

      // Header
      l_cSerializer.addS32(11);

      // Path Index
      l_cSerializer.addS32(34);
      l_cSerializer.addS32(m_iIndex);

      // Position
      l_cSerializer.addS32(26);
      l_cSerializer.addVector3df(m_cPos);

      // Normal
      l_cSerializer.addS32(42);
      l_cSerializer.addVector3df(m_cNormal);

      // Jump
      if (m_bJump)
        l_cSerializer.addS32(28);

      // Minimal velocity
      if (m_fMinVel > 0.0f) {
        l_cSerializer.addS32(96);
        l_cSerializer.addF32(m_fMinVel);
      }

      // Maximal velocity
      if (m_fMaxVel > 0.0f) {
        l_cSerializer.addS32(99);
        l_cSerializer.addF32(m_fMaxVel);
      }

      // Next Nodes
      l_cSerializer.addS32(46);
      l_cSerializer.addS32((irr::s32)m_vNext.size());

      for (std::vector<SAiLink *>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++) {
        l_cSerializer.addString((*it)->serialize());
      }

      return l_cSerializer.getMessageAsString();
    }

    void CAiNode::SAiPathNode::fillNextVector(const std::vector<SAiPathNode*>& a_cPathData) {
      for (std::vector<SAiLink*>::iterator it = m_vNext.begin(); it != m_vNext.end(); it++) {
        for (std::vector<SAiPathNode*>::const_iterator cit = a_cPathData.begin(); cit != a_cPathData.end(); cit++) {
          if ((*it)->m_iNext == (*cit)->m_iIndex) {
            (*it)->m_pNext = *cit;
            (*it)->m_cLinkLine = irr::core::line3df(m_cPos + 2.5f * m_cNormal, (*cit)->m_cPos + 2.5f * (*cit)->m_cNormal);
            (*it)->m_fLinkLength = (*it)->m_cLinkLine.getLength();
            printf("Link %i to %i\n", m_iIndex, (*cit)->m_iIndex);
            break;
          }
        }
      }
    }

    CAiNode::SAiLink::SAiLink() : m_pNext(nullptr), m_eType(enLinkType::Default), m_iNext(-1), m_iPriority(-1), m_fLinkLength(0.0f), m_sBlocking("") {
    }

    CAiNode::SAiLink::SAiLink(const std::string &a_sData, SAiPathNode *a_pThis) : m_pNext(nullptr), m_pThis(a_pThis), m_eType(enLinkType::Default), m_iNext(-1), m_iPriority(-1), m_fLinkLength(0.0f), m_sBlocking("") {
      messages::CSerializer64 l_cSerializer(a_sData.c_str());

      irr::s32 l_iHeader = l_cSerializer.getS32();

      if (l_iHeader == 6) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iType = l_cSerializer.getS32();

          switch (l_iType) {
            case 11:
              m_iNext = l_cSerializer.getS32();
              break;

            case 23:
              m_eType = (enLinkType)l_cSerializer.getS32();
              break;

            case 12:
              m_sBlocking = l_cSerializer.getString();
              break;

            case 34:
              m_iPriority = l_cSerializer.getS32();
              break;

            default:
              // IMainClass::getInstance()->log((std::wstring(L"Invalid field header #") + std::to_wstring(l_iType)).c_str(), irr::ELL_WARNING);
              break;
          }
        }
      }
      // else IMainClass::getInstance()->log(L"Invalid header of AI Link.", irr::ELL_WARNING);
    }

    std::string CAiNode::SAiLink::serialize() {
      messages::CSerializer64 l_cSerializer;

      // Header
      l_cSerializer.addS32(6);

      // Next
      if (m_pNext != nullptr) {
        l_cSerializer.addS32(11);
        l_cSerializer.addS32(m_pNext->m_iIndex);
      }

      // Type
      l_cSerializer.addS32(23);
      l_cSerializer.addS32((irr::s32)m_eType);

      // Blocking String
      if (m_sBlocking != "" && m_eType == enLinkType::MayBeBlocked) {
        l_cSerializer.addS32(12);
        l_cSerializer.addString(m_sBlocking);
      }

      // Priority index
      if (m_iPriority != -1) {
        l_cSerializer.addS32(34);
        l_cSerializer.addS32(m_iPriority);
      }

      return l_cSerializer.getMessageAsString();
    }
  }
}