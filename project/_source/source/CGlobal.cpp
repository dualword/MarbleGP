// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <CGlobal.h>

namespace dustbin {
  CGlobal::CGlobal() {
  }

  CGlobal::~CGlobal() {
  }

  CGlobal* CGlobal::getInstance() {
    return m_pInstance;
  }

  CGlobal *CGlobal::m_pInstance = nullptr;
}