#ifdef _WINDOWS

// (w) 2021 by Dustbin::Games / Christian Keimel
#define IRRKLANG_STATIC

#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <sound/CSoundData.h>
#include <irrKlang.h>
#include <CGlobal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace sound {
    /**
    * @class CDatFileReader
    * @author Christian Keimel
    * For Irrklang we need to wrap the Irrlicht file reader
    */
    class CDatFileReader : public irrklang::IFileReader {
      private:
        irr::io::IFileSystem *m_pFs;
        irr::io::IReadFile   *m_pFile;
        std::string           m_sName;

      public:
        CDatFileReader(const std::string& a_sName, irr::io::IFileSystem* a_pFs) : m_pFs(a_pFs), m_pFile(nullptr), m_sName(a_sName) {
          m_pFile = m_pFs->createAndOpenFile(m_sName.c_str());
        }

        virtual ~CDatFileReader() {
          m_pFile->drop();
        }

        virtual irrklang::ik_s32 read(void *a_pBuffer, irrklang::ik_u32 a_iSizeToRead) override {
          if (m_pFile != nullptr)
            return (irrklang::ik_s32)m_pFile->read(a_pBuffer, a_iSizeToRead);
          else
            return 0;
        }

        virtual bool seek(irrklang::ik_s32 a_iFinalPos, bool a_bRelativeMovement = false) override {
          if (m_pFile != nullptr)
            return m_pFile->seek(a_iFinalPos, a_bRelativeMovement);
          else
            return false;
        }

        virtual irrklang::ik_s32 getSize() override {
          if (m_pFile != nullptr)
            return (irrklang::ik_s32)m_pFile->getSize();
          else
            return 0;
        }

        virtual irrklang::ik_s32 getPos() override {
          if (m_pFile != nullptr)
            return (irrklang::ik_s32)m_pFile->getPos();
          else
            return 0;
        }

        virtual const irrklang::ik_c8 *getFileName() override {
          return (irrklang::ik_c8 *)m_sName.c_str();
        }
    };

    /**
    * @class CDatFileReaderFactor
    * @author Christian Keimel
    * The file factory for Irrklang so that we can read files from the archive(s)
    */
    class CDatFileReaderFactory : public irrklang::IFileFactory {
      private:
        irr::io::IFileSystem *m_pFs;

      public:
        CDatFileReaderFactory(irr::io::IFileSystem* a_pFs) : m_pFs(a_pFs) {
        }

        virtual ~CDatFileReaderFactory() {
        }

        virtual irrklang::IFileReader* createFileReader(const irrklang::ik_c8 *a_sFilename) override {
          return new CDatFileReader(a_sFilename, m_pFs);
        }
    };

    class CSoundInterface : public IAudioBuffer::IDeletionListener, public ISoundInterface {
      private:
        struct SMarbleSound {
          int       m_iMarble;                               /**< ID of the marble these sounds blong to */
          bool      m_bActive;                               /**< Is the sound active? */
          bool      m_bViewport;                             /**< Is this a viewport marble? */
          irr::f32  m_fVolume;                               /**< The current volume of the sound */
          irr::f32  m_fPan;                                  /**< The stereo position of the sound for viewport marbles */
          irr::f32  m_aParam[(int)enMarbleSounds::Count];    /**< The volume factor read from the parameters*/

          irrklang::vec3df m_cPosition;   /**< Position of the marble */

          irrklang::ISound *m_aSounds  [(int)enMarbleSounds::Count];
          irrklang::ISound *m_aOneShots[(int)enOneShots    ::Count];

          SMarbleSound() : m_iMarble(0), m_bActive(false), m_bViewport(false), m_fVolume(0.0f), m_fPan(0.0f) {
          }
        };

        irr::f32              m_fSoundtrackVolume;
        irr::f32              m_fSoundFXVolumeMenu;
        irr::f32              m_fSoundFXVolumeGame;
        irr::f32              m_fViewportVolume;
        irr::f32              m_fMasterVolume;
        enSoundTrack          m_eSoundTrack;
        irr::core::vector3df  m_vPosListener;
        irr::core::vector3df  m_vVelListener;
        irrklang::ISound     *m_pSoundTrack;          /**< The soundtrack that is currently playing */
        bool                  m_bMenu;                /**< Are we currently in the menu or game? */

        irrklang::ISoundEngine *m_pEngine;    /**< The irrKlang sound engine */

        SMarbleSound m_aMarbles[16];

        irrklang::ISoundSource *m_aOneShots[(int)enOneShots::Count];
        irrklang::ISoundSource *m_a2dSounds[(int)en2dSounds::Count];

        /**
        * Volume of the sounds. Key = path to file, Value = volume
        */
        std::map<std::wstring, irr::f32> m_mSoundParameters;

        /**
        * The soundtracks. Bool parameter in tuple is whether or not it is played looped
        */
        std::map<enSoundTrack, std::tuple<irrklang::ISoundSource *, bool>> m_mSoundTracks;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs) : m_bMenu(true) {
          m_pEngine = irrklang::createIrrKlangDevice();

          CDatFileReaderFactory *l_pFactory = new CDatFileReaderFactory(a_pFs);
          m_pEngine->addFileFactory(l_pFactory);
          l_pFactory->drop();

          m_vVelListener = irr::core::vector3df(0.0f, 0.0f, 0.0f);
          m_eSoundTrack  = enSoundTrack::enStNone;
          m_pSoundTrack  = nullptr;

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
                    printf("SFX Param: \"%ls\": %.2f\n", l_sName.c_str(), l_fVolume);
                    m_mSoundParameters[l_sName] = l_fVolume;
                  }
                }
              }
              l_pReader->drop();
            }
          }

          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++) m_aMarbles[i].m_aSounds  [j] = nullptr;
            for (int j = 0; j < (int)enOneShots    ::Count; j++) m_aMarbles[i].m_aOneShots[j] = nullptr;
          }
          
          for (int i = 0; i < (int)enOneShots::Count; i++)
            m_aOneShots[i] = nullptr;

          for (int i = 0; i < (int)en2dSounds::Count; i++)
            m_a2dSounds[i] = nullptr;

          if (m_pEngine != nullptr) {
            std::wstring l_a2dSounds[] = {
              L"data/sounds/button_hover.ogg",
              L"data/sounds/button_press.ogg",
              L"data/sounds/countdown.ogg",
              L"data/sounds/countdown_go.ogg",
              L"data/sounds/lap.ogg",
              L""
            };

            for (int i = 0; l_a2dSounds[i] != L""; i++) {
              m_a2dSounds[i] = m_pEngine->addSoundSourceFromFile(helpers::ws2s(l_a2dSounds[i]).c_str());
            }
          }

          if (m_pEngine != nullptr) {
            std::wstring l_aMarbleSounds[] = {
              L"data/sounds/rolling.ogg",
              L"data/sounds/wind.ogg",
              L"data/sounds/skid.ogg",
              L"data/sounds/stunned.ogg",
              L""
            };

            for (int i = 0; i < 16; i++) {
              for (int j = 0; l_aMarbleSounds[j] != L""; j++) {
                m_aMarbles[i].m_aSounds[j] = m_pEngine->play3D(helpers::ws2s(l_aMarbleSounds[j]).c_str(), irrklang::vec3df(0.0f, 0.0f, 0.0f), true, true);
                m_aMarbles[i].m_aSounds[j]->setMinDistance( 25.0f);
                m_aMarbles[i].m_aSounds[j]->setMaxDistance(250.0f);
                m_aMarbles[i].m_aSounds[j]->setVolume     (  0.0f);
                m_aMarbles[i].m_aSounds[j]->setIsPaused   (false );

                if (m_mSoundParameters.find(l_aMarbleSounds[j]) != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[j] = m_mSoundParameters[l_aMarbleSounds[j]]; else m_aMarbles[i].m_aParam[j] = 1.0f;
              }
            }

            std::wstring l_aOneShots[] = {
              L"data/sounds/respawn_start.ogg",
              L"data/sounds/respawn.ogg",
              L"data/sounds/hit.ogg",
              L"data/sounds/checkpoint.ogg",
              L"data/sounds/lap.ogg",
              L"data/sounds/gameover.ogg",
              L""
            };

            for (int i = 0; l_aOneShots[i] != L""; i++) {
              std::string l_sSound = helpers::ws2s(l_aOneShots[i]);
              m_aOneShots[i] = m_pEngine->addSoundSourceFromFile(l_sSound.c_str());

              if (m_aOneShots[i] != nullptr) {
                m_aOneShots[i]->setDefaultMinDistance( 25.0f);
                m_aOneShots[i]->setDefaultMaxDistance(250.0f);
                m_aOneShots[i]->setDefaultVolume     (m_mSoundParameters.find(l_aOneShots[i]) != m_mSoundParameters.end() ? m_mSoundParameters[l_aOneShots[i]] : 1.0f);
              }
              else printf("Error loading sound \"%s\"\n", l_sSound.c_str());
            }
          }

          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
              if (m_aMarbles[i].m_aSounds[j] != nullptr) {
                m_aMarbles[i].m_aSounds[j]->setVolume(0.0f);
                m_aMarbles[i].m_aSounds[j]->setIsPaused(false);
              }
            }
          }
        }

        virtual ~CSoundInterface() {
          m_pEngine->stopAllSounds();

          for (int i = 0; i < 16; i++) {
            for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
              if (m_aMarbles[i].m_aSounds[j] != nullptr) {
                m_aMarbles[i].m_aSounds[j]->drop();
                m_aMarbles[i].m_aSounds[j] = nullptr;
              }
            }
          }

          for (int i = 0; i < (int)enOneShots::Count; i++)
            if (m_aOneShots[i] != nullptr) {
              m_aOneShots[i]->drop();
              m_aOneShots[i] = nullptr;
            }

          irrklang::ISoundEngine *p = m_pEngine;
          m_pEngine = nullptr;
          p->drop();
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
          if (m_pSoundTrack != nullptr)
            m_pSoundTrack->setVolume(m_fMasterVolume * m_fSoundtrackVolume);
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
          m_pEngine->setSoundVolume(0.0f);
        }

        virtual void unmuteAudio() override {
          m_pEngine->setSoundVolume(1.0f);
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
            if (m_pSoundTrack != nullptr) {
              m_pSoundTrack->stop();
              m_pSoundTrack->drop();
            }

            m_eSoundTrack = a_eSoundTrack;

            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end()) {
              m_pSoundTrack = m_pEngine->play2D(std::get<0>(m_mSoundTracks[m_eSoundTrack]), std::get<1>(m_mSoundTracks[m_eSoundTrack]), true);
              m_pSoundTrack->setVolume(m_fMasterVolume * m_fSoundtrackVolume);
              m_pSoundTrack->setIsPaused(false);
            }
            else
              m_pSoundTrack = nullptr;


            if (m_pSoundTrack != nullptr) {
              m_pSoundTrack->setVolume(m_fMasterVolume * m_fSoundtrackVolume);
              m_pSoundTrack->setIsPaused(false);
            }
          }
        }

        virtual void setSoundtrackFade(irr::f32 a_fValue) override {
          if (m_pSoundTrack != nullptr) {
            m_pSoundTrack->setVolume((a_fValue > 0.0f ? a_fValue : 0.0f) * m_fMasterVolume * m_fSoundtrackVolume);
          }
        }


        /**
        * Start a game, i.e. the sounds are initialized
        */
        virtual void startGame() override {
          std::wstring l_aMarbleSounds[] = {
            L"data/sounds/rolling.ogg",
            L"data/sounds/wind.ogg",
            L"data/sounds/skid.ogg",
            L"data/sounds/stunned.ogg",
            L""
          };

          for (int i = 0; i < 16; i++) {
            for (int j = 0; l_aMarbleSounds[j] != L""; j++) {
              if (m_aMarbles[i].m_bViewport) {
                m_aMarbles[i].m_aSounds[j] = m_pEngine->play2D(helpers::ws2s(l_aMarbleSounds[j]).c_str(), true, true);
                m_aMarbles[i].m_aSounds[j]->setVolume(0.0f);
                m_aMarbles[i].m_aSounds[j]->setIsPaused(false);
              }
              else {
                m_aMarbles[i].m_aSounds[j] = m_pEngine->play3D(helpers::ws2s(l_aMarbleSounds[j]).c_str(), irrklang::vec3df(0.0f, 0.0f, 0.0f), true, true);
                m_aMarbles[i].m_aSounds[j]->setMinDistance( 25.0f);
                m_aMarbles[i].m_aSounds[j]->setMaxDistance(250.0f);
                m_aMarbles[i].m_aSounds[j]->setVolume     (  0.0f);
                m_aMarbles[i].m_aSounds[j]->setIsPaused   (false );
              }

              if (m_mSoundParameters.find(l_aMarbleSounds[j]) != m_mSoundParameters.end()) m_aMarbles[i].m_aParam[j] = m_mSoundParameters[l_aMarbleSounds[j]]; else m_aMarbles[i].m_aParam[j] = 1.0f;
            }
          }
        }

        /**
        * Stop a game, i.e. the sounds are muted
        */
        virtual void stopGame() override {
          std::wstring l_aMarbleSounds[] = {
            L"data/sounds/rolling.ogg",
            L"data/sounds/wind.ogg",
            L"data/sounds/skid.ogg",
            L"data/sounds/stunned.ogg",
            L""
          };

          for (int i = 0; i < 16; i++) {
            for (int j = 0; l_aMarbleSounds[j] != L""; j++) {
              m_aMarbles[i].m_aSounds[j]->stop();
              m_aMarbles[i].m_aSounds[j]->drop();
              m_aMarbles[i].m_aSounds[j] = nullptr;
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
                if (m_aMarbles[i].m_aSounds[j] != nullptr) m_aMarbles[i].m_aSounds[j]->setVolume(0.0f);
              }
            }
          }
          else {
            for (int i = 0; i < 16; i++) {
              for (int j = 0; j < (int)enMarbleSounds::Count; j++) {
                if (m_aMarbles[i].m_aSounds[j] != nullptr) m_aMarbles[i].m_aSounds[j]->setVolume(m_fMasterVolume * m_aMarbles[i].m_fVolume);
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
          if (m_pEngine != nullptr) {
            int l_iIndex = a_iMarble - 10000;

            if (l_iIndex >= 0 && l_iIndex < 16) {
              m_aMarbles[l_iIndex].m_cPosition = irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z);

              irrklang::vec3df l_cVel = irrklang::vec3df(a_cVelocity.X, a_cVelocity.Y, a_cVelocity.Z) - irrklang::vec3df(m_vVelListener.X, m_vVelListener.Y, m_vVelListener.Z);
              irrklang::vec3df l_cPos = irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z);

              if (!m_aMarbles[l_iIndex].m_bViewport) {
                for (int i = 0; i < (int)enMarbleSounds::Count; i++) {
                  if (m_aMarbles[l_iIndex].m_aSounds[i] != nullptr) {
                    m_aMarbles[l_iIndex].m_aSounds[i]->setPosition(l_cPos);
                    m_aMarbles[l_iIndex].m_aSounds[i]->setVelocity(l_cVel);
                  }
                }
              }

              if (m_aMarbles[l_iIndex].m_bViewport)
                a_fVolume *= m_fViewportVolume;

              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Rolling]->setVolume(            a_bHasContact ? m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Rolling] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f);
              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Skid   ]->setVolume(a_bBrake && a_bHasContact ? m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Skid   ] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume : 0.0f);
              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Wind   ]->setVolume(                            m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Wind   ] * m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume       );
            
              if (a_fHit > 0.0f && m_aOneShots[(int)enOneShots::Hit] != nullptr) {
                if (m_aMarbles[l_iIndex].m_bViewport) {
                  irrklang::ISound *p = m_pEngine->play2D(m_aOneShots[(int)enOneShots::Hit], false, true, false);
                  p->setVolume(m_fViewportVolume * m_fMasterVolume * m_mSoundParameters[L"data/sounds/hit.ogg"]);
                  p->setIsPaused(false);
                  p->drop();
                }
                else {
                  irrklang::ISound *p = m_pEngine->play3D(m_aOneShots[(int)enOneShots::Hit], irrklang::vec3df(a_cPosition.X, a_cPosition.Y, a_cPosition.Z), false, true, false);
                  if (p != nullptr) {
                    p->setVolume(m_fViewportVolume * m_fMasterVolume * m_mSoundParameters[L"data/sounds/hit.ogg"]);
                    p->setIsPaused(false);
                    p->drop();
                  }
                }
              }

              for (int i = 0; i < (int)enOneShots::Count; i++) {
                if (m_aMarbles[l_iIndex].m_aOneShots[i] != nullptr) {
                  if (m_aMarbles[l_iIndex].m_aOneShots[i]->isFinished()) {
                    m_aMarbles[l_iIndex].m_aOneShots[i]->drop();
                    m_aMarbles[l_iIndex].m_aOneShots[i] = nullptr;
                  }
                }
              }
            }
          }
        }

        /**
        * Play a specific sound for a marble, i.e. checkpoint, lap passed and game over
        * @param a_iMarble ID of the marble
        * @param a_eSound the sound to play
        */
        virtual void playMarbleOneShotSound(int a_iMarble, enOneShots a_eSound) override {
          if (m_pEngine != nullptr) {
            int l_iIndex = a_iMarble - 10000;

            if (l_iIndex >= 0 && l_iIndex < 16) {
              if (m_aMarbles[l_iIndex].m_bViewport)
                m_aMarbles[l_iIndex].m_aOneShots[(int)a_eSound] = m_pEngine->play2D(m_aOneShots[(int)a_eSound], false, true);
              else
                m_aMarbles[l_iIndex].m_aOneShots[(int)a_eSound] = m_pEngine->play3D(m_aOneShots[(int)a_eSound], m_aMarbles[l_iIndex].m_cPosition, false, true);

              if (m_aMarbles[l_iIndex].m_aOneShots[(int)a_eSound] != nullptr) {
                m_aMarbles[l_iIndex].m_aOneShots[(int)a_eSound]->setVolume(m_aMarbles[l_iIndex].m_bViewport ? m_fViewportVolume * m_fSoundFXVolumeGame * m_fMasterVolume : m_fMasterVolume * m_fSoundFXVolumeGame);
                m_aMarbles[l_iIndex].m_aOneShots[(int)a_eSound]->setIsPaused(false);
              }
            }
          }
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df& a_cPosition) override {
          if (m_pEngine != nullptr) {
            int l_iIndex = a_iMarble - 10000;

            if (l_iIndex >= 0 && l_iIndex < 16) {
              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Stunned]->setIsPaused(false);
              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(m_fMasterVolume * m_fSoundFXVolumeGame * m_aMarbles[l_iIndex].m_aParam[(int)enMarbleSounds::Stunned]);
            }
          }
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) override {
          if (m_pEngine != nullptr) {
            int l_iIndex = a_iMarble - 10000;

            if (l_iIndex >= 0 && l_iIndex < 16) {
              m_aMarbles[l_iIndex].m_aSounds[(int)enMarbleSounds::Stunned]->setIsPaused(true);
            }
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

        virtual void play2d(en2dSounds a_eSound, irr::f32 a_fVolume, irr::f32 a_fPan) override {
          if (m_a2dSounds[(int)a_eSound] != nullptr && m_pEngine != nullptr) {
            irrklang::ISound *p = m_pEngine->play2D(m_a2dSounds[(int)a_eSound], false, true);
            p->setVolume(a_fVolume);
            p->setPan(a_fPan);
            p->setIsPaused(false);
          }
        }

        /**
        * Define a marble controlled by a local player
        */
        virtual void setViewportMarble(int a_iMarble) override {
          int l_iIndex = a_iMarble - 10000;

          if (l_iIndex >= 0 && l_iIndex < 16) {
            m_aMarbles[l_iIndex].m_bViewport = true;
          }
        }

        virtual void clear3dSounds() override {
        }

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) override {
          m_vVelListener = a_vVel;
          m_vPosListener = a_pCamera->getAbsolutePosition();

          irr::core::vector3df l_cPos = a_pCamera->getAbsolutePosition();
          irr::core::vector3df l_cTgt = a_pCamera->getTarget          ();
          irr::core::vector3df l_cUp  = a_pCamera->getUpVector        ();

          irrklang::vec3df l_cSoundVel = irrklang::vec3df(0.0f, 0.0f, 0.0f);

          m_pEngine->setListenerPosition(irrklang::vec3df(l_cPos.X, l_cPos.Y, l_cPos.Z), irrklang::vec3df(l_cTgt.X, l_cTgt.Y, l_cTgt.Z), l_cSoundVel, irrklang::vec3df(l_cUp.X, l_cUp.Y, l_cUp.Z));
        }

        virtual void bufferDeleted(IAudioBuffer* a_pBuffer) override {
        }

        virtual void assignSoundtracks(const std::map<enSoundTrack, std::tuple<std::string, bool>> &a_mSoundTracks) override {
          if (m_pEngine != nullptr) {
            for (std::map<enSoundTrack, std::tuple<std::string, bool>>::const_iterator it = a_mSoundTracks.begin(); it != a_mSoundTracks.end(); it++) {
              m_mSoundTracks[it->first] = std::tuple(m_pEngine->addSoundSourceFromFile(std::get<0>(it->second).c_str()), std::get<1>(it->second));
              std::get<0>(m_mSoundTracks[it->first])->setDefaultVolume(m_fMasterVolume * m_fSoundtrackVolume);
            }
          }
        }
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice) {
      return new CSoundInterface(a_pDevice->getFileSystem());
    }
  }
}

#endif