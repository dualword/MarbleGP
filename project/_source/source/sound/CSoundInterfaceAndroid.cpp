// (w) 2021 by Dustbin::Games / Christian Keimel

#include <media/NdkMediaExtractor.h>
#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <android/asset_manager.h>
#include <vorbis/vorbisfile.h>
#include <vorbis/codec.h>
#include <oboe/Oboe.h>
#include <ogg/ogg.h>
#include <CGlobal.h>
#include <string>
#include <map>

#include <stdio.h>
#include <android/log.h>
#include <vector>

#define APP_NAME "MarbleGP_Android_Audio"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, APP_NAME, __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, APP_NAME, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, APP_NAME, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, APP_NAME, __VA_ARGS__))


namespace dustbin {
  namespace sound {
    constexpr int kMaxCompressionRatio { 12 };

    typedef struct SOggMemoryFile {
      char        *m_pBuffer;   /**< The actual data */
      size_t       m_iSize;     /**< Size of the data */
      std::string  m_sFile;     /**< The file name (for debugging) */

      SOggMemoryFile() : m_pBuffer(nullptr), m_iSize(0), m_sFile("") {
      }

      SOggMemoryFile(const std::string &a_sFile) : m_pBuffer(nullptr), m_iSize(0), m_sFile(a_sFile) {
        irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

        if (l_pFs->existFile(a_sFile.c_str())) {
          irr::io::IReadFile *l_pFile = l_pFs->createAndOpenFile(a_sFile.c_str());
          m_iSize = l_pFile->getSize();

          m_pBuffer = new char[m_iSize];
          l_pFile->read(m_pBuffer, m_iSize);
        }
      }

      ~SOggMemoryFile() {
        if (m_pBuffer != nullptr)
          delete m_pBuffer;
      }
    }
    SOggMemoryFile;

    typedef struct SOggMemorySound {
      SOggMemoryFile *m_pFile;    /**< The memory file */
      size_t          m_iPos;     /**< The current position */
      bool            m_bLooped;  /**< Is this sound looped? */
      FILE           *m_pMemFile;

      SOggMemorySound() : m_pFile(nullptr), m_iPos(0), m_bLooped(false), m_pMemFile(nullptr) {
      }

      void setMemoryFile(SOggMemoryFile *a_pFile) {
        m_pFile = a_pFile;
        m_pMemFile = fmemopen(a_pFile->m_pBuffer, a_pFile->m_iSize, "rb");
      }
    }
    SOggMemorySound;

    std::map<std::wstring, SOggMemoryFile *> g_mSoundFiles;

    ogg_sync_state g_cOggSync;

    class CErrorCallback : public oboe::AudioStreamErrorCallback {
      public:
        bool onError(oboe::AudioStream* a_pStream, oboe::Result a_eResult) override {
          LOGE("onError: %i\n", (int)a_eResult);
          a_pStream->stop();
          return true;
        }

        void onErrorBeforeClose(oboe::AudioStream* a_pStream, oboe::Result a_eResult) override {
          LOGE("onErrorBeforeClose: %i\n", (int)a_eResult);
          a_pStream->stop();
        }

        void onErrorAfterClose(oboe::AudioStream* a_pStream, oboe::Result a_eResult) override {
          LOGE("onErrorAfterClose: %i\n", (int)a_eResult);
          a_pStream->stop();
        }
    };

    class CAudioPlayer : public oboe::AudioStreamCallback {
      private:
        irr::f32 m_fVolume;
        bool m_bPlaying;
        bool m_bLooped;
        bool m_bStereo;
        std::shared_ptr<oboe::AudioStream> m_pStream;
        oboe::AudioStreamBuilder m_cBuilder;
        std::string m_sSound;
        SOggMemorySound  m_cSound;
        OggVorbis_File m_cFile;
        int m_iOpen;

        CErrorCallback m_cError;

        void renderSilence(float *a_pStart, int32_t a_iNumSamples) {
          for (int i = 0; i < a_iNumSamples; ++i) {
            a_pStart[i] = 0;
          }
        }

