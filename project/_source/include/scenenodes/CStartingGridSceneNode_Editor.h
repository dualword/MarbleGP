// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace scenenodes {
    const int g_StartingGridScenenodeId = MAKE_IRR_ID('d', 's', 'g', 's');
    const irr::c8 g_StartingGridScenenodeName[] = "StartingGridScenenode";

    class CStartingGridSceneNode_Editor : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df  m_cBox;
        irr::video::SMaterial m_cMaterial;
        irr::video::S3DVertex m_aVertices[20];

        int m_iMarblesPerRow;
        float m_fWidth,   /**< Total width of the grid*/
              m_fLength,  /**< Total length of the grid*/
              m_fAngle;   /**< Angle around the Y-axis of the grid*/

        irr::scene::IMeshSceneNode* m_pMarbles[16]; /**< The marble nodes */

        void updateGridPositions();

      public:
        CStartingGridSceneNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CStartingGridSceneNode_Editor();

        //*** Virtual method inherited from irr::scene::ISceneNode
        virtual irr::u32 getMaterialCount();
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
    };
  }
}
