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
        virtual irr::f32 getMasterVolume() = 0;
        virtual irr::f32 getSfxVolumeGame() = 0;
        virtual irr::f32 getSfxVolumeMenu() = 0;
        virtual irr::f32 getSoundtrackVolume() = 0;

        virtual void startSoundtrack(enSoundTrack a_eSoundTrack) = 0;
        virtual void setSoundtrackFade(irr::f32 a_fValue) = 0;

        /**
        * Start a game, i.e. the sounds are initialized
        */
        virtual void startGame() = 0;

        /**
        * Stop a game, i.e. the sounds are muted
        */
        virtual void stopGame() = 0;

        /**
        * Pause or unpause a game, i.e. in-game sounds are muted or unmuted
        */
        virtual void pauseGame(bool a_bPaused) = 0;

        /**
        * Update the sounds of a marble. Only the closest sounds to the listener
        * are played
        * @param a_iMarble ID of the marble to update
        * @param a_cPosition the position of the marble
        * @param a_cVelocity the velocity of the marble
        * @param a_fHit the volume of the "hit" sound for the marble
        * @param a_fVolume the volume of the wind and rolling sounds calculated from the speed of the marble
        * @param a_bBrake does the marble currently brake?
        * @param a_HasContact does the marble have a contact?
        */
        virtual void playMarbleSounds(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) = 0;

        /**
        * Play the sounds of a marble assigned to a viewport
        * @param a_iMarble ID of the marble to update
        * @param a_cPosition the position of the marble
        * @param a_cVelocity the velocity of the marble
        * @param a_fVolume the volume of the wind and rolling sounds calculated from the speed of the marble
        * @param a_bBrake does the marble currently brake?
        * @param a_HasContact does the marble have a contact?
        */
        virtual void playViewportMarbleSound(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) = 0;

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df &a_cPosition) = 0;

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) = 0;

        /**
        * Play a specific sound for a marble, i.e. checkpoint, lap passed
        * @param a_iMarble ID of the marble
        * @param a_eSound the sound to play
        */
        virtual void playMarbleOneShotSound(int a_iMarble, enOneShots a_eSound) = 0;

        /**
        * Define a marble controlled by a local player
        */
        virtual void setViewportMarble(int a_iMarble) = 0;
        
        virtual void play2d(en2dSounds a_eSound, irr::f32 a_fVolume, irr::f32 a_fPan) = 0;
        virtual void clear3dSounds() = 0;

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) = 0;

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) = 0;
        virtual void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) = 0;

        virtual void setMenuFlag(bool a_bMenu) = 0;
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice);
  }
}