      public:
        /**
        * Construct a new Player from the given DataSource. Players can share the same data source.
        * For example, you could play two identical sounds concurrently by creating 2 Players with the
        * same data source.
        *
        * @param a_pSource the audio source
        * @param a_bLooped flag to indicate whether or not this sound is looped
        */
        CAudioPlayer(const std::string &a_sSound, bool a_bLooped) :
          m_fVolume        (1.0f     ),
          m_bPlaying       (false    ),
          m_bLooped        (a_bLooped),
          m_bStereo        (true     ),
          m_pStream        (nullptr  ),
          m_sSound         (a_sSound ),
          m_iOpen          (-1       )
        {
          if (g_mSoundFiles.find(helpers::s2ws(a_sSound)) != g_mSoundFiles.end()) {
            m_cSound.setMemoryFile(g_mSoundFiles[helpers::s2ws(a_sSound)]);
          }


          memset(&m_cFile, 0, sizeof(OggVorbis_File));
          m_iOpen = ov_open(m_cSound.m_pMemFile, &m_cFile, nullptr, -1);

          switch (m_iOpen) {
            case OV_EREAD: LOGD("A read from media returned an error."); break;
            case OV_ENOTVORBIS: LOGD("Bitstream does not contain any Vorbis data."); break;
            case OV_EVERSION: LOGD("Vorbis version mismatch."); break;
            case OV_EBADHEADER: LOGD("Invalid Vorbis bitstream header."); break;
            case OV_EFAULT: LOGD("Internal logic fault."); break;
          }

          if (m_iOpen == 0) {
            m_bStereo = m_cFile.vi->channels > 1;

            m_cBuilder.setDirection(oboe::Direction::Output)
              ->setFormatConversionAllowed(true)
              ->setSampleRate(44100)
              ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
              ->setUsage(oboe::Usage::Game)
              ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
              ->setSharingMode(oboe::SharingMode::Shared)
              ->setFormat(oboe::AudioFormat::Float)
              ->setChannelCount(m_bStereo ? oboe::ChannelCount::Mono : oboe::ChannelCount::Stereo)
              ->setErrorCallback(&m_cError);

            m_cBuilder.setCallback(this);
            oboe::Result l_eResult = m_cBuilder.openStream(m_pStream);

            if (l_eResult == oboe::Result::OK) {
              m_pStream->start();
            }
          }
        }

        oboe::DataCallbackResult renderAudio(float *a_pTargetData, int32_t a_iNumFrames) {
          if (m_bPlaying && m_iOpen == 0) {
            int l_iRemaining = a_iNumFrames;
            LOGD("Render Audio 1: %i", a_iNumFrames);
            LOGD("Render Audio 2: %s", m_cSound.m_pFile->m_sFile.c_str());
            int l_iFrame = 0;

            while (l_iRemaining > 0) {
              float **l_pPcm = nullptr;
              int l_iSection = 0;
              int l_iRead = ov_read_float(&m_cFile, &l_pPcm, std::min(256, l_iRemaining), &l_iSection);

              if (l_iRead > 0) {
                for (int i = 0; i < l_iRead; i++) {
                  for (int j = 0; j < m_cFile.vi->channels; j++) {
                    a_pTargetData[l_iFrame] = l_pPcm[j][i];
                    l_iFrame++;
                    l_iRemaining--;
                  }

                  if (l_iRemaining == 0)
                    break;
                }
              }
              else break;
              LOGD("Render Audio 3: %i, %i", l_iRead, l_iRemaining);
            }
            LOGD("Render Audio 4 (%i, %i)", a_iNumFrames, l_iRemaining);
          } else {
            renderSilence(a_pTargetData, a_iNumFrames * (m_bStereo ? 2 : 1));
          }

          return oboe::DataCallbackResult::Continue;
        }

        void resetPlayHead() {
          m_cSound.m_iPos = 0;
        }

        void stopSound() {
          m_pStream->stop();
        }

        void startSound() {
          m_pStream->start();
        }

        void setPlaying(bool a_bPlay, bool a_bReset) {
          if (a_bReset)
            resetPlayHead();

          m_bPlaying = a_bPlay; 
        }

        void setLooping(bool a_bIsLooped) { 
          m_bLooped = a_bIsLooped; 
        }

        void setVolume(irr::f32 a_fVolume) {
          m_fVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;
        }

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream* a_pStream, void* a_pAudioData, int32_t a_iNumFrames) {
          renderAudio(static_cast<float *>(a_pAudioData), a_iNumFrames);
          return oboe::DataCallbackResult::Continue;
        }
    };

