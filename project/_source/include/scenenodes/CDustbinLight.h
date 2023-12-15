/* (w) 2021 - 23 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    const int g_DustbinLightId = MAKE_IRR_ID('d', 's', 'l', 'd');
    const irr::c8 g_DustbinLightName[] = "DustbinLight";

    /**
    * @class CDustbinLight
    * @author Christian Keimel
    * This node stores data for the global light used by the Dustbin Shader
    */
    class CDustbinLight : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df m_cBox;

        irr::f32 m_fNear;   /**< The light's near value */
        irr::f32 m_fFar;    /**< The light's far value */
        irr::f32 m_fFov;    /**< The light's field of view */

        irr::core::vector3df m_cTarget;     /**< The light's target */

      public:
        CDustbinLight(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CDustbinLight();

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
        * Set the light target
        * @param a_cTarget the light target
        */
        void setLightTarget(const irr::core::vector3df &a_cTarget);

        /**
        * Get the light target
        * @return the light target
        */
        const irr::core::vector3df &getLightTarget() const;

        /**
        * Set the field of view
        * @param a_fFov the field of view
        */
        void setFieldOfView(irr::f32 a_fFov);

        /**
        * Get the field of view
        * @return the field of view
        */
        irr::f32 getFieldOfView() const;

        /**
        * Set the light's near value
        * @param a_fNear the light's near value
        */
        void setNearValue(irr::f32 a_fNear);

        /**
        * Get the light's near value
        * @return the light's near value
        */
        irr::f32 getNearValue() const;

        /**
        * Set the light's far value
        * @param a_fFar the light's far value
        */
        void setFarValue(irr::f32 a_fFar);

        /**
        * Get the light's far value
        * @return the light's far value
        */
        irr::f32 getFarValue() const;
    };
  }
}