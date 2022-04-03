#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace scenenodes {
    const int g_DustbinCameraId = MAKE_IRR_ID('d', 'g', 'c', 'a');
    const irr::c8 g_DustbinCameraNodeName[] = "DustbinCamera";

    /**
    * @class CDustbinCamera
    * @author Christian Keimel
    * This scenenode is used to store camera
    * data in a way that works for me
    */
    class CDustbinCamera : public irr::scene::ISceneNode {
    private:
      irr::core::aabbox3df m_cBox;

      irr::f32 m_fAngleV;
      irr::f32 m_fAngleH;

    public:
      CDustbinCamera(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
      virtual ~CDustbinCamera();

      virtual void render();
      virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
      virtual irr::scene::ESCENE_NODE_TYPE getType() const;
      virtual void OnRegisterSceneNode();

      virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
      virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

      virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

      void setValues(const irr::core::vector3df &a_cPosition, irr::f32 a_fAngleV, irr::f32 a_fAngleH);
      void getValues(irr::core::vector3df &a_cPosition, irr::f32 &a_fAngleV, irr::f32 &a_fAngleH);
    };
  }
}