    class CSoundInterface : public ISoundInterface {
      private:
        /**
        * This structure holds all data necessary to play the marble sounds
        */
        struct SMarbleSound {

          CAudioPlayer *m_aSounds[(int)enMarbleSounds::Count];

          SMarbleSound() {
            /*m_aSounds[(int)enMarbleSounds::Rolling] = new CAudioPlayer("data/sounds/rolling.ogg", true);
            m_aSounds[(int)enMarbleSounds::Wind   ] = new CAudioPlayer("data/sounds/wind.ogg"   , true);
            m_aSounds[(int)enMarbleSounds::Skid   ] = new CAudioPlayer("data/sounds/skid.ogg"   , true);
            m_aSounds[(int)enMarbleSounds::Stunned] = new CAudioPlayer("data/sounds/stunned.ogg", true);

            m_aSounds[(int)enMarbleSounds::Rolling]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Wind   ]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Skid   ]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(0.0f);

            m_aSounds[(int)enMarbleSounds::Rolling]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Wind   ]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Skid   ]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Stunned]->setPlaying(true, true);

            LOGD("Marble sounds loaded.");*/
          }

          ~SMarbleSound() {
            for (int i = 0; i < (int)enMarbleSounds::Count; i++) {
              if (m_aSounds[i] != nullptr) {
                delete m_aSounds[i];
                m_aSounds[i] = nullptr;
              }
            }
          }
        };

        irr::f32 m_fMasterVolume;
        irr::f32 m_fSoundtrackVolume;
        irr::f32 m_fGameVolume;
        irr::f32 m_fMenuVolume;

        bool m_bMenu;

        int m_iPlayerMarble;

        CAudioPlayer *m_aSounds2d[(int)en2dSounds::Count];
        CAudioPlayer *m_aShots   [(int)enOneShots::Count];

        std::map<enSoundTrack  , CAudioPlayer *> m_mSoundTracks;
        std::map<std::wstring  , irr::f32      > m_mParameters;

        std::vector<CAudioPlayer *> m_vMenuSounds;
        std::vector<CAudioPlayer *> m_vGameSounds;

        SMarbleSound *m_pMarble;

        enSoundTrack m_eSoundTrack;

        irr::f32 m_fMarbleParams[4];

        irr::core::vector3df m_cPosition;     /**< Position of the listener */
        irr::core::vector3df m_cDirection;    /**< Looking direction of the listener */
        irr::core::vector3df m_cVelocity;     /**< Velocity of the listener */

        irr::core::matrix4 m_cViewMatrix;   /**< The view matrix of the listener */

