/* (w) 2021 - 23 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace scenenodes {
    const int g_SpeedNodeId = MAKE_IRR_ID('s', 'p', 'e', 'e');
    const irr::c8 g_SpeedNodeName[] = "SpeedNode";

    /**
    * @class CSpeedNode
    * @author Christian Keimel
    * This node is used to indicate the proper speed before a jump for the marbles on-track
    */
    class CSpeedNode : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df m_cBox;

        irr::f32 m_fMinSpeed;   /**< The minimum speed for success */
        irr::f32 m_fMaxSpeed;   /**< The maximum speed for success */
        irr::f32 m_fBestSpeed;  /**< The best speed for the jump */

        irr::video::ITexture *m_aSpeed[4];      /**< The textures indicating the speed (0 == orange, 1 == red, 2 == green, 3 == gray) */
        irr::video::ITexture *m_aWarn [2];      /**< The warning textures notofying the player of a completely wrong velocity (0 == orange, 1 == red) */
        irr::video::ITexture *m_aArrow[2];      /**< The arrows showing the player that the speed can be changed (0 == orange, 1 == red) */

        /**
        * Update the textures of all "IMeshSceneNode" children
        * @param a_pTexture the new texture
        */
        void updateTexture(irr::video::ITexture *a_pTexture);

      public:
        CSpeedNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CSpeedNode();

        //*** Virtual method inherited from irr::scene::ISceneNode
        virtual irr::u32 getMaterialCount();
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        /**
        * Get the minimal speed
        * @return the minimal speed
        */
        irr::f32 getMinSpeed();

        /**
        * Get the maximal speed
        * @return the minimal speed
        */
        irr::f32 getMaxSpeed();

        /**
        * Get the bestspeed
        * @return the minimal speed
        */
        irr::f32 getBestSpeed();

        /**
        * Set the speed of the marble of the current viewport
        */
        void setMarbleSpeed(irr::f32 a_fSpeed, int a_iStep);

        /**
        * Fill the texture maps
        */
        void fillTextureMaps();
    };
  }
}