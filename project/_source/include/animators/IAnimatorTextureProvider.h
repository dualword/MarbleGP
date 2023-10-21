#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace animators {
    /**
    * @class IAnimatorTextureProvider
    * @author Christian Keimel
    * The interface which provides the textures for the MarbleGP texture animator
    */
    class IAnimatorTextureProvider {
      protected:
        static IAnimatorTextureProvider *m_pInstance;

      public:
        IAnimatorTextureProvider();
        virtual ~IAnimatorTextureProvider();

        /**
        * Get the texture with the race information
        * @return the texture with the race information
        */
        virtual irr::video::ITexture *getRaceInfoTexture() = 0;

        /**
        * Get the texture for the race counter
        * @return the texture for the race counter
        */
        virtual irr::video::ITexture *getLapCountTexture() = 0;

        /**
        * Get the instance
        * @return the instance
        */
        static IAnimatorTextureProvider *getTextureProviderInstance();
    };
  }
}
