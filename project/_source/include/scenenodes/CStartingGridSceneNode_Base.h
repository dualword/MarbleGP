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

    /**
    * @class CStartingGridSceneNode_Base
    * @author Christian Keimel
    * This is the base of the starting grid scene nodes. There are two
    * derived classes, one for the editor including a cone to show the
    * position, and one for the game including core to create physics
    * objects
    */
    class CStartingGridSceneNode_Base : public irr::scene::ISceneNode {
      protected:
        irr::core::aabbox3df  m_cBox;
        irr::video::SMaterial m_cMaterial;
        irr::video::S3DVertex m_aVertices[20];

        int m_iMarblesPerRow;
        float m_fWidth,      /**< Total width of the grid*/
          m_fRowLength,  /**< Total length of the grid*/
          m_fOffset,     /**< The offset of the first marble in a row to the last marble in a row */
          m_fAngle;      /**< Angle around the Y-axis of the grid*/

        irr::scene::IMeshSceneNode* m_pMarbles[16]; /**< The marble nodes */

        void updateGridPositions();

      public:
        CStartingGridSceneNode_Base(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CStartingGridSceneNode_Base();

        //*** Virtual method inherited from irr::scene::ISceneNode
        virtual irr::u32 getMaterialCount();
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
    };
  }
}