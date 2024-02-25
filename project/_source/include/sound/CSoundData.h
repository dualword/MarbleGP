// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <string>

namespace dustbin {
  namespace sound {

    /**
    * @class CAudioDevice
    * @author Christian Keimel
    * This class is the central audio interface
    */
    class CAudioDevice {
      private:
        bool        m_bInitialized;

      public:
        CAudioDevice();
        virtual ~CAudioDevice();

        void updateListener(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVelocity);
        void mute();
        void unmute();
    };

    /**
    * @class IAudioBuffer
    * @author Christian Keimel
    * This is the interface for implementing an audio buffer. Every sound has an audio buffer,
    * but an audio buffer can be used by multiple sounds
    */
    class IAudioBuffer {
      public:
        class IDeletionListener {
          public:
            virtual void bufferDeleted(IAudioBuffer *a_pBuffer) = 0;
        };

      private:
        irr::s32           m_iReferenceCount;
        std::wstring       m_sName;
        IDeletionListener *m_pListener;

      protected:

      public:
        IAudioBuffer(const std::wstring &a_sName);
        virtual ~IAudioBuffer();

        void grab();
        bool drop();

        const std::wstring &getName();
        void setDeletionListener(IDeletionListener* a_pListener);
    };

    /**
    * @class ISound
    * @author Christian Keimel
    * This interface must be implemented by all sounds used in the game
    */
    class ISound {
      protected:
        IAudioBuffer *m_pBuffer;

      public:
        ISound(IAudioBuffer *a_pBuffer);
        virtual ~ISound();

        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void setPosition(const irr::core::vector3df &a_cPos) = 0;
        virtual void setVelocity(const irr::core::vector3df &a_cVel) = 0;
        virtual void setVolume(irr::f32 a_fVolume) = 0;
    };

    /**
    * @class CSound2d
    * @author Christian Keimel
    * This is the implementation of the ISound interface for 2d sounds
    * @see ISound
    */
    class CSound2d : public ISound {
      private:
        bool m_bLoop,
             m_bPlaying;

      public:
        CSound2d(IAudioBuffer* a_pBuffer, bool a_bLoop);
        virtual ~CSound2d();

        virtual void play();
        virtual void stop();
        virtual void setPosition(const irr::core::vector3df &a_cPos);
        virtual void setVelocity(const irr::core::vector3df &a_cVel);
        virtual void setVolume(irr::f32 a_fVolume);
    };

  }
}