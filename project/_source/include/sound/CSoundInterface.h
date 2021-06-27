// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <sound/CSoundEnums.h>
#include <sound/CSoundData.h>
#include <string>
#include <vector>
#include <map>

namespace dustbin {
  namespace audio {
    /**
    * @class CSoundInterface
    * @author Christian Keimel
    * This is the central management class for all sounds in the game
    */
    class CSoundInterface : public IAudioBuffer::IDeletionListener {
      private:
        irr::f32              m_fSoundtrackVolume,
                              m_fSoundFXVolume;
        enSoundTrack          m_eSoundTrack;
        irr::core::vector3df  m_vVelListener;
        CAudioDevice         *m_pDevice;
        ISound               *m_pSoundTrack;
        bool                  m_bMuteSfx;

        /*
        Sound Parameters.
        Tuple Members:
        0: Volume
        1: Min Distance
        2: Max Distance
        */
        std::map<std::wstring, std::tuple<irr::f32, irr::f32, irr::f32> > m_mSoundParameters;
        std::map<std::wstring, IAudioBuffer                            *> m_mAudioBuffer;

        std::vector<ISound *> m_vNoDopplerSounds;

        std::map                   <std::wstring, ISound *>   m_m2dSounds;
        std::map<irr::s32, std::map<std::wstring, ISound *> > m_m3dSounds;
        std::map<enSoundTrack                   , ISound *>   m_mSoundTracks;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs);
        ~CSoundInterface();

        void createSoundFileFactory(irr::io::IFileArchive *a_pArchive);

        void setSfxVolume(irr::f32 a_fVolume);
        void setSoundtrackVolume(irr::f32 a_fVolume);
        void muteAudio();
        void unmuteAudio();
        void muteSoundFX(bool a_bMute);
        irr::f32 getSoundtrackVolume();
        irr::f32 getSfxVolume();

        void startSoundtrack(enSoundTrack a_eSoundTrack);
        void setSoundtrackFade(irr::f32 a_fValue);

        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bLooped);
        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, irr::f32 a_fVolume, bool a_bLooped);
        void play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan);
        void clear3dSounds();

        void addSoundParameter(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fMinDistance, irr::f32 a_fMaxDistance);

        void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel);

        void preloadSound(const std::wstring& a_sName);
        void assignSound (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, bool a_bDoppler);
        void assignFixed (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, const irr::core::vector3df &a_vPos);

        void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks);

        virtual void bufferDeleted(IAudioBuffer* a_pBuffer);
      };
  }
}