#ifdef _RASPI

// (w) 2021 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <sound/CSoundData.h>
#include <CGlobal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace sound {
    class CSoundInterface : public IAudioBuffer::IDeletionListener, public ISoundInterface {
      private:
        irr::f32              m_fSoundtrackVolume;
        irr::f32              m_fSoundFXVolumeMenu;
        irr::f32              m_fSoundFXVolumeGame;
        irr::f32              m_fViewportVolume;
        irr::f32              m_fMasterVolume;
        enSoundTrack          m_eSoundTrack;
        irr::core::vector3df  m_vPosListener;
        irr::core::vector3df  m_vVelListener;
        bool                  m_bMenu;                /**< Are we currently in the menu or game? */

        /**
        * Volume of the sounds. Key = path to file, Value = volume
        */
        std::map<std::string, irr::f32> m_mSoundParameters;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs)/* : m_bMenu(true)*/ {
          m_vVelListener = irr::core::vector3df(0.0f, 0.0f, 0.0f);
          m_eSoundTrack  = enSoundTrack::enStNone;

          dustbin::CGlobal *l_pGlobal = CGlobal::getInstance();

          m_fMasterVolume      = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxMaster );
          m_fSoundtrackVolume  = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSoundTrack);
          m_fSoundFXVolumeGame = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxGame   );
          m_fSoundFXVolumeMenu = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxMenu   );
          m_fViewportVolume    = 0.6f;
          
          if (a_pFs->existFile("data/marblesounds.xml")) {
            irr::io::IXMLReader *l_pReader = a_pFs->createXMLReader("data/marblesounds.xml");

            if (l_pReader != nullptr) {
              while (l_pReader->read()) {
                std::wstring l_sNodeName = l_pReader->getNodeName();

                if (l_sNodeName == L"sound") {
                  std::wstring l_sName = L"";

                  irr::f32 l_fVolume  =  1.0f;

                  for (unsigned i = 0; i < l_pReader->getAttributeCount(); i++) {
                    std::wstring l_sAttr  = l_pReader->getAttributeName (i),
                      l_sValue = l_pReader->getAttributeValue(i);

                    if (l_sAttr == L"id") {
                      l_sName = l_sValue;
                    }
                    else if (l_sAttr == L"vol") {
                      l_fVolume = std::stof(l_sValue);
                    }
                  }

                  if (l_sName != L"") {
                    m_mSoundParameters[helpers::ws2s(l_sName)] = l_fVolume;
                  }
                }
              }
              l_pReader->drop();
            }
          }
          
          std::string l_a2dSounds[] = {
            "data/sounds/button_hover.ogg",
            "data/sounds/button_press.ogg",
            "data/sounds/countdown.ogg",
            "data/sounds/countdown_go.ogg",
            "data/sounds/lap.ogg",
            ""
          };

          std::string l_aMarbleSounds[] = {
            "data/sounds/rolling.ogg",
            "data/sounds/wind.ogg",
            "data/sounds/skid.ogg",
            "data/sounds/stunned.ogg",
            ""
          };

          std::string l_aOneShots[] = {
            "data/sounds/respawn_start.ogg",
            "data/sounds/respawn.ogg",
            "data/sounds/hit.ogg",
            "data/sounds/checkpoint.ogg",
            "data/sounds/lap.ogg",
            "data/sounds/gameover.ogg",
            ""
          };
        }

        virtual ~CSoundInterface() {
          m_mSoundParameters.clear();
          
          printf("Sound Interface deleted.\n");
        }

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) override {
        }

        virtual void setMenuFlag(bool a_bMenu) override {
          m_bMenu = a_bMenu;
        }

        virtual void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) override {
        }

        virtual void setMasterVolume(irr::f32 a_fVolume) override {
          m_fMasterVolume = a_fVolume;
        }

        virtual void setSfxVolumeGame(irr::f32 a_fVolume) override {
          m_fSoundFXVolumeGame = a_fVolume;
        }

        virtual void setSfxVolumeMenu(irr::f32 a_fVolume) override {
          m_fSoundFXVolumeMenu = a_fVolume;
        }

        virtual void setSoundtrackVolume(irr::f32 a_fVolume) override {
          m_fSoundtrackVolume = a_fVolume;
        }

        virtual void muteAudio() override {
        }

        virtual void unmuteAudio() override {
        }

        virtual irr::f32 getSoundtrackVolume() override {
          return m_fSoundtrackVolume;
      }

        virtual irr::f32 getMasterVolume() override {
          return m_fMasterVolume;
        }

        virtual irr::f32 getSfxVolumeGame() override {
          return m_fSoundFXVolumeGame;
        }

        virtual irr::f32 getSfxVolumeMenu() override {
          return m_fSoundFXVolumeMenu;
        }

        virtual void startSoundtrack(enSoundTrack a_eSoundTrack) override {
          if (a_eSoundTrack != m_eSoundTrack) {
          }
        }

        virtual void setSoundtrackFade(irr::f32 a_fValue) override {
        }


        /**
        * Start a game, i.e. the sounds are initialized
        */
        virtual void startGame() override {
          std::string l_aMarbleSounds[] = {
            "data/sounds/rolling.ogg",
            "data/sounds/wind.ogg",
            "data/sounds/skid.ogg",
            "data/sounds/stunned.ogg",
            ""
          };
        }

        /**
        * Stop a game, i.e. the sounds are muted
        */
        virtual void stopGame() override {
        }

        /**
        * Pause or unpause a game, i.e. in-game sounds are muted or unmuted
        */
        virtual void pauseGame(bool a_bPaused) override {
        }

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
        virtual void playMarbleSounds(int a_iMarble, const irr::core::vector3df& a_cPosition, const irr::core::vector3df& a_cVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
        }

        /**
        * Play a specific sound for a marble, i.e. checkpoint, lap passed and game over
        * @param a_iMarble ID of the marble
        * @param a_eSound the sound to play
        */
        virtual void playMarbleOneShotSound(int a_iMarble, enOneShots a_eSound) override {
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df& a_cPosition) override {
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) override {
        }


        /**
        * Play the sounds of a marble assigned to a viewport
        * @param a_iMarble ID of the marble to update
        * @param a_cPosition the position of the marble
        * @param a_cVelocity the velocity of the marble
        * @param a_fVolume the volume of the wind and rolling sounds calculated from the speed of the marble
        * @param a_bBrake does the marble currently brake?
        * @param a_HasContact does the marble have a contact?
        */
        virtual void playViewportMarbleSound(int a_iMarble, const irr::core::vector3df& a_cPosition, const irr::core::vector3df& a_vVelocity, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
        }

        virtual void play2d(en2dSounds a_eSound, irr::f32 a_fVolume, irr::f32 a_fPan) override {
        }

        /**
        * Define a marble controlled by a local player
        */
        virtual void setViewportMarble(int a_iMarble) override {
        }

        virtual void clear3dSounds() override {
        }

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) override {
          m_vVelListener = a_vVel;
          m_vPosListener = a_pCamera->getAbsolutePosition();

          irr::core::vector3df l_cPos = a_pCamera->getAbsolutePosition();
          irr::core::vector3df l_cTgt = a_pCamera->getTarget          ();
          irr::core::vector3df l_cUp  = a_pCamera->getUpVector        ();
        }

        virtual void bufferDeleted(IAudioBuffer* a_pBuffer) override {
        }

        virtual void assignSoundtracks(const std::map<enSoundTrack, std::tuple<std::string, bool>> &a_mSoundTracks) override {
        }

        virtual void stopEverything() override {
        }
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice) {
      return new CSoundInterface(a_pDevice->getFileSystem());
    }
  }
}

#endif
