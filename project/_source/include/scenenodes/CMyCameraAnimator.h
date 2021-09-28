// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <string>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CMyCameraAnimator
    * @author Christian Keimel
    * This is the custom scene node animator for the "view track" camera
    */
    class CMyCameraAnimator : public irr::scene::ISceneNodeAnimator {
      private:
        irr::core::vector2di  m_vPosition;
        irr::core::vector3df  m_vCamLookAt,
                              m_vCamPos;
        irr::IrrlichtDevice  *m_pDevice;
        bool                  m_bLeftDown,
                              m_bRightDown,
                              m_bActive,
                              m_bLoaded;
        irr::f32              m_fAngleV,
                              m_fAngleH;

      public:
        CMyCameraAnimator(irr::IrrlichtDevice *a_pDevice);

        virtual void animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 m_iTimeMs);
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
          m_vPosition.X = a_cPos.X;
          m_vPosition.Y = a_cPos.Y;
        }

        void initPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam);
        void copyPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam, irr::scene::ICameraSceneNode *a_pOther);
        void setPositionLookAt(irr::scene::ICameraSceneNode *a_pCam, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cLookAt);

        void setIsActive(bool a_bIsActive);
        bool isActive();

        std::string saveData();
        void loadData(const std::string &a_sData, irr::core::vector3df &a_cPos, irr::core::vector3df &a_cTgt);
    };
  }
}
