// (w) 2024 by Dustbin::Games / Christian Keimel
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    IGlobal::IGlobal() {
      m_pInstance = this;
    }

    IGlobal::~IGlobal() {
      m_pInstance = nullptr;
    }

    IGlobal *IGlobal::getInstance() {
      return m_pInstance;
    }

    IGlobal *IGlobal::m_pInstance = nullptr;
  }
}