      public:
        CSoundInterface(irr::IrrlichtDevice *a_pDevice) : 
          m_fMasterVolume    (1.0f),
          m_fSoundtrackVolume(1.0f),
          m_fGameVolume      (1.0f),
          m_fMenuVolume      (1.0f),
          m_bMenu            (true),
          m_iPlayerMarble    (0),
          m_pMarble          (nullptr),
          m_eSoundTrack      (enSoundTrack::enStNone) 
        {
          LOGI("CSoundInterface construtor.");
          oboe::DefaultStreamValues::SampleRate = 44100;

          std::string l_a2dSounds[] = {
            "data/sounds/button_hover.ogg",
            "data/sounds/button_press.ogg",
            "data/sounds/countdown.ogg",
            "data/sounds/countdown_go.ogg",
            ""
          };

          /*for (int i = 0; l_a2dSounds[i] != ""; i++) {
            LOGD("*******************");
            LOGD("Loading 2d sound %s", l_a2dSounds[i].c_str());
            LOGD("*******************");
            preloadSound(helpers::s2ws(l_a2dSounds[i]), true);
            m_aSounds2d[i] = new CAudioPlayer(l_a2dSounds[i], false);
          }

          if (CGlobal::getInstance()->getFileSystem()->existFile("data/marblesounds.xml")) {
            irr::io::IXMLReader *l_pReader = CGlobal::getInstance()->getFileSystem()->createXMLReader("data/marblesounds.xml");

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
                      LOGD("+++++++++++++++++++++++");
                      LOGD("Loading marble sound %s", helpers::ws2s(l_sName).c_str());
                      LOGD("+++++++++++++++++++++++");
                    }
                    else if (l_sAttr == L"vol") {
                      l_fVolume = std::stof(l_sValue);
                    }
                  }

                  if (l_sName != L"") {
                    printf("SFX Param: \"%ls\": %.2f\n", l_sName.c_str(), l_fVolume);
                    m_mParameters[l_sName] = l_fVolume;
                  }
                }
              }
              l_pReader->drop();
            }
          }

          LOGD("********************");
          LOGD("* 2d sounds loaded *");
          LOGD("********************");
          m_fMarbleParams[0] = m_mParameters.find(L"data/sounds/rolling.ogg") != m_mParameters.end() ? m_mParameters[L"data/sounds/rolling.ogg"] : 1.0f;
          m_fMarbleParams[1] = m_mParameters.find(L"data/sounds/wind.ogg"   ) != m_mParameters.end() ? m_mParameters[L"data/sounds/rolling.ogg"] : 1.0f;
          m_fMarbleParams[2] = m_mParameters.find(L"data/sounds/skid.ogg"   ) != m_mParameters.end() ? m_mParameters[L"data/sounds/rolling.ogg"] : 1.0f;
          m_fMarbleParams[3] = m_mParameters.find(L"data/sounds/stunned.ogg") != m_mParameters.end() ? m_mParameters[L"data/sounds/rolling.ogg"] : 1.0f;

          std::string l_aOneShots[] = {
            "data/sounds/respawn_start.ogg",
            "data/sounds/respawn.ogg",
            "data/sounds/hit.ogg",
            "data/sounds/checkpoint.ogg",
            "data/sounds/lap.ogg",
            "data/sounds/gameover.ogg",
            ""
          };

          for (int i = 0; l_aOneShots[i] != ""; i++) {
            LOGD("Loading one shot sound %s", l_aOneShots[i].c_str());
            preloadSound(helpers::s2ws(l_aOneShots[i]), false);
            m_aShots[i] = new CAudioPlayer(l_aOneShots[i], false);
          }*/

          preloadSound(L"data/sounds/theme_menu.ogg"  , true);
          preloadSound(L"data/sounds/theme_race.ogg"  , false);
          preloadSound(L"data/sounds/theme_result.ogg", false);

          m_pMarble = new SMarbleSound();
        }

        ~CSoundInterface() override {
          for (int i = 0; i < (int)enOneShots::Count; i++) {
            if (m_aShots[i] != nullptr) {
              delete m_aShots[i];
              m_aShots[i] = nullptr;
            }
          }

          while (g_mSoundFiles.size() > 0) {
            SOggMemoryFile *l_pFile = g_mSoundFiles.begin()->second;
            g_mSoundFiles.erase(g_mSoundFiles.begin());
            delete l_pFile;
          }

          if (m_pMarble != nullptr)
            delete m_pMarble;

          ogg_sync_clear(&g_cOggSync);
        }

        void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) override {
        }

        void setMasterVolume(irr::f32 a_fVolume) override {
          m_fMasterVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }

