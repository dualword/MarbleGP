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
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId),
      m_pSelected(nullptr),
      m_pHovered (nullptr)
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

            l_pDrv->draw3DLine((*it)->m_cPos + 2.5f * (*it)->m_cNormal, (*it2)->m_pNext->m_cPos +  2.5f * (*it2)->m_pNext->m_cNormal, l_cColor);
          }
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

    bool CAiNode::addPathNode(const irr::core::vector3df& a_cPos, const irr::core::vector3df& a_cNormal) {
      SAiPathNode *p = nullptr;

      if (m_pHovered != nullptr) {
        for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
          if ((*it)->m_pBox == m_pHovered) {
            p = *it;
            std::wstring s = L"Close path by linking to node #" + std::to_wstring(p->m_iIndex);
            // IMainClass::getInstance()->log(s, irr::ELL_INFORMATION);
          }
        }
      }

      if (p == nullptr) {
        p = new SAiPathNode();

        p->m_cPos    = a_cPos;
        p->m_cNormal = a_cNormal;
        p->m_cNormal = p->m_cNormal.normalize();
      
        m_vPathData.push_back(p);
        updatePathIndices();

        irr::scene::IMeshSceneNode *l_pNode = SceneManager->addMeshSceneNode(SceneManager->getMesh("data/objects/box.3ds"), this, -1, p->m_cPos + 2.5f * p->m_cNormal);
        l_pNode->setScale(irr::core::vector3df(1.5f));
        l_pNode->getMaterial(0).TextureLayer[0].Texture = SceneManager->getVideoDriver()->getTexture("data/textures/blue.png");
        l_pNode->setIsDebugObject(true);

        irr::scene::ITriangleSelector* l_pSelector = SceneManager->createTriangleSelector(l_pNode->getMesh(), l_pNode);

        if (l_pSelector != nullptr) {
          l_pNode->setTriangleSelector(l_pSelector);
          l_pSelector->drop();
        }

        p->m_pBox = l_pNode;

        // IMainClass::getInstance()->log(L"Add new AI path node", irr::ELL_INFORMATION);
      }

      if (m_pSelected != nullptr) {
        m_pSelected->m_vNext.push_back(new SAiLink(p, enLinkType::Default));
      }

      m_pSelected = p;

      updatePathColors();

      return true;
    }

    void CAiNode::updatePathColors() {
      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        (*it)->m_pBox->getMaterial(0).TextureLayer[0].Texture = 
          (*it)->m_pBox == m_pHovered ? SceneManager->getVideoDriver()->getTexture("data/textures/yellow.png") :
          (*it) == m_pSelected        ? SceneManager->getVideoDriver()->getTexture("data/textures/blue.png"  ) :
                                        SceneManager->getVideoDriver()->getTexture("data/textures/white.png" );
      }
    }

    void CAiNode::updatePathIndices() {
      int l_iIndex = 0;

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        (*it)->m_iIndex = l_iIndex++;
      }
    }

    void CAiNode::clearSelection() {
      // IMainClass::getInstance()->log(L"AI Path node selection cleared.", irr::ELL_INFORMATION);
      m_pSelected = nullptr;
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
                m_vNext.push_back(new SAiLink(l_cSerializer.getString()));
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
            printf("Link %i to %i\n", m_iIndex, (*cit)->m_iIndex);
            break;
          }
        }
      }
    }

    CAiNode::SAiLink::SAiLink() : m_pNext(nullptr), m_eType(enLinkType::Default), m_iNext(-1), m_iPriority(-1), m_sBlocking("") {
    }

    CAiNode::SAiLink::SAiLink(SAiPathNode* a_pNext, enLinkType a_eType) : m_pNext(a_pNext), m_eType(a_eType), m_iNext(-1), m_iPriority(-1), m_sBlocking("") {
    }

    CAiNode::SAiLink::SAiLink(const std::string &a_sData) : m_pNext(nullptr), m_eType(enLinkType::Default), m_iNext(-1), m_iPriority(-1), m_sBlocking("") {
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

    void CAiNode::setHovered(irr::scene::ISceneNode* a_pNode) {
      m_pHovered = a_pNode;
      updatePathColors();
    }

    void CAiNode::moveSelected(const irr::core::vector3df& a_cNewPos) {
      if (m_pSelected != nullptr) {
        m_pSelected->m_cPos = a_cNewPos;
        if (m_pSelected->m_pBox != nullptr)
          m_pSelected->m_pBox->setPosition(a_cNewPos + 2.5f * m_pSelected->m_cNormal);
      }
    }

    void CAiNode::deletePathNode(irr::scene::ISceneNode *a_pNode) {
      SAiPathNode *l_pNode = nullptr;

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        if ((*it)->m_pBox == a_pNode) {
          l_pNode = *it;
          break;
        }
      }

      if (l_pNode != nullptr) {
        if (m_pSelected == l_pNode) {
          m_pSelected = nullptr;
        }

        for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
          for (std::vector<SAiLink*>::iterator it2 = (*it)->m_vNext.begin(); it2 != (*it)->m_vNext.end(); it2++) {
            if ((*it2)->m_pNext == l_pNode) {
              delete *it2;
              (*it)->m_vNext.erase(it2);
              // IMainClass::getInstance()->log(L"Node Link deleted.", irr::ELL_INFORMATION);
              break;
            }
          }
        }

        for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
          if (*it == l_pNode) {
            break;
          }
        }
      }

      updatePathColors();
      updatePathIndices();
    }

    scenenodes::CAiNode::SAiPathNode* CAiNode::getSelectedNodeData() {
      return m_pSelected;
    }

    void CAiNode::deleteLink(irr::scene::ISceneNode* a_pLink) {
      SAiPathNode *l_pNode = nullptr;

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        if ((*it)->m_pBox == a_pLink) {
          l_pNode = *it;
          break;
        }
      }

      if (l_pNode != nullptr) {
        for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
          for (std::vector<SAiLink*>::iterator it2 = (*it)->m_vNext.begin(); it2 != (*it)->m_vNext.end(); it2++) {
            if ((*it2)->m_pNext == l_pNode) {
              delete *it2;
              (*it)->m_vNext.erase(it2);
              // IMainClass::getInstance()->log(L"Node Link deleted.", irr::ELL_INFORMATION);
              break;
            }
          }
        }
      }

      updatePathColors();
      updatePathIndices();
    }

    bool CAiNode::setSelected(irr::scene::ISceneNode* a_pNode) {
      m_pSelected = nullptr;

      for (std::vector<SAiPathNode*>::iterator it = m_vPathData.begin(); it != m_vPathData.end(); it++) {
        if ((*it)->m_pBox == a_pNode) {
          if (m_pSelected != nullptr)
            m_pSelected->m_vNext.push_back(new SAiLink(*it, enLinkType::Default));

          m_pSelected = *it;
          updatePathColors();
          return true;
        }
      }

      return false;
    }
 }
}