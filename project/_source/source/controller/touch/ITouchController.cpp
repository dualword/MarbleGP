// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <controller/touch/ITouchController.h>

namespace dustbin {
  namespace controller {
    ITouchController::SControl::SControl() : m_pTextureOff(nullptr), m_pTextureOn(nullptr), m_bTouched(false), m_bActive(true), m_iTouchID(-1) {
    }

    ITouchController::SControl::SControl(
      const irr::core::recti &a_cDestination,
      const irr::core::recti &a_cSource,
      const irr::video::SColor &a_cBackground,
      irr::video::ITexture *a_pOff,
      irr::video::ITexture *a_pOn
    ) :
      m_pTextureOff(a_pOff),
      m_pTextureOn (a_pOn),
      m_bTouched   (false),
      m_bActive    (true),
      m_iTouchID   (-1)
    {
      m_cDestination = a_cDestination;
      m_cSource      = a_cSource;
      m_cBackground  = a_cBackground;
    }

    ITouchController::ITouchController(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) : m_pDrv(a_pDrv), m_cViewport(a_cViewport) {
    }

    ITouchController::~ITouchController() {
    }
  }
}