          for (std::vector<CAudioPlayer*>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fMenuVolume * m_fMasterVolume);
          }

          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        void setSfxVolumeGame(irr::f32 a_fVolume) override {
          m_fGameVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        void setSfxVolumeMenu(irr::f32 a_fVolume) override {
          m_fMenuVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        void setSoundtrackVolume(irr::f32 a_fVolume) override {
          m_fSoundtrackVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;
          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        void muteAudio() override {
          // for (std::map<en2dSounds, CAudioPlayer *>::iterator it = m_mSounds.begin(); it != m_mSounds.end(); it++)
          //   it->second->setVolume(0.0f);
        }

        void unmuteAudio() override {
          for (std::vector<CAudioPlayer*>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }

          for (std::vector<CAudioPlayer*>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fMenuVolume * m_fMasterVolume);
          }

          for (std::map<enSoundTrack, CAudioPlayer *>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        irr::f32 getMasterVolume() override {
          return m_fMasterVolume;
        }

        irr::f32 getSfxVolumeGame() override {
          return m_fGameVolume;
        }

        irr::f32 getSfxVolumeMenu() override {
          return m_fMenuVolume;
        }

        irr::f32 getSoundtrackVolume() override {
          return m_fSoundtrackVolume;
        }

        void startSoundtrack(enSoundTrack a_eSoundTrack) override {
          if (a_eSoundTrack != m_eSoundTrack) {
            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(false, true);

            m_eSoundTrack = a_eSoundTrack;

            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(true, true);
          }
        }

        void setSoundtrackFade(irr::f32 a_fValue) override {
          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fMasterVolume * m_fSoundtrackVolume * a_fValue);
          }
        }

        /**
        * Start a game, i.e. the sounds are initialized
        */
        void startGame() override {
        }

        /**
        * Stop a game, i.e. the sounds are muted
        */
        void stopGame() override {
        }

        /**
        * Pause or unpause a game, i.e. in-game sounds are muted or unmuted
        */
        void pauseGame(bool a_bPaused) override {

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
        void playMarbleSounds(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_pMarble->m_aSounds[(int)enMarbleSounds::Rolling]->setVolume(            a_bHasContact ? m_fMarbleParams[0] * a_fVolume * m_fMasterVolume * m_fGameVolume : 0.0f);
            m_pMarble->m_aSounds[(int)enMarbleSounds::Wind   ]->setVolume(                            m_fMarbleParams[1] * a_fVolume * m_fMasterVolume * m_fGameVolume       );
            m_pMarble->m_aSounds[(int)enMarbleSounds::Skid   ]->setVolume(a_bBrake && a_bHasContact ? m_fMarbleParams[2] * a_fVolume * m_fMasterVolume * m_fGameVolume : 0.0f);
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
        void playViewportMarbleSound(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        void playMarbleStunned(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          if (m_iPlayerMarble == a_iMarble) {
            m_pMarble->m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(m_fMarbleParams[3] * m_fMasterVolume * m_fGameVolume);
          }
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        void stopMarbleStunned(int a_iMarble) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_pMarble->m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(0.0f);
          }
        }

        /**
        * Play a specific sound for a marble, i.e. checkpoint, lap passed
        * @param a_iMarble ID of the marble
        * @param a_eSound the sound to play
        */
        void playMarbleOneShotSound(int a_iMarble, enOneShots a_eSound) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_aShots[(int)a_eSound]->setVolume(m_fMasterVolume * m_fGameVolume);
            m_aShots[(int)a_eSound]->setPlaying(true, true);
          }
        }

        /**
        * Define a marble controlled by a local player
        */
        void setViewportMarble(int a_iMarble) override {
          m_iPlayerMarble = a_iMarble;
        }

        void play2d(en2dSounds a_eSound, irr::f32 a_fVolume, irr::f32 a_fPan) override {
          if (m_aSounds2d[(int)a_eSound] != nullptr) {
            m_aSounds2d[(int)a_eSound]->setPlaying(true, a_fPan == 0.0f);
            m_aSounds2d[(int)a_eSound]->setVolume(a_fVolume * m_fMasterVolume * (a_eSound == en2dSounds::ButtonHover || a_eSound == en2dSounds::ButtonPress ? m_fMenuVolume : m_fGameVolume));
          }
        }

        void clear3dSounds() override {
          // 3d sounds not supported by Android
        }

        void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) override {
          m_cPosition  = a_pCamera->getAbsolutePosition();
          m_cDirection = a_pCamera->getTarget();
          m_cVelocity  = a_vVel;

          m_cViewMatrix = a_pCamera->getViewMatrix();
        }

        void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) override {
          if (g_mSoundFiles.find(a_sName) == g_mSoundFiles.end())
            g_mSoundFiles[a_sName] = new SOggMemoryFile(helpers::ws2s(a_sName));
        }

        void assignSoundtracks(const std::map<enSoundTrack, std::tuple<std::string, bool>> &a_mSoundTracks) override {
          for (std::map<enSoundTrack, std::tuple<std::string, bool>>::const_iterator it = a_mSoundTracks.begin(); it != a_mSoundTracks.end(); it++) {
            if (m_mSoundTracks.find(it->first) == m_mSoundTracks.end()) {
              m_mSoundTracks[it->first] = new CAudioPlayer(std::get<0>(it->second), std::get<1>(it->second));
            }
          }
        }

        void setMenuFlag(bool a_bMenu) override {
          m_bMenu = a_bMenu;
        }
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice) {
      int l_iInit = ogg_sync_init(&g_cOggSync);
      printf("%i\n", l_iInit);
      return new CSoundInterface(a_pDevice);
    }
  }
}
