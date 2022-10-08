/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CAiGameData.h>
#include <scenenodes/CRespawnNode.h>
#include <scenenodes/CAiPathNode.h>
#include <messages/CSerializer64.h>
#include <scenenodes/CDustbinId.h>
#include <scenenodes/CWorldNode.h>
#include <CGlobal.h>
#include <algorithm>

namespace dustbin {
  namespace scenenodes {
    const int c_iBegin      = 6;
    const int c_iIndex      = 11;
    const int c_iPos        = 12;
    const int c_iSide       = 20;
    const int c_iNormal     = 23;
    const int c_iWidth      = 26;
    const int c_iFactor     = 28;
    const int c_iVMin       = 42;
    const int c_iVMax       = 46;
    const int c_iType       = 66;
    const int c_iNextStart  = 69;
    const int c_iNext       = 93;
    const int c_iNextEnd    = 94;
    const int c_iVBest      = 95;
    const int c_iEnd        = 99;
    

    CAiPathNode::CAiPathNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : 
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId),
      m_pDrv         (nullptr),
      m_pSelected    (nullptr),
      m_bNewlyCreated(true)
    {
      m_pDrv = a_pMgr->getVideoDriver();
      setVisible(false);
    }

    CAiPathNode::~CAiPathNode() {
      while (m_vSections.size() > 0) {
        SAiPathSection *p = *m_vSections.begin();
        m_vSections.erase(m_vSections.begin());
        delete p;
      }
    }

