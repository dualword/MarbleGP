// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode_Editor.h>

namespace dustbin {
  namespace scenenodes {
    CStartingGridSceneNode_Editor::CStartingGridSceneNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CStartingGridSceneNode_Base(a_pParent, a_pMgr, a_iId)
    {
    }

    CStartingGridSceneNode_Editor::~CStartingGridSceneNode_Editor() {
      printf("Delete starting grid scene node.\n");
    }

    void CStartingGridSceneNode_Editor::render() {
      if (IsVisible) {

        irr::u16 l_aIndices[108];

        for (int i = 0; i < 18; i++) {
          l_aIndices[     (3 * i)    ] = 0;
          l_aIndices[     (3 * i) + 1] = i != 17 ? i + 2 : 1;
          l_aIndices[     (3 * i) + 2] = i + 1;
          l_aIndices[54 + (3 * i)    ] = 19;
          l_aIndices[54 + (3 * i) + 1] = i + 1;
          l_aIndices[54 + (3 * i) + 2] = i != 17 ? i + 2 : 1;
        }


        irr::video::IVideoDriver* l_pDrv = SceneManager->getVideoDriver();

        l_pDrv->setMaterial(m_cMaterial);
        l_pDrv->setTransform(irr::video::ETS_WORLD, getAbsoluteTransformation());
        l_pDrv->drawVertexPrimitiveList(m_aVertices, 20, &l_aIndices[0], 36, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, irr::video::EIT_16BIT);
      }
    }

    irr::scene::ISceneNode* CStartingGridSceneNode_Editor::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent  == nullptr) a_pNewParent  = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CStartingGridSceneNode_Editor* l_pRet = new CStartingGridSceneNode_Editor(a_pNewParent, a_pNewManager, ID);

      l_pRet->m_iMarblesPerRow = m_iMarblesPerRow;
      l_pRet->m_fWidth         = m_fWidth;
      l_pRet->m_fRowLength     = m_fRowLength;
      l_pRet->m_fOffset        = m_fOffset;
      l_pRet->m_fAngle         = m_fAngle;

      return l_pRet;
    }
  }
}