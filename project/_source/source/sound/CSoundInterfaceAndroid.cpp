// (w) 2021 by Dustbin::Games / Christian Keimel

#include <media/NdkMediaExtractor.h>
#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <android/asset_manager.h>
#include <oboe/Oboe.h>
#include <CGlobal.h>
#include <string>
#include <map>

// #include <aaudio/AAudio.h>

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


    class CAssetDataSource  {
      public:
        struct SAudioProperties {
          int32_t m_iChannelCount;
          int32_t m_iSampleRate;
        };

      private:
        static int32_t decode(AAsset *a_pAsset, uint8_t *a_pTargetData, SAudioProperties a_cTargetProperties) {

          // LOGI("DustbinGames: Using NDK decoder");

          int32_t l_iBytesWritten = 0;
          // open asset as file descriptor
          off_t l_iStart, l_iLength;
          int l_iFd = AAsset_openFileDescriptor(a_pAsset, &l_iStart, &l_iLength);

          if (l_iFd >= 0) {
            // Extract the audio frames
            AMediaExtractor *l_pExtractor = AMediaExtractor_new();
            media_status_t l_eAmResult = AMediaExtractor_setDataSourceFd(l_pExtractor, l_iFd,
              static_cast<off64_t>(l_iStart),
              static_cast<off64_t>(l_iLength));
            if (l_eAmResult != AMEDIA_OK){
              LOGE("Error setting extractor data source, err %d", l_eAmResult);
              return 0;
            }

            // Specify our desired output format by creating it from our source
            AMediaFormat *format = AMediaExtractor_getTrackFormat(l_pExtractor, 0);

            int32_t l_iSampleRate;
            if (AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_SAMPLE_RATE, &l_iSampleRate)) {
              // LOGD("Source sample rate %d", l_iSampleRate);
              if (l_iSampleRate != a_cTargetProperties.m_iSampleRate) {
                LOGE("Input (%d) and output (%d) sample rates do not match. "
                  "NDK decoder does not support resampling.",
                  l_iSampleRate,
                  a_cTargetProperties.m_iSampleRate);
                return 0;
              }
            } else {
              LOGE("Failed to get sample rate");
              return 0;
            };

            int32_t l_iChannelCount;
            if (AMediaFormat_getInt32(format, AMEDIAFORMAT_KEY_CHANNEL_COUNT, &l_iChannelCount)) {
              // LOGD("Got channel count %d", l_iChannelCount);
              if (l_iChannelCount != a_cTargetProperties.m_iChannelCount){
                LOGE("NDK decoder does not support different "
                  "input (%d) and output (%d) channel counts",
                  l_iChannelCount,
                  a_cTargetProperties.m_iChannelCount);
              }
            } else {
              LOGE("Failed to get channel count");
              return 0;
            }

            // const char *l_sFormatStr = AMediaFormat_toString(format);
            // LOGD("Output format %s", l_sFormatStr);

            const char *l_sMimeType;
            if (AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &l_sMimeType)) {
              // LOGD("Got mime type %s", l_sMimeType);
            } else {
              LOGE("Failed to get mime type");
              return 0;
            }

            // Obtain the correct decoder
            AMediaCodec *l_pCodec = nullptr;
            AMediaExtractor_selectTrack(l_pExtractor, 0);
            l_pCodec = AMediaCodec_createDecoderByType(l_sMimeType);
            AMediaCodec_configure(l_pCodec, format, nullptr, nullptr, 0);
            AMediaCodec_start(l_pCodec);

            // DECODE

            bool l_bExtracting = true;
            bool l_bDecoding = true;

            while(l_bExtracting || l_bDecoding) {
              if (l_bExtracting) {

                // Obtain the index of the next available input buffer
                // LOGD("7");
                ssize_t l_iInputIndex = AMediaCodec_dequeueInputBuffer(l_pCodec, 2000);
                // LOGD("8");
                // LOGI("Got input buffer %d", (int)l_iInputIndex);

                // The input index acts as a status if its negative
                if (l_iInputIndex < 0) {
                  if (l_iInputIndex == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
                    // LOGI("Codec.dequeueInputBuffer try again later");
                  } 
                  else {
                    LOGE("Codec.dequeueInputBuffer unknown error status");
                  }
                } 
                else {

                  // Obtain the actual buffer and read the encoded data into it
                  size_t l_iInputSize;
                  uint8_t *l_pInputBuffer = AMediaCodec_getInputBuffer(l_pCodec, l_iInputIndex, &l_iInputSize);
                  // LOGI("Sample size is: %d", (int)l_iInputSize);

                  ssize_t l_iSampleSize = AMediaExtractor_readSampleData(l_pExtractor, l_pInputBuffer, l_iInputSize);
                  auto l_iPresentationTimeUs = AMediaExtractor_getSampleTime(l_pExtractor);

                  if (l_iSampleSize > 0) {

                    // Enqueue the encoded data
                    AMediaCodec_queueInputBuffer(l_pCodec, l_iInputIndex, 0, l_iSampleSize,
                      l_iPresentationTimeUs,
                      0);
                    AMediaExtractor_advance(l_pExtractor);

                  } else {
                     LOGD("End of extractor data stream");
                    l_bExtracting = false;

                    // We need to tell the codec that we've reached the end of the stream
                    AMediaCodec_queueInputBuffer(l_pCodec, l_iInputIndex, 0, 0,
                      l_iPresentationTimeUs,
                      AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                  }
                }
              }

              // LOGD("A");

              if (l_bDecoding) {
                // Dequeue the decoded data
                AMediaCodecBufferInfo l_cInfo;
                ssize_t l_iOutputIndex = AMediaCodec_dequeueOutputBuffer(l_pCodec, &l_cInfo, 0);

                if (l_iOutputIndex >= 0) {
                  // Check whether this is set earlier
                  if (l_cInfo.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                    LOGD("Reached end of decoding stream");
                    l_bDecoding = false;
                  } 

                  // Valid index, acquire buffer
                  size_t l_iOutputSize;
                  uint8_t *l_pOutputBuffer = AMediaCodec_getOutputBuffer(l_pCodec, l_iOutputIndex, &l_iOutputSize);

                  // copy the data out of the buffer
                  memcpy(a_pTargetData + l_iBytesWritten, l_pOutputBuffer, l_cInfo.size);
                  l_iBytesWritten += l_cInfo.size;
                  AMediaCodec_releaseOutputBuffer(l_pCodec, l_iOutputIndex, false);
                } 
                else {
                  // The outputIndex doubles as a status return if its value is < 0
                  switch(l_iOutputIndex) {
                    case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                      // LOGD("dequeueOutputBuffer: try again later");
                      break;
                    case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                      LOGD("dequeueOutputBuffer: output buffers changed");
                      break;
                    case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
                      // LOGD("dequeueOutputBuffer: output outputFormat changed");
                      format = AMediaCodec_getOutputFormat(l_pCodec);
                      LOGD("outputFormat changed to: %s", AMediaFormat_toString(format));
                      break;
                  }
                }
              }
            }

            // Clean up
            AMediaFormat_delete(format);
            AMediaCodec_delete(l_pCodec);
            AMediaExtractor_delete(l_pExtractor);
            LOGD("Decoding done.");
          }

          return l_iBytesWritten;
        }

      public:
        int64_t getSize() const { return mBufferSize; }
        SAudioProperties getProperties() const { return mProperties; }
        const float* getData() const { return mBuffer.get(); }

        static CAssetDataSource* newFromCompressedAsset(AAssetManager& a_cAssetManager, const char* a_sFilename, SAudioProperties a_cTargetProperties) {
          AAsset *l_pAsset = AAssetManager_open(&a_cAssetManager, a_sFilename, AASSET_MODE_BUFFER);
          LOGD("Open data file \"%s\"", a_sFilename);
          if (!l_pAsset) {
            LOGE("Failed to open asset %s", a_sFilename);
            return nullptr;
          }

          off_t a_iAssetSize = AAsset_getLength(l_pAsset);
          // LOGD("Opened %s, size %ld", a_sFilename, a_iAssetSize);

          // Allocate memory to store the decompressed audio. We don't know the exact
          // size of the decoded data until after decoding so we make an assumption about the
          // maximum compression ratio and the decoded sample format (float for FFmpeg, int16 for NDK).
          const long maximumDataSizeInBytes = kMaxCompressionRatio * a_iAssetSize * sizeof(int16_t);
          uint8_t *l_pDecodedData = new uint8_t[maximumDataSizeInBytes];

          int64_t l_iBytesDecoded = decode(l_pAsset, l_pDecodedData, a_cTargetProperties);
          auto numSamples = l_iBytesDecoded / sizeof(int16_t);

          // Now we know the exact number of samples we can create a float array to hold the audio data
          auto l_pOutputBuffer = std::make_unique<float[]>(numSamples);

          // The NDK decoder can only decode to int16, we need to convert to floats
          oboe::convertPcm16ToFloat(
            reinterpret_cast<int16_t*>(l_pDecodedData),
            l_pOutputBuffer.get(),
            l_iBytesDecoded / sizeof(int16_t));

          delete[] l_pDecodedData;
          AAsset_close(l_pAsset);

          return new CAssetDataSource(std::move(l_pOutputBuffer),
            numSamples,
            a_cTargetProperties);
        }

      private:
        CAssetDataSource(std::unique_ptr<float[]> data, size_t size,
          const SAudioProperties properties)
          : mBuffer(std::move(data))
          , mBufferSize(size)
          , mProperties(properties) {
        }

        const std::unique_ptr<float[]> mBuffer;
        const int64_t mBufferSize;
        const SAudioProperties mProperties;
    };

    class CErrorCallback : public oboe::AudioStreamErrorCallback {
      public:
        virtual bool onError(oboe::AudioStream* a_pStream, oboe::Result a_eResult) {
          LOGE("onError: %i\n", (int)a_eResult);
          a_pStream->stop();
          return true;
        }

        virtual void onErrorBeforeClose(oboe::AudioStream* a_pStream, oboe::Result a_eResult) {
          LOGE("onErrorBeforeClose: %i\n", (int)a_eResult);
          a_pStream->stop();
        }

        virtual void onErrorAfterClose(oboe::AudioStream* a_pStream, oboe::Result a_eResult) {
          LOGE("onErrorAfterClose: %i\n", (int)a_eResult);
          a_pStream->stop();
        }
    };

    class CAudioPlayer : public oboe::AudioStreamCallback {
      private:
        irr::u32 m_iReadFrameIndex;
        irr::f32 m_fVolume;
        bool m_bPlaying;
        bool m_bLooped;
        CAssetDataSource *m_pSource;
        std::shared_ptr<oboe::AudioStream> m_pStream;
        oboe::AudioStreamBuilder m_cBuilder;
        
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
          m_iReadFrameIndex(0        ),
          m_fVolume        (1.0f     ),
          m_bPlaying       (false    ),
          m_bLooped        (a_bLooped),
          m_pSource        (nullptr  ),
          m_pStream        (nullptr  )
        {
          
          m_cBuilder.setDirection(oboe::Direction::Output)
            ->setFormatConversionAllowed(true)
            ->setSampleRate(44100)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Medium)
            ->setUsage(oboe::Usage::Game)
            ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
            ->setSharingMode(oboe::SharingMode::Shared)
            ->setFormat(oboe::AudioFormat::Float)
            ->setChannelCount(oboe::ChannelCount::Stereo)
            ->setErrorCallback(&m_cError);

          CAssetDataSource::SAudioProperties l_cTargetProperties {
            .m_iChannelCount = 2,
            .m_iSampleRate = 44100
          };

          std::string l_sSound = a_sSound;
          if (l_sSound.substr(0, 5) == "data/")
            l_sSound = l_sSound.substr(5);

          m_pSource = CAssetDataSource::newFromCompressedAsset(*(CGlobal::getInstance()->getAndroidApp()->activity->assetManager), l_sSound.c_str(), l_cTargetProperties);

          m_cBuilder.setCallback(this);
          oboe::Result l_eResult = m_cBuilder.openStream(m_pStream);

          if (l_eResult == oboe::Result::OK) {
            m_pStream->start();
          }
        }

        oboe::DataCallbackResult renderAudio(float *a_pTargetData, int32_t a_iNumFrames) {
          if (m_pSource == nullptr || m_pSource->getData() == nullptr)
            return oboe::DataCallbackResult::Stop;

          const CAssetDataSource::SAudioProperties properties = m_pSource->getProperties();

          if (m_bPlaying) {
            int64_t l_iFramesToRenderFromData = a_iNumFrames;
            int64_t l_iTotalSourceFrames = m_pSource->getSize() / properties.m_iChannelCount;
            const float *l_pData = m_pSource->getData();

            // Check whether we're about to reach the end of the recording
            if (!m_bLooped && m_iReadFrameIndex + a_iNumFrames >= l_iTotalSourceFrames) {
              l_iFramesToRenderFromData = l_iTotalSourceFrames - m_iReadFrameIndex;
              m_bPlaying = false;
            }

            for (int i = 0; i < l_iFramesToRenderFromData; ++i) {
              for (int j = 0; j < properties.m_iChannelCount; ++j) {
                a_pTargetData[(i * properties.m_iChannelCount) + j] = m_fVolume * l_pData[(m_iReadFrameIndex * properties.m_iChannelCount) + j];
              }

              // Increment and handle wraparound
              if (++m_iReadFrameIndex >= l_iTotalSourceFrames) {
                if (!m_bLooped)
                  return oboe::DataCallbackResult::Stop;
                else
                  m_iReadFrameIndex = 0;
              }
            }

            if (l_iFramesToRenderFromData < a_iNumFrames){
              // fill the rest of the buffer with silence
              renderSilence(&a_pTargetData[l_iFramesToRenderFromData], a_iNumFrames * properties.m_iChannelCount);
            }
          } else {
            renderSilence(a_pTargetData, a_iNumFrames * properties.m_iChannelCount);
          }

          return oboe::DataCallbackResult::Continue;
        }

        void resetPlayHead() { 
          m_iReadFrameIndex = 0; 
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
          if (m_pSource == nullptr || m_pSource->getData() == nullptr)
            return oboe::DataCallbackResult::Stop;

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
            m_aSounds[(int)enMarbleSounds::Rolling] = new CAudioPlayer("data/sounds/rolling.wav", true);
            m_aSounds[(int)enMarbleSounds::Wind   ] = new CAudioPlayer("data/sounds/wind.wav"   , true);
            m_aSounds[(int)enMarbleSounds::Skid   ] = new CAudioPlayer("data/sounds/skid.wav"   , true);
            m_aSounds[(int)enMarbleSounds::Stunned] = new CAudioPlayer("data/sounds/stunned.wav", true);

            m_aSounds[(int)enMarbleSounds::Rolling]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Wind   ]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Skid   ]->setVolume(0.0f);
            m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(0.0f);

            m_aSounds[(int)enMarbleSounds::Rolling]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Wind   ]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Skid   ]->setPlaying(true, true);
            m_aSounds[(int)enMarbleSounds::Stunned]->setPlaying(true, true);

            LOGD("Marble sounds loaded.");
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

        SMarbleSound m_aMarble;

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
          m_eSoundTrack      (enSoundTrack::enStNone) 
        {
          LOGI("CSoundInterface construtor.");
          oboe::DefaultStreamValues::SampleRate = 44100;

          std::string l_a2dSounds[] = {
            "data/sounds/button_hover.wav",
            "data/sounds/button_press.wav",
            "data/sounds/countdown.wav",
            "data/sounds/countdown_go.wav",
            ""
          };

          for (int i = 0; l_a2dSounds[i] != ""; i++) {
            LOGD("*******************");
            LOGD("Loading 2d sound %s", l_a2dSounds[i].c_str());
            LOGD("*******************");
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
            "data/sounds/respawn_start.wav",
            "data/sounds/respawn.wav",
            "data/sounds/hit.wav",
            "data/sounds/checkpoint.wav",
            "data/sounds/lap.wav",
            "data/sounds/gameover.wav",
            ""
          };

          for (int i = 0; l_aOneShots[i] != ""; i++) {
            LOGD("Loading one shot sound %s", l_aOneShots[i].c_str());
            m_aShots[i] = new CAudioPlayer(l_aOneShots[i], false);
          }
        }

        virtual ~CSoundInterface() {
          for (int i = 0; i < (int)enOneShots::Count; i++) {
            if (m_aShots[i] != nullptr) {
              delete m_aShots[i];
              m_aShots[i] = nullptr;
            }
          }
        }

        virtual void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) override {
        }

        virtual void setMasterVolume(irr::f32 a_fVolume) override {
          m_fMasterVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }

          for (std::vector<CAudioPlayer*>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fMenuVolume * m_fMasterVolume);
          }
        }

        virtual void setSfxVolumeGame(irr::f32 a_fVolume) override {
          m_fGameVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        virtual void setSfxVolumeMenu(irr::f32 a_fVolume) override {
          m_fMenuVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        virtual void setSoundtrackVolume(irr::f32 a_fVolume) override {
          m_fSoundtrackVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;
          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        virtual void muteAudio() override {
          // for (std::map<en2dSounds, CAudioPlayer *>::iterator it = m_mSounds.begin(); it != m_mSounds.end(); it++)
          //   it->second->setVolume(0.0f);
        }

        virtual void unmuteAudio() override {
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

        virtual irr::f32 getMasterVolume() override {
          return m_fMasterVolume;
        }

        virtual irr::f32 getSfxVolumeGame() override {
          return m_fGameVolume;
        }

        virtual irr::f32 getSfxVolumeMenu() override {
          return m_fMenuVolume;
        }

        virtual irr::f32 getSoundtrackVolume() override {
          return m_fSoundtrackVolume;
        }

        virtual void startSoundtrack(enSoundTrack a_eSoundTrack) override {
          if (a_eSoundTrack != m_eSoundTrack) {
            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(false, true);

            m_eSoundTrack = a_eSoundTrack;

            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(true, true);
          }
        }

        virtual void setSoundtrackFade(irr::f32 a_fValue) override {
          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fMasterVolume * m_fSoundtrackVolume * a_fValue);
          }
        }

        /**
        * Start a game, i.e. the sounds are initialized
        */
        virtual void startGame() override {
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
        virtual void playMarbleSounds(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fHit, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_aMarble.m_aSounds[(int)enMarbleSounds::Rolling]->setVolume(            a_bHasContact ? m_fMarbleParams[0] * a_fVolume * m_fMasterVolume * m_fGameVolume : 0.0f);
            m_aMarble.m_aSounds[(int)enMarbleSounds::Wind   ]->setVolume(                            m_fMarbleParams[1] * a_fVolume * m_fMasterVolume * m_fGameVolume       );
            m_aMarble.m_aSounds[(int)enMarbleSounds::Skid   ]->setVolume(a_bBrake && a_bHasContact ? m_fMarbleParams[2] * a_fVolume * m_fMasterVolume * m_fGameVolume : 0.0f);
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
        virtual void playViewportMarbleSound(int a_iMarble, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bBrake, bool a_bHasContact) override {
        }

        /**
        * Play the stunned sound of a marble (only played if the distance is less than 150)
        * @param a_iMarble ID of the stunned marble
        * @param a_cPosition position of the stunned marble
        */
        virtual void playMarbleStunned(int a_iMarble, const irr::core::vector3df &a_cPosition) override {
          if (m_iPlayerMarble == a_iMarble) {
            m_aMarble.m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(m_fMarbleParams[3] * m_fMasterVolume * m_fGameVolume);
          }
        }

        /**
        * Stunned state of a marble ends so the sound must be stopped
        * @param a_iMarble ID of the no longer stunned marbel
        */
        virtual void stopMarbleStunned(int a_iMarble) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_aMarble.m_aSounds[(int)enMarbleSounds::Stunned]->setVolume(0.0f);
          }
        }

        /**
        * Play a specific sound for a marble, i.e. checkpoint, lap passed
        * @param a_iMarble ID of the marble
        * @param a_eSound the sound to play
        */
        virtual void playMarbleOneShotSound(int a_iMarble, enOneShots a_eSound) override {
          if (a_iMarble == m_iPlayerMarble) {
            m_aShots[(int)a_eSound]->setVolume(m_fMasterVolume * m_fGameVolume);
            m_aShots[(int)a_eSound]->setPlaying(true, true);
          }
        }

        /**
        * Define a marble controlled by a local player
        */
        virtual void setViewportMarble(int a_iMarble) override {
          m_iPlayerMarble = a_iMarble;
        }

        virtual void play2d(en2dSounds a_eSound, irr::f32 a_fVolume, irr::f32 a_fPan) override {
          if (m_aSounds2d[(int)a_eSound] != nullptr) {
            m_aSounds2d[(int)a_eSound]->setPlaying(true, a_fPan == 0.0f);
            m_aSounds2d[(int)a_eSound]->setVolume(a_fVolume * m_fMasterVolume * (a_eSound == en2dSounds::ButtonHover || a_eSound == en2dSounds::ButtonPress ? m_fMenuVolume : m_fGameVolume));
          }
        }

        virtual void clear3dSounds() override {
          // 3d sounds not supported by Android
        }

        virtual void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) override {
          m_cPosition  = a_pCamera->getAbsolutePosition();
          m_cDirection = a_pCamera->getTarget();
          m_cVelocity  = a_vVel;

          m_cViewMatrix = a_pCamera->getViewMatrix();
        }

        virtual void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) override {
        }

        virtual void assignSoundtracks(const std::map<enSoundTrack, std::tuple<std::string, bool>> &a_mSoundTracks) override {
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
      return new CSoundInterface(a_pDevice);
    }
  }
}
