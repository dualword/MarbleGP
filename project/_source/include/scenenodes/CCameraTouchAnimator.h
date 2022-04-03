// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CMyCameraAnimator
    * @author Christian Keimel
    * This is the custom scene node animator for the "view track" camera
    * with touch control for the Android version
    */
    class CMyCameraAnimator : public irr::scene::ISceneNodeAnimator {
      private:
        irr::core::vector2di   m_vPosition[2];
        irr::core::vector3df   m_vCamLookAt;
        irr::core::vector3df   m_vCamPos;
        irr::f32               m_fAngleV;
        irr::f32               m_fAngleH;
        irr::s32               m_aTouchID[2];
        irr::s32               m_iTouchCnt;     /**< The number of touching fingers */
        bool                   m_bReady;
        irr::core::line2di     m_cLine;

        irr::scene::ICameraSceneNode *m_pCamera;

      public:
        CMyCameraAnimator(irr::IrrlichtDevice *a_pDevice);

        virtual void animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 a_iTimeMs);
        virtual ISceneNodeAnimator *createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pNewManager = 0);
        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getType () const;
        virtual bool hasFinished(void) const;
        virtual bool isEventReceiverEnabled() const;
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        irr::f32 getAngleV() { return m_fAngleV; }
        irr::f32 getAngleH() { return m_fAngleH; }

        void setAngleV(irr::f32 a_fAngleV) { m_fAngleV = a_fAngleV; }
        void setAngleH(irr::f32 a_fAngleH) { m_fAngleH = a_fAngleH; }

        void setMousePosition(const irr::core::position2di a_cPos) {
        }

        void copyPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam, irr::scene::ICameraSceneNode *a_pOther);
        void setPositionLookAt(irr::scene::ICameraSceneNode *a_pCam, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cLookAt);

        void setData(const irr::core::vector3df& a_cPos, irr::f32 a_fAngleV, irr::f32 a_fAngleH);
    };
  }
}
