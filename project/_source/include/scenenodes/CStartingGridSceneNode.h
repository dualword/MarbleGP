// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gameclasses/SMarbleNodes.h>
#include <irrlicht.h>

namespace dustbin {
  namespace shaders {
    class CDustbinShaders;
  }

  namespace scenenodes {
    const int g_StartingGridScenenodeId = MAKE_IRR_ID('d', 's', 'g', 's');
    const irr::c8 g_StartingGridScenenodeName[] = "StartingGridScenenode";

    /**
    * @class CStartingGridSceneNode
    * @author Christian Keimel
    * This is the starting grid node for the game which does not
    * render a cone and creates phyiscs objects
    */
    class CStartingGridSceneNode : public irr::scene::ISceneNode {
      private:
        int                   m_iNextMarble;
        irr::core::aabbox3df  m_cBox;
        irr::video::SMaterial m_cMaterial;

        int   m_iMarblesPerRow;
        float m_fWidth;      /**< Total width of the grid*/
        float m_fRowLength;  /**< Total length of the grid*/
        float m_fOffset;     /**< The offset of the first marble in a row to the last marble in a row */
        float m_fAngle;      /**< Angle around the Y-axis of the grid*/

        shaders::CDustbinShaders *m_pShader;  /**< The dustbin shader instance */

        gameclasses::SMarbleNodes *m_pMarbles[16]; /**< The marble nodes */

        void updateGridPositions();

      public:
        CStartingGridSceneNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CStartingGridSceneNode();

        irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager);

        gameclasses::SMarbleNodes *getMarble(int a_iMarbleID);
        void removeUnusedMarbles();

        /**
        * Get a marble scene node without removing it from the list
        * @param a_iMarbleID the ID of the marble
        * @return the marble's positional scene node
        */
        irr::scene::ISceneNode *getMarbleById(int a_iMarbleID);

        irr::f32 getAngle();

        virtual irr::u32 getMaterialCount() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;

        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual void render();

        /**
        * Set the shader
        * @param a_pShader the shader
        */
        void setShader(shaders::CDustbinShaders *a_pShader);
    };
  }
}
