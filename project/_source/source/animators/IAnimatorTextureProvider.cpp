#include <animators/IAnimatorTextureProvider.h>

namespace dustbin {
  namespace animators {
    IAnimatorTextureProvider::IAnimatorTextureProvider() {
    }

    IAnimatorTextureProvider::~IAnimatorTextureProvider() {
    }

    IAnimatorTextureProvider *IAnimatorTextureProvider::getTextureProviderInstance() {
      return m_pInstance;
    }


    IAnimatorTextureProvider *IAnimatorTextureProvider::m_pInstance = nullptr;
  }
}