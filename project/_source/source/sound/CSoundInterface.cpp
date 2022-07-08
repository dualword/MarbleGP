// (w) 2021 by Dustbin::Games / Christian Keimel
#define IRRKLANG_STATIC

#include <sound/ISoundInterface.h>
#include <sound/CSoundData.h>
#include <irrKlang.h>
#include <CGlobal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace dustbin {
  namespace sound {
    // An enumeration for the continuus marble sounds
    enum class enMarbleSounds {
      Rolling = 0,
      Wind    = 1,
      Skid    = 2,
      Stunned = 3,
      Count   = 4
    };

    // An enumeration for the one-shot sounds
    enum class enOneShots {
      RespawnStart,
      RespawnDone,
      Hit,
      Count
    };

    class CSoundInterface : public IAudioBuffer::IDeletionListener, public ISoundInterface {
      private:
        struct SMarbleSound {
          int       m_iMarble;                               /**< ID of the marble these sounds blong to */
          bool      m_bActive;                               /**< Is the sound active? */
          irr::f32  m_fDistance;                             /**< Squared distance of the sound */
          irr::f32  m_fVolume;                               /**< The current volume of the sound */
          irr::f32  m_aParam[(int)enMarbleSounds::Count];    /**< The volume factor read from the parameters*/

          irrklang::ISound *m_pSounds[(int)enMarbleSounds::Count];

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

        irrklang::ISoundEngine *m_pEngine;    /**< The irrKlang sound engine */

        SMarbleSound m_aMarbles[16];

        irrklang::ISoundSource *m_aOneShots[(int)enOneShots::Count];

        /**
        * Volume of the sounds. Key = path to file, Value = volume
        */
        std::map<std::wstring, irr::f32      > m_mSoundParameters;
        std::map<std::wstring, IAudioBuffer *> m_mAudioBuffer;

        std::map<std::wstring, ISound *> m_m2dSounds;
        std::map<enSoundTrack, ISound *> m_mSoundTracks;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs) : m_bMenu(true) {
          m_pEngine = irrklang::createIrrKlangDevice();

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

          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++)
              m_aMarbles[i].m_pSounds[j] = nullptr;
          }

          for (int i = 0; i < (int)enOneShots::Count; i++)
            m_aOneShots[i] = nullptr;

          m_pDevice = new CAudioDevice();
        }

        virtual ~CSoundInterface() {
          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++)
              if (m_aMarbles[i].m_pSounds[j] != nullptr) m_aMarbles[i].m_pSounds[j]->drop();
          }

          for (std::map<std::wstring, ISound *>::iterator it = m_m2dSounds.begin(); it != m_m2dSounds.end(); it++) {
            it->second->stop();
            delete it->second;
          }

          for (int i = 0; i < (int)enOneShots::Count; i++)
            if (m_aOneShots[i] != nullptr)
              m_aOneShots[i]->drop();

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

          m_pEngine->drop();
        }

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) override {
          if (m_mAudioBuffer.find(a_sName) == m_mAudioBuffer.end()) {
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
          for (int i = 0; i < 16; i++) {
            if (m_pEngine != nullptr) {
              if (m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Rolling] == nullptr) m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Rolling] = m_pEngine->play3D("data/sounds/rolling.ogg"      , irrklang::vec3d(0.0f, 0.0f, 0.0f), true , true);
              if (m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Wind   ] == nullptr) m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Wind   ] = m_pEngine->play3D("data/sounds/wind.ogg"         , irrklang::vec3d(0.0f, 0.0f, 0.0f), true , true);
              if (m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Skid   ] == nullptr) m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Skid   ] = m_pEngine->play3D("data/sounds/skid.ogg"         , irrklang::vec3d(0.0f, 0.0f, 0.0f), true , true);
              if (m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Stunned] == nullptr) m_aMarbles[i].m_pSounds[(int)enMarbleSounds::Stunned] = m_pEngine->play3D("data/sounds/stunned.ogg"      , irrklang::vec3d(0.0f, 0.0f, 0.0f), true , true);

              if (m_mSoundParameters.find(L"data/sounds/rolling.ogg") != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[(int)enMarbleSounds::Rolling] = m_mSoundParameters[L"data/sounds/rolling.ogg"]; else m_aMarbles[i].m_aParam[0] = 1.0f;
              if (m_mSoundParameters.find(L"data/sounds/wind.ogg"   ) != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[(int)enMarbleSounds::Wind   ] = m_mSoundParameters[L"data/sounds/wind.ogg"   ]; else m_aMarbles[i].m_aParam[1] = 1.0f;
              if (m_mSoundParameters.find(L"data/sounds/skid.ogg"   ) != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[(int)enMarbleSounds::Skid   ] = m_mSoundParameters[L"data/sounds/skid.ogg"   ]; else m_aMarbles[i].m_aParam[2] = 1.0f;
              if (m_mSoundParameters.find(L"data/sounds/hit.ogg"    ) != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[(int)enMarbleSounds::Stunned] = m_mSoundParameters[L"data/sounds/hit.ogg"    ]; else m_aMarbles[i].m_aParam[2] = 1.0f;

              for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
                if (m_aMarbles[i].m_pSounds[j] != nullptr) {
                  m_aMarbles[i].m_pSounds[j]->setMinDistance( 25.0f);
                  m_aMarbles[i].m_pSounds[j]->setMaxDistance(250.0f);
                  m_aMarbles[i].m_pSounds[j]->setVolume(0.0f);
                  m_aMarbles[i].m_pSounds[j]->setIsPaused(false);
                }
              }
            }
          }

          if (m_aOneShots[(int)enOneShots::RespawnStart] == nullptr) {
            m_aOneShots[(int)enOneShots::RespawnStart] = m_pEngine->addSoundSourceFromFile("data/sounds/respawn_start.ogg");
            m_aOneShots[(int)enOneShots::RespawnStart]->setDefaultMinDistance( 25.0f);
            m_aOneShots[(int)enOneShots::RespawnStart]->setDefaultMaxDistance(250.0f);
            m_aOneShots[(int)enOneShots::RespawnStart]->setDefaultVolume     (m_mSoundParameters.find(L"data/sounds/respawn_start.ogg") != m_mSoundParameters.end() ? m_mSoundParameters[L"data/sounds/respawn_start.ogg"] : 1.0f);
          }

          if (m_aOneShots[(int)enOneShots::RespawnDone] == nullptr) {
            m_aOneShots[(int)enOneShots::RespawnDone] = m_pEngine->addSoundSourceFromFile("data/sounds/respawn.ogg");
            m_aOneShots[(int)enOneShots::RespawnDone]->setDefaultMinDistance( 25.0f);
            m_aOneShots[(int)enOneShots::RespawnDone]->setDefaultMaxDistance(250.0f);
            m_aOneShots[(int)enOneShots::RespawnDone]->setDefaultVolume     (m_mSoundParameters.find(L"data/sounds/respawn.ogg") != m_mSoundParameters.end() ? m_mSoundParameters[L"data/sounds/respawn.ogg"] : 1.0f);
          }

          if (m_aOneShots[(int)enOneShots::Hit] == nullptr) {
            m_aOneShots[(int)enOneShots::Hit] = m_pEngine->addSoundSourceFromFile("data/sounds/hit.ogg");
            m_aOneShots[(int)enOneShots::Hit]->setDefaultMinDistance( 25.0f);
            m_aOneShots[(int)enOneShots::Hit]->setDefaultMaxDistance(250.0f);
            m_aOneShots[(int)enOneShots::Hit]->setDefaultVolume     (m_mSoundParameters.find(L"data/sounds/hit.ogg") != m_mSoundParameters.end() ? m_mSoundParameters[L"data/sounds/hit.ogg"] : 1.0f);
          }
        }

        /**
        * Stop a game, i.e. the sounds are muted
        */
        virtual void stopGame() override {
          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
              if (m_aMarbles[i].m_pSounds[j] != nullptr) m_aMarbles[i].m_pSounds[j]->stop();
            }
          }
        }

        /**
        * Pause or unpause a game, i.e. in-game sounds are muted or unmuted
        */
        virtual void pauseGame(bool a_bPaused) override {
          if (a_bPaused) {
            for (int i = 0; i < 16; i++) {
              for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
                if (m_aMarbles[i].m_pSounds[j] != nullptr) m_aMarbles[i].m_pSounds[j]->setVolume(0.0f);
              }
            }
          }
          else {
            for (int i = 0; i < 16; i++) {
              for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
                if (m_aMarbles[i].m_pSounds[j] != nullptr) m_aMarbles[i].m_pSounds[j]->setVolume(m_aMarbles[i].m_fVolume);
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
        virtual void playMarbleSounds(int a_iMarble, const irr::core::vector3df& a_cPosition, const irr::core::vector3df& a_cVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
          irr::f32 l_fDistSq = a_cPosition.getDistanceFromSQ(m_vPosListener);

          // printf("%.2f\n", a_fHit);

          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            for (int i = 0; i < (int)enMarbleSounds::Count; i++) {
              m_aMarbles[l_iIndex].m_pSounds[i]->setPosition(irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z));
              m_aMarbles[l_iIndex].m_pSounds[i]->setVelocity(irrklang::vec3df(a_cVelocity.X, a_cVelocity.Y, a_cVelocity.Z));
            }

            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Rolling]->setVolume(            a_bHasContact ? m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Rolling] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f);
            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Skid   ]->setVolume(a_bBrake && a_bHasContact ? m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Skid   ] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f);
            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Wind   ]->setVolume(                            m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Wind   ] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume       );
            
            if (a_fHit > 0.0f) {
              m_pEngine->play3D(m_aOneShots[(int)enOneShots::Hit], irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z), false, false, false);
            }
          }
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df& a_cPosition) override {
          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Stunned]->setPlayPosition(0);
            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Stunned]->setIsPaused(false);
          }
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) override {
          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            m_aMarbles[l_iIndex].m_pSounds[(int)enMarbleSounds::Stunned]->setIsPaused(true);
          }
        }

        /**
        * Play the first respawn sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the respawning marble
        * @param a_cPosition position of the respawning marble
        */
        virtual void playMarbleRespawnStart(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            m_pEngine->play3D(m_aOneShots[(int)enOneShots::RespawnStart], irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z), false, false, false);
          }
        }

        /**
        * Play the second respawn sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the respawning marble
        * @param a_cPosition position of the respawning marble
        */
        virtual void playMarbleRespawnDone(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            m_pEngine->play3D(m_aOneShots[(int)enOneShots::RespawnDone], irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z), false, false, false);
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

          irr::core::vector3df l_cPos = a_pCamera->getAbsolutePosition();
          irr::core::vector3df l_cTgt = a_pCamera->getTarget          ();
          irr::core::vector3df l_cUp  = a_pCamera->getUpVector        ();

          m_pEngine->setListenerPosition(irrklang::vec3df(l_cPos.X, l_cPos.Y, l_cPos.Z), irrklang::vec3df(l_cTgt.X, l_cTgt.Y, l_cTgt.Z), irrklang::vec3df(a_vVel.X, a_vVel.Y, a_vVel.Z), irrklang::vec3df(l_cUp.X, l_cUp.Y, l_cUp.Z));
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
