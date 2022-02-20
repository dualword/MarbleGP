// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <sound/CSoundEnums.h>
#include <string>
#include <map>

namespace dustbin {
  namespace sound {
    /**
    * @class ISoundInterface
    * @author Christian Keimel
    * This interface hides the actual implementations
    * for audio output
    */
    class ISoundInterface {
      public:
        virtual ~ISoundInterface() { }

        virtual void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) = 0;

        virtual void setMasterVolume(irr::f32 a_fVolume) = 0;
        virtual void setSfxVolumeGame(irr::f32 a_fVolume) = 0;
        virtual void setSfxVolumeMenu(irr::f32 a_fVolume) = 0;
        virtual void setSoundtrackVolume(irr::f32 a_fVolume) = 0;
        virtual void muteAudio() = 0;
        virtual void unmuteAudio() = 0;
        virtual irr::f32 getMasterColume() = 0;
        virtual irr::f32 getSfxVolumeGame() = 0;
        virtual irr::f32 getSfxVolumeMenu() = 0;
        virtual irr::f32 getSoundtrackVolume() = 0;

        virtual void startSoundtrack(enSoundTrack a_eSoundTrack) = 0;
        virtual void setSoundtrackFade(irr::f32 a_fValue) = 0;

        virtual void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bLooped) = 0;
        virtual void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, irr::f32 a_fVolume, bool a_bLooped) = 0;
        virtual void play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan) = 0;
        virtual void clear3dSounds() = 0;

        virtual void addSoundParameter(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fMinDistance, irr::f32 a_fMaxDistance) = 0;

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) = 0;

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) = 0;
        virtual void assignSound (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, bool a_bDoppler) = 0;
        virtual void assignFixed (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, const irr::core::vector3df &a_vPos) = 0;

        virtual void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) = 0;

        virtual void setMenuFlag(bool a_bMenu) = 0;
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice);
  }
}

