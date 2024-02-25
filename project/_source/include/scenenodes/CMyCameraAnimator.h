// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
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
        irr::core::vector2di   m_vPosition;
        irr::core::vector3df   m_vCamLookAt,
                               m_vCamPos;
        irr::IrrlichtDevice   *m_pDevice;
        bool                   m_bLeftDown,
                               m_bRightDown,
                               m_bActive,
                               m_bLoaded;
        irr::f32               m_fAngleV,
                               m_fAngleH;
        irr::core::position2di m_cMouse;      /**< Mouse position for touch control */

        irr::scene::ICameraSceneNode *m_pCamera;  /**< The animated camera */

      public:
        CMyCameraAnimator(irr::IrrlichtDevice *a_pDevice);

        virtual void animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 a_iTimeMs);
        virtual ISceneNodeAnimator *createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pNewManager = 0);
        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getType () const;
        virtual bool hasFinished(void) const;
        virtual bool isEventReceiverEnabled() const;
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        void setData(const irr::core::vector3df &a_cPos, irr::f32 a_fAngleV, irr::f32 a_fAngleH);

        void setMousePosition(const irr::core::position2di a_cPos) {
          m_vPosition.X = a_cPos.X;
          m_vPosition.Y = a_cPos.Y;
        }

        void initPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam);
    };
  }
}
