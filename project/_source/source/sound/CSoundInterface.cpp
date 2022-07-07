// (w) 2021 by Dustbin::Games / Christian Keimel
#include <sound/ISoundInterface.h>
#include <sound/CSoundData.h>
#include <CGlobal.h>
#include <iostream>
#include <AL/alc.h>
#include <fstream>
#include <AL/al.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace sound {
    #define __OTHER_MARBLE_SOUNDS 4
    #define __STUNNED_MARBLE_SOUNDS 2
    #define __RESPAWN_MARBLE_SOUNDS 2

    class CSoundInterface : public IAudioBuffer::IDeletionListener, public ISoundInterface {
      private:
        struct SMarbleSound {
          int       m_iMarble;      /**< ID of the marble these sounds blong to */
          bool      m_bActive;      /**< Is the sound active? */
          irr::f32  m_fDistance;    /**< Squared distance of the sound */
          irr::f32  m_fVolume;      /**< The current volume of the sound */
          irr::f32  m_aParam[4];    /**< The volume factor read from the parameters*/
          ISound   *m_pSounds[4];   /**< The sounds (only the first sound is used for stunned and respawn) */

          SMarbleSound() : m_iMarble(0), m_bActive(false), m_fDistance(0.0f), m_fVolume(0.0f) {
          }
        };

        irr::f32              m_fSoundtrackVolume,
                              m_fSoundFXVolumeMenu,
                              m_fSoundFXVolumeGame,
                              m_fMasterVolume;
        enSoundTrack          m_eSoundTrack;
        irr::core::vector3df  m_vPosListener;
        irr::core::vector3df  m_vVelListener;
        CAudioDevice         *m_pDevice;
        ISound               *m_pSoundTrack;
        bool                  m_bMenu;    /**< Are we currently in the menu or game? */

        SMarbleSound m_aOthers      [__OTHER_MARBLE_SOUNDS  ];
        SMarbleSound m_aStunned     [__STUNNED_MARBLE_SOUNDS];
        SMarbleSound m_aRespawnStart[__RESPAWN_MARBLE_SOUNDS];
        SMarbleSound m_aRespawnDone [__RESPAWN_MARBLE_SOUNDS];

        int m_iNextStunned;       /**< The next stunned sound to be played */
        int m_iNextRespawnStart;  /**< The next respawn start sound to be played */
        int m_iNextRespawnDone;   /**< The next respawn done sound to be played */

        /**
        * Volume of the sounds. Key = path to file, Value = volume
        */
        std::map<std::wstring, irr::f32      > m_mSoundParameters;
        std::map<std::wstring, IAudioBuffer *> m_mAudioBuffer;

        std::map<std::wstring, ISound *> m_m2dSounds;
        std::map<enSoundTrack, ISound *> m_mSoundTracks;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs) : m_bMenu(true), m_iNextStunned(0), m_iNextRespawnStart(0), m_iNextRespawnDone(0) {
          m_vVelListener = irr::core::vector3df(0.0f, 0.0f, 0.0f);
          m_eSoundTrack  = enSoundTrack::enStNone;
          m_pSoundTrack  = nullptr;

          dustbin::CGlobal *l_pGlobal = CGlobal::getInstance();

          m_fMasterVolume      = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxMaster );
          m_fSoundtrackVolume  = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSoundTrack);
          m_fSoundFXVolumeGame = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxGame   );
          m_fSoundFXVolumeMenu = (irr::f32)(CGlobal::getInstance()->getSettingData().m_fSfxMenu   );

          if (a_pFs->existFile("data/marblesounds.xml")) {
            irr::io::IXMLReader *l_pReader = a_pFs->createXMLReader("data/marblesounds.xml");

            if (l_pReader != nullptr) {
              while (l_pReader->read()) {
                std::wstring l_sNodeName = l_pReader->getNodeName();

                if (l_sNodeName == L"sound") {
                  std::wstring l_sName = L"";

                  irr::f32 l_fMinDist =  0.0f,
                    l_fMaxDist = 25.0f,
                    l_fVolume  =  1.0f;

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
                    printf("SFX Param: \"%ls\": %.2f, %.2f, %.2f\n", l_sName.c_str(), l_fVolume, l_fMinDist, l_fMaxDist);
                    m_mSoundParameters[l_sName] = l_fVolume;
                  }
                }
              }
              l_pReader->drop();
            }
          }

          m_pDevice = new CAudioDevice();
        }

        virtual ~CSoundInterface() {
          for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
            for (int j = 0; j < 4; j++)
              if (m_aOthers[i].m_pSounds[j] != nullptr) delete m_aOthers[i].m_pSounds[j];
          }

          for (int i = 0; i < __STUNNED_MARBLE_SOUNDS; i++) {
            if (m_aStunned[i].m_pSounds[0] != nullptr) delete m_aStunned[i].m_pSounds[0];
          }

          for (int i = 0; i < __RESPAWN_MARBLE_SOUNDS; i++) {
            if (m_aRespawnStart[i].m_pSounds[0] != nullptr) delete m_aRespawnStart[i].m_pSounds[0];
            if (m_aRespawnDone [i].m_pSounds[0] != nullptr) delete m_aRespawnDone [i].m_pSounds[0];
          }

          for (std::map<std::wstring, ISound *>::iterator it = m_m2dSounds.begin(); it != m_m2dSounds.end(); it++) {
            it->second->stop();
            delete it->second;
          }

          m_m2dSounds.clear();

          for (std::map<enSoundTrack, ISound*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->stop();
            delete it->second;
          }

          m_mSoundTracks.clear();

          m_mSoundParameters.clear();
          delete m_pDevice;

          for (std::map<std::wstring, IAudioBuffer *>::iterator it = m_mAudioBuffer.begin(); it != m_mAudioBuffer.end(); it++)
            delete it->second;

          m_mAudioBuffer.clear();
        }

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) override {
          if (m_mAudioBuffer.find(a_sName) == m_mAudioBuffer.end()) {
            IAudioBuffer *p = new CAudioBufferOggVorbis(a_sName);
            p->setDeletionListener(this);
            m_mAudioBuffer[a_sName] = p;
          }
        }

        virtual void setMenuFlag(bool a_bMenu) override {
          m_bMenu = a_bMenu;
        }

        virtual void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) override {
        }

        virtual void setMasterVolume(irr::f32 a_fVolume) override {
          m_fMasterVolume = a_fVolume;
          if (m_pSoundTrack != nullptr)
            m_pSoundTrack->setVolume(0.5f * m_fMasterVolume * m_fSoundtrackVolume);
        }

        virtual void setSfxVolumeGame(irr::f32 a_fVolume) override {
          m_fSoundFXVolumeGame = a_fVolume;
        }

        virtual void setSfxVolumeMenu(irr::f32 a_fVolume) override {
          m_fSoundFXVolumeMenu = a_fVolume;
        }

        virtual void setSoundtrackVolume(irr::f32 a_fVolume) override {
          m_fSoundtrackVolume = a_fVolume;
          if (m_pSoundTrack != nullptr)
            m_pSoundTrack->setVolume(0.5f * m_fMasterVolume * m_fSoundtrackVolume);
        }

        virtual void muteAudio() override {
          m_pDevice->mute();
        }

        virtual void unmuteAudio() override {
          m_pDevice->unmute();
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
            printf("Start Soundtrack: %s\n", a_eSoundTrack == enSoundTrack::enStMenu ? "Menu" : a_eSoundTrack == enSoundTrack::enStRace ? "Race" : a_eSoundTrack == enSoundTrack::enStFinish ? "Finish" : "None");
            if (m_pSoundTrack != nullptr)
              m_pSoundTrack->stop();

            m_eSoundTrack = a_eSoundTrack;

            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_pSoundTrack = m_mSoundTracks[m_eSoundTrack];
            else
              m_pSoundTrack = nullptr;


            if (m_pSoundTrack != nullptr) {
              m_pSoundTrack->setVolume(0.5f * m_fMasterVolume * m_fSoundtrackVolume);
              m_pSoundTrack->play();
            }
          }
        }

        virtual void setSoundtrackFade(irr::f32 a_fValue) override {
          if (m_pSoundTrack != nullptr) {
            m_pSoundTrack->setVolume((a_fValue > 0.0f ? a_fValue : 0.0f) * 0.5f * m_fMasterVolume * m_fSoundtrackVolume);
          }
        }


        /**
        * Start a game, i.e. the sounds are initialized
        */
        virtual void startGame() override {
          for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
            if (m_aOthers[i].m_pSounds[0] == nullptr) m_aOthers[i].m_pSounds[0] = new CSound3d(m_mAudioBuffer[L"data/sounds/rolling.ogg"], true , 1.0f, 25, 250);
            if (m_aOthers[i].m_pSounds[1] == nullptr) m_aOthers[i].m_pSounds[1] = new CSound3d(m_mAudioBuffer[L"data/sounds/wind.ogg"   ], true , 1.0f, 25, 250);
            if (m_aOthers[i].m_pSounds[2] == nullptr) m_aOthers[i].m_pSounds[2] = new CSound3d(m_mAudioBuffer[L"data/sounds/skid.ogg"   ], true , 1.0f, 25, 250);
            if (m_aOthers[i].m_pSounds[3] == nullptr) m_aOthers[i].m_pSounds[3] = new CSound3d(m_mAudioBuffer[L"data/sounds/hit.ogg"    ], false, 1.0f, 25, 250);

            if (m_mSoundParameters.find(L"data/sounds/rolling.ogg") != m_mSoundParameters.end()) m_aOthers[i].m_aParam[0] = m_mSoundParameters[L"data/sounds/rolling.ogg"]; else m_aOthers[i].m_aParam[0] = 1.0f;
            if (m_mSoundParameters.find(L"data/sounds/wind.ogg"   ) != m_mSoundParameters.end()) m_aOthers[i].m_aParam[1] = m_mSoundParameters[L"data/sounds/wind.ogg"   ]; else m_aOthers[i].m_aParam[1] = 1.0f;
            if (m_mSoundParameters.find(L"data/sounds/skid.ogg"   ) != m_mSoundParameters.end()) m_aOthers[i].m_aParam[2] = m_mSoundParameters[L"data/sounds/skid.ogg"   ]; else m_aOthers[i].m_aParam[2] = 1.0f;
            if (m_mSoundParameters.find(L"data/sounds/hit.ogg"    ) != m_mSoundParameters.end()) m_aOthers[i].m_aParam[3] = m_mSoundParameters[L"data/sounds/hit.ogg"    ]; else m_aOthers[i].m_aParam[2] = 1.0f;
          }

          for (int i = 0; i < __STUNNED_MARBLE_SOUNDS; i++) {
            if (m_aStunned[i].m_pSounds[0] == nullptr) m_aStunned[i].m_pSounds[0] = new CSound3d(m_mAudioBuffer[L"data/sounds/stunned.ogg"], true, 0.0f, 25, 250);
            m_aStunned[i].m_iMarble = -1;
          }

          for (int i = 0; i < __RESPAWN_MARBLE_SOUNDS; i++) {
            if (m_aRespawnStart[i].m_pSounds[0] == nullptr) m_aRespawnStart[i].m_pSounds[0] = new CSound3d(m_mAudioBuffer[L"data/sounds/respawn_start.ogg"], false, 1.0f, 25, 250);
            if (m_aRespawnDone [i].m_pSounds[0] == nullptr) m_aRespawnDone [i].m_pSounds[0] = new CSound3d(m_mAudioBuffer[L"data/sounds/respawn.ogg"      ], false, 1.0f, 25, 250);
          }
        }

        /**
        * Stop a game, i.e. the sounds are muted
        */
        virtual void stopGame() override {
          for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
            for (int j = 0; j < 4; j++) {
              if (m_aOthers[i].m_pSounds[j] != nullptr) m_aOthers[i].m_pSounds[j]->stop();
            }
          }

          for (int i = 0; i < __STUNNED_MARBLE_SOUNDS; i++) {
            if (m_aStunned[i].m_pSounds[0] != nullptr) m_aStunned[i].m_pSounds[0]->stop();
          }

          for (int i = 0; i < __RESPAWN_MARBLE_SOUNDS; i++) {
            if (m_aRespawnStart[i].m_pSounds[0] != nullptr) m_aRespawnStart[i].m_pSounds[0]->stop();
            if (m_aRespawnDone [i].m_pSounds[0] != nullptr) m_aRespawnDone [i].m_pSounds[0]->stop();
          }

          m_iNextStunned      = 0;
          m_iNextRespawnStart = 0;
          m_iNextRespawnDone  = 0;
        }

        /**
        * Pause or unpause a game, i.e. in-game sounds are muted or unmuted
        */
        virtual void pauseGame(bool a_bPaused) override {
          if (a_bPaused) {
            for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
              for (int j = 0; j < 4; j++) {
                if (m_aOthers[i].m_pSounds[j] != nullptr) m_aOthers[i].m_pSounds[j]->setVolume(0.0f);
              }
            }
          }
          else {
            for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
              for (int j = 0; j < 4; j++) {
                if (m_aOthers[i].m_pSounds[j] != nullptr) m_aOthers[i].m_pSounds[j]->setVolume(m_aOthers[i].m_fVolume);
              }
            }
          }
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
        virtual void playMarbleSounds(int a_iMarble, const irr::core::vector3df& a_cPosition, const irr::core::vector3df& a_vVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
          irr::f32 l_fDistSq = a_cPosition.getDistanceFromSQ(m_vPosListener);

          // printf("%.2f\n", a_fHit);

          for (int i = 0; i < __OTHER_MARBLE_SOUNDS; i++) {
            if (!m_aOthers[i].m_bActive || m_aOthers[i].m_fDistance > l_fDistSq || m_aOthers[i].m_iMarble == a_iMarble) {
              if (m_aOthers[i].m_iMarble != a_iMarble)
                printf("%i: %.2f, %.2f\n", a_iMarble, std::sqrt(l_fDistSq), a_fVolume);

              for (int j = 0; j < 4; j++) {
                if (m_aOthers[i].m_pSounds[j] != nullptr) {
                  m_aOthers[i].m_pSounds[j]->setPosition(a_cPosition);
                  m_aOthers[i].m_pSounds[j]->setVelocity(a_vVelocity); 

                  if (j == 0)
                    m_aOthers[i].m_pSounds[0]->setVolume(a_bHasContact ? m_aOthers[i].m_aParam[0] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f); 
                  else if (j == 1)
                    m_aOthers[i].m_pSounds[1]->setVolume(m_aOthers[i].m_aParam[1] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume);
                  else if (j == 2)
                    m_aOthers[i].m_pSounds[2]->setVolume(a_bBrake && a_bHasContact ? m_aOthers[i].m_aParam[2] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f);
                  else if (j == 3 && a_fHit > 0.0f) {
                    printf("==> %.2f\n", a_fHit);
                    m_aOthers[i].m_pSounds[3]->setVolume(a_fHit);
                  }

                  m_aOthers[i].m_pSounds[j]->play();
                }
              }

              m_aOthers[i].m_bActive   = true;
              m_aOthers[i].m_fDistance = l_fDistSq;
              m_aOthers[i].m_fVolume   = a_fVolume;
              m_aOthers[i].m_iMarble   = a_iMarble;

              break;
            }
          }
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df& a_cPosition) override {
          irr::f32 l_fDistSq = a_cPosition.getDistanceFromSQ(m_vPosListener);

          if (l_fDistSq < 62500) {
            for (int i = 0; i < __STUNNED_MARBLE_SOUNDS; i++) {
              if (m_aStunned[i].m_iMarble == -1) {
                printf("Stunned sound %i: %i [1]\n", i, a_iMarble);

                m_aStunned[i].m_iMarble = a_iMarble;
                m_aStunned[i].m_pSounds[0]->setPosition(a_cPosition);
                m_aStunned[i].m_pSounds[0]->setVolume(1.0f * m_mSoundParameters[L"data/sounds/stunned.ogg"]);
                m_aStunned[i].m_pSounds[0]->play();

                return;
              }
            }

            printf("Stunned sound %i: %i [2]\n", m_iNextStunned, a_iMarble);
            if (m_aStunned[m_iNextStunned].m_pSounds[0] != nullptr) {
              m_aStunned[m_iNextStunned].m_iMarble = a_iMarble;
              m_aStunned[m_iNextStunned].m_pSounds[0]->setPosition(a_cPosition);
              m_aStunned[m_iNextStunned].m_pSounds[0]->setVolume(1.0f * m_mSoundParameters[L"data/sounds/stunned.ogg"]);
              m_aStunned[m_iNextStunned].m_pSounds[0]->play();
            }

            m_iNextStunned++;
            if (m_iNextStunned >= __STUNNED_MARBLE_SOUNDS) m_iNextStunned = 0;
          }
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) override {
          for (int i = 0; i < __STUNNED_MARBLE_SOUNDS; i++) {
            if (m_aStunned[i].m_iMarble == a_iMarble && m_aStunned[i].m_pSounds[0] != nullptr) {
              m_aStunned[i].m_pSounds[0]->setVolume(0.0f);
              m_aStunned[i].m_iMarble = -1;
            }
          }
        }

        /**
        * Play the first respawn sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the respawning marble
        * @param a_cPosition position of the respawning marble
        */
        virtual void playMarbleRespawnStart(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          irr::f32 l_fDistSq = a_cPosition.getDistanceFromSQ(m_vPosListener);

          if (l_fDistSq < 62500) {
            printf("Respawn start sound %i: %i\n", m_iNextRespawnStart, a_iMarble);
            if (m_aRespawnStart[m_iNextRespawnStart].m_pSounds[0] != nullptr) {
              m_aRespawnStart[m_iNextRespawnStart].m_iMarble = a_iMarble;
              m_aRespawnStart[m_iNextRespawnStart].m_pSounds[0]->setPosition(a_cPosition);
              m_aRespawnStart[m_iNextRespawnStart].m_pSounds[0]->setVolume(1.0f * m_mSoundParameters[L"data/sounds/respawn_start.ogg"]);
              m_aRespawnStart[m_iNextRespawnStart].m_pSounds[0]->play();
            }

            m_iNextRespawnStart++;
            if (m_iNextRespawnStart >= __RESPAWN_MARBLE_SOUNDS) m_iNextRespawnStart = 0;
          }
        }

        /**
        * Play the second respawn sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the respawning marble
        * @param a_cPosition position of the respawning marble
        */
        virtual void playMarbleRespawnDone(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          irr::f32 l_fDistSq = a_cPosition.getDistanceFromSQ(m_vPosListener);

          if (l_fDistSq < 62500) {
            printf("Respawn done sound %i: %i\n", m_iNextRespawnDone, a_iMarble);
            if (m_aRespawnDone[m_iNextRespawnDone].m_pSounds[0] != nullptr) {
              m_aRespawnDone[m_iNextRespawnDone].m_iMarble = a_iMarble;
              m_aRespawnDone[m_iNextRespawnDone].m_pSounds[0]->setPosition(a_cPosition);
              m_aRespawnDone[m_iNextRespawnDone].m_pSounds[0]->setVolume(1.0f * m_mSoundParameters[L"data/sounds/respawn.ogg"]);
              m_aRespawnDone[m_iNextRespawnDone].m_pSounds[0]->play();
            }

            m_iNextRespawnDone++;
            if (m_iNextRespawnDone>= __RESPAWN_MARBLE_SOUNDS) m_iNextRespawnDone = 0;
          }
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

        virtual void play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan) override {
          if (m_m2dSounds.find(a_sName) == m_m2dSounds.end()) {
            if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
              ISound* p = new CSound2d(m_mAudioBuffer[a_sName], false);
              m_m2dSounds[a_sName] = p;
            }
          }

          if (m_m2dSounds.find(a_sName) != m_m2dSounds.end()) {
            m_m2dSounds[a_sName]->setVolume(m_fMasterVolume * (m_bMenu ? m_fSoundFXVolumeMenu : m_fSoundFXVolumeGame) * a_fVolume);
            m_m2dSounds[a_sName]->setPosition(irr::core::vector3df(0.0f, a_fPan, 0.0f));
            m_m2dSounds[a_sName]->play();
          }
        }

        virtual void clear3dSounds() override {
        }

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) override {
          m_vVelListener = a_vVel;
          m_vPosListener = a_pCamera->getAbsolutePosition();
          m_pDevice->updateListener(a_pCamera, m_vVelListener);
        }

        virtual void bufferDeleted(IAudioBuffer* a_pBuffer) override {
          if (m_mAudioBuffer.find(a_pBuffer->getName()) != m_mAudioBuffer.end()) {
            m_mAudioBuffer.erase(a_pBuffer->getName());
          }
          delete a_pBuffer;
        }

        virtual void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) override {
          for (std::map<enSoundTrack, std::wstring>::const_iterator it = a_mSoundTracks.begin(); it != a_mSoundTracks.end(); it++) {
            if (m_mAudioBuffer.find(it->second) != m_mAudioBuffer.end()) {
              ISound *p = new CSound2d(m_mAudioBuffer[it->second], it->second.find(L"_result") == std::string::npos);
              m_mSoundTracks[it->first] = p;
            }
          }
        }
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice) {
      return new CSoundInterface(a_pDevice->getFileSystem());
    }
  }
}