    void CAiPathNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CAiPathNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CAiPathNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_AiPathNodeId;
    }

    irr::scene::ISceneNode* CAiPathNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      CAiPathNode *p = new CAiPathNode(Parent, SceneManager, getNextSceneNodeId());

      return p;
    }

    void CAiPathNode::OnRegisterSceneNode() {
      if (IsVisible) {
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_AUTOMATIC);
      }

      ISceneNode::OnRegisterSceneNode();
    }

    void CAiPathNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::scene::ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      m_bStartNode = a_pIn->getAttributeAsBool("startnode");

      int l_iIndex = 0;
      std::string l_sName = "AISection_" + std::to_string(l_iIndex);

      // Read all AI path sections
      while (a_pIn->existsAttribute(l_sName.c_str())) {
        std::string l_sData = a_pIn->getAttributeAsString(l_sName.c_str()).c_str();

        if (l_sData != "")
          m_vSections.push_back(new SAiPathSection(l_sData));

        l_iIndex++;
        l_sName = "AISection_" + std::to_string(l_iIndex);
      }

      // Create the links between the AI path sections
      for (std::vector<SAiPathSection*>::iterator it = m_vSections.begin(); it != m_vSections.end(); it++) {
        for (std::vector<irr::s32>::iterator it2 = (*it)->m_vNextIndices.begin(); it2 != (*it)->m_vNextIndices.end(); it2++) {
          (*it)->addToNextSections(m_vSections[*it2]);
          m_vSections[*it2]->addToPrevSections(*it);
        }
        (*it)->m_vNextIndices.clear();
      }

      if (m_bNewlyCreated) {
        setVisible(false);
        m_bNewlyCreated = false;
      }
    }

    void CAiPathNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::scene::ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("startnode", m_bStartNode);

      int l_iIndex = 0;

      for (std::vector<SAiPathSection*>::const_iterator it = m_vSections.begin(); it != m_vSections.end(); it++) {
        std::string l_sName = "AISection_" + std::to_string(l_iIndex++);
        a_pOut->addString(l_sName.c_str(), (*it)->serialize().c_str());
      }
    }

    bool CAiPathNode::isStartupPath() const {
      return m_bStartNode;
    }

    void CAiPathNode::addPathSection(CAiPathNode::SAiPathSection* a_pNew) {
      m_vSections.push_back(a_pNew);
      m_cBox.addInternalPoint(a_pNew->m_cPosition);
      m_cBox.addInternalPoint(a_pNew->m_cPosition + a_pNew->m_fWidth * a_pNew->m_cSideVector);
      m_cBox.addInternalPoint(a_pNew->m_cPosition - a_pNew->m_fWidth * a_pNew->m_cSideVector);
      m_cBox.addInternalPoint(a_pNew->m_cPosition + a_pNew->m_cNormal);

      int l_iIndex = 0;

      for (std::vector<SAiPathSection *>::iterator it = m_vSections.begin(); it != m_vSections.end(); it++) {
        (*it)->m_iIndex = l_iIndex++;
      }
    }

    CAiPathNode::SAiPathSection::SAiPathSection() :
      m_iIndex    (0),
      m_iNode     (-1),
      m_eType     (enSegmentType::Default),
      m_fWidth    (1.00f),
      m_fFactor   (0.75f),
      m_fMinSpeed (-1.0f),
      m_fMaxSpeed (-1.0f),
      m_fBestSpeed(-1.0f)
    {
    }

    CAiPathNode::SAiPathSection::SAiPathSection(const std::string &a_sData) :
      m_iIndex    (0),
      m_iNode     (-1),
      m_eType     (enSegmentType::Default),
      m_fWidth    (1.00f),
      m_fFactor   (0.75f),
      m_fMinSpeed (-1.0f),
      m_fMaxSpeed (-1.0f),
      m_fBestSpeed(-1.0f)
    {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      if (l_cSerializer.getS32() == c_iBegin) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iToken = l_cSerializer.getS32();

          switch (l_iToken) {
            case c_iIndex    : m_iIndex      =                l_cSerializer.getS32      (); break;
            case c_iPos      : m_cPosition   =                l_cSerializer.getVector3df(); break;
            case c_iSide     : m_cSideVector =                l_cSerializer.getVector3df(); break;
            case c_iNormal   : m_cNormal     =                l_cSerializer.getVector3df(); break;
            case c_iWidth    : m_fWidth      =                l_cSerializer.getF32      (); break;
            case c_iFactor   : m_fFactor     =                l_cSerializer.getF32      (); break;
            case c_iVMin     : m_fMinSpeed   =                l_cSerializer.getF32      (); break;
            case c_iVMax     : m_fMaxSpeed   =                l_cSerializer.getF32      (); break;
            case c_iVBest    : m_fBestSpeed  =                l_cSerializer.getF32      (); break;
            case c_iType     : m_eType       = (enSegmentType)l_cSerializer.getS32      (); break;
            case c_iNextStart: {
              while (l_cSerializer.hasMoreMessages()) {
                irr::s32 l_iNextToken = l_cSerializer.getS32();

                if (l_iNextToken == c_iNext)
                  m_vNextIndices.push_back(l_cSerializer.getS32());
                else if (l_iNextToken == c_iNextEnd)
                  break;
              }
              break;
            }
            default:
              break;
          }

          if (l_iToken == c_iEnd) {
            break;
          }
        }
      }
    }

    CAiPathNode::SAiPathSection::~SAiPathSection() {
    }

    void CAiPathNode::SAiPathSection::draw(irr::video::IVideoDriver* a_pDrv, bool a_bIsSelected) {
      irr::core::line3df l_cThis = getSegmentLine();
      irr::video::SColor l_cColor = m_eType == enSegmentType::Default ? irr::video::SColor(0xFF, 0, 0xFF, 0) : m_eType == enSegmentType::Jump ? irr::video::SColor(0xFF, 0, 0, 0xFF) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);

      a_pDrv->draw3DLine(l_cThis.start, l_cThis.end, a_bIsSelected ? irr::video::SColor(0xFF, 0xFF, 0, 0) : l_cColor);
      a_pDrv->draw3DLine(l_cThis.getMiddle(), l_cThis.getMiddle() + 5.0f * m_cNormal, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));

      for (std::vector<SAiPathSection*>::iterator it = m_vNextSegments.begin(); it != m_vNextSegments.end(); it++) {
        irr::core::line3df l_cOther = (*it)->getSegmentLine();

        irr::core::line3df l_cLines[] = {
          irr::core::line3df(l_cThis.start, l_cOther.start),
          irr::core::line3df(l_cThis.end  , l_cOther.end  ),
          irr::core::line3df(l_cThis.start, l_cOther.end  ),
          irr::core::line3df(l_cThis.end  , l_cOther.start)
        };

        if (l_cLines[0].getMiddle().getDistanceFromSQ(l_cLines[1].getMiddle()) > l_cLines[0].getMiddle().getDistanceFromSQ(l_cLines[3].getMiddle())) {
          a_pDrv->draw3DLine(l_cLines[0].start, l_cLines[0].end, l_cColor);
          a_pDrv->draw3DLine(l_cLines[1].start, l_cLines[1].end, l_cColor);
        }
        else {
          a_pDrv->draw3DLine(l_cLines[2].start, l_cLines[2].end, l_cColor);
          a_pDrv->draw3DLine(l_cLines[3].start, l_cLines[3].end, l_cColor);
        }
      }
    }

    irr::core::line3df CAiPathNode::SAiPathSection::getSegmentLine() {
      return irr::core::line3df(m_cPosition - m_fWidth * m_fFactor * m_cSideVector + 0.15f * m_cNormal, m_cPosition + m_fWidth * m_fFactor * m_cSideVector + 0.15f * m_cNormal);
    }

    /**
    * Add a segment to the vector of next sections
    * @param a_pNext the next section to add
    */
    void CAiPathNode::SAiPathSection::addToNextSections(SAiPathSection* a_pNext) {
      for (std::vector<SAiPathSection *>::iterator it = m_vNextSegments.begin(); it != m_vNextSegments.end(); it++)
        if ((*it) == a_pNext) {
          CGlobal::getInstance()->getIrrlichtDevice()->getLogger()->log(L"Section is already in the list of next segments, aborting.", irr::ELL_INFORMATION);
          return;
        }

      m_vNextSegments.push_back(a_pNext);
    }

    /**
    * Add a segment to the vector of previous sections
    * @param a_pPrev the previous section to add
    */
    void CAiPathNode::SAiPathSection::addToPrevSections(SAiPathSection* a_pPrev) {
      for (std::vector<SAiPathSection *>::iterator it = m_vPrevSegments.begin(); it != m_vPrevSegments.end(); it++)
        if ((*it) == a_pPrev) {
          CGlobal::getInstance()->getIrrlichtDevice()->getLogger()->log(L"Section is already in the list of previous segments, aborting.", irr::ELL_INFORMATION);
          return;
        }

      m_vPrevSegments.push_back(a_pPrev);
    }

    std::string CAiPathNode::SAiPathSection::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iBegin);

      l_cSerializer.addS32(c_iIndex);
      l_cSerializer.addS32(m_iIndex);

      l_cSerializer.addS32(c_iPos);
      l_cSerializer.addVector3df(m_cPosition);

      l_cSerializer.addS32(c_iSide);
      l_cSerializer.addVector3df(m_cSideVector);

      l_cSerializer.addS32(c_iNormal);
      l_cSerializer.addVector3df(m_cNormal);

      l_cSerializer.addS32(c_iWidth);
      l_cSerializer.addF32(m_fWidth);

      l_cSerializer.addS32(c_iFactor);
      l_cSerializer.addF32(m_fFactor);

      if (m_eType == enSegmentType::Jump) {
        l_cSerializer.addS32(c_iVMin);
        l_cSerializer.addF32(m_fMinSpeed);

        l_cSerializer.addS32(c_iVMax);
        l_cSerializer.addF32(m_fMaxSpeed);

        l_cSerializer.addS32(c_iVBest);
        l_cSerializer.addF32(m_fBestSpeed);
      }

      l_cSerializer.addS32(c_iType);
      l_cSerializer.addS32((irr::s32)m_eType);

      l_cSerializer.addS32(c_iNextStart);

      for (std::vector<SAiPathSection*>::iterator it = m_vNextSegments.begin(); it != m_vNextSegments.end(); it++) {
        l_cSerializer.addS32(c_iNext);
        l_cSerializer.addS32((*it)->m_iIndex);
      }

      l_cSerializer.addS32(c_iNextEnd);

      l_cSerializer.addS32(c_iEnd);

      return l_cSerializer.getMessageAsString();
    }
  }
}