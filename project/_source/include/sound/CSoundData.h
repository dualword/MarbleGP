// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <AL/alc.h>
#include <AL/al.h>
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
        ALCdevice  *m_pAlDevice ;
        ALCcontext *m_pALContext;
        bool        m_bInitialized;

      public:
        CAudioDevice();
        virtual ~CAudioDevice();

        ALCdevice  *getDevice ();
        ALCcontext *getContext();

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
        ALuint m_iBuffer;

      public:
        IAudioBuffer(const std::wstring &a_sName);
        virtual ~IAudioBuffer();

        void grab();
        bool drop();

        ALuint getBuffer();
        const std::wstring &getName();
        void setDeletionListener(IDeletionListener* a_pListener);
    };

    /**
    * @class CAudioBufferOggVorbis
    * @author Christian Keimel
    * The audio buffer implementation for ogg-vorbis audio files
    * @see IAudioBuffer
    */
    class CAudioBufferOggVorbis : public IAudioBuffer {
      public:
        CAudioBufferOggVorbis(const std::wstring &a_sName);
        virtual ~CAudioBufferOggVorbis();
    };

    /**
    * @class ISound
    * @author Christian Keimel
    * This interface must be implemented by all sounds used in the game
    */
    class ISound {
      protected:
        IAudioBuffer *m_pBuffer;
        ALuint        m_iSource;

      public:
        ISound(IAudioBuffer *a_pBuffer);
        virtual ~ISound();

        ALuint getSource();

        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void setPosition(const irr::core::vector3df &a_cPos) = 0;
        virtual void setVelocity(const irr::core::vector3df &a_cVel) = 0;
        virtual void setVolume(irr::f32 a_fVolume) = 0;
    };

    /**
    * @class CSound3d
    * @author Christian Keimel
    * This is the implementation of a 3d sound
    * @see ISound
    */
    class CSound3d : public ISound {
      private:
        bool m_bLoop,
             m_bPlaying;

      public:
        CSound3d(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist);
        virtual ~CSound3d();

        virtual void play();
        virtual void stop();
        virtual void setPosition(const irr::core::vector3df &a_cPos);
        virtual void setVelocity(const irr::core::vector3df &a_cVel);
        virtual void setVolume(irr::f32 a_fVolume);
    };

    /**
    * @class CSound3dFixed
    * @author Christian Keimel
    * This is a child class of CSound3d that has no velocity so that the Doppler effect does not apply
    * @see CSound3d
    */
    class CSound3dFixed : public CSound3d {
      public:
        CSound3dFixed(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist, const irr::core::vector3df &a_cPos);
        virtual ~CSound3dFixed();

        virtual void setPosition(const irr::core::vector3df &a_cPos);
        virtual void setVelocity(const irr::core::vector3df &a_cVel);
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