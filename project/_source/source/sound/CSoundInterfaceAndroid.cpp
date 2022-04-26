// (w) 2021 by Dustbin::Games / Christian Keimel

#include <media/NdkMediaExtractor.h>
#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <android/asset_manager.h>
#include <oboe/Oboe.h>
#include <CGlobal.h>
#include <string>
#include <map>

#include <aaudio/AAudio.h>

#include <stdio.h>
#include <android/log.h>
#include <vector>

#define APP_NAME "RhythmGame"
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
          LOGD("Using NDK decoder");

          // open asset as file descriptor
          off_t l_iStart, l_iLength;
          int l_iFd = AAsset_openFileDescriptor(a_pAsset, &l_iStart, &l_iLength);

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
            LOGD("Source sample rate %d", l_iSampleRate);
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
            LOGD("Got channel count %d", l_iChannelCount);
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

          const char *l_sFormatStr = AMediaFormat_toString(format);
          LOGD("Output format %s", l_sFormatStr);

          const char *l_sMimeType;
          if (AMediaFormat_getString(format, AMEDIAFORMAT_KEY_MIME, &l_sMimeType)) {
            LOGD("Got mime type %s", l_sMimeType);
          } else {
            LOGE("Failed to get mime type");
            return 0;
          }

          // Obtain the correct decoder
          LOGD("1");
          AMediaCodec *l_pCodec = nullptr;
          LOGD("2");
          AMediaExtractor_selectTrack(l_pExtractor, 0);
          LOGD("3");
          l_pCodec = AMediaCodec_createDecoderByType(l_sMimeType);
          LOGD("4");
          AMediaCodec_configure(l_pCodec, format, nullptr, nullptr, 0);
          LOGD("5");
          AMediaCodec_start(l_pCodec);
          LOGD("6");

          // DECODE

          bool l_bExtracting = true;
          bool l_bDecoding = true;
          int32_t l_iBytesWritten = 0;

          while(l_bExtracting || l_bDecoding) {
            if (l_bExtracting) {

              // Obtain the index of the next available input buffer
              LOGD("7");
              ssize_t l_iInputIndex = AMediaCodec_dequeueInputBuffer(l_pCodec, 2000);
              LOGD("8");
              LOGI("Got input buffer %d", (int)l_iInputIndex);

              // The input index acts as a status if its negative
              if (l_iInputIndex < 0){
                if (l_iInputIndex == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
                  LOGI("Codec.dequeueInputBuffer try again later");
                } 
                else {
                  LOGE("Codec.dequeueInputBuffer unknown error status");
                }
              } 
              else {

                // Obtain the actual buffer and read the encoded data into it
                size_t l_iInputSize;
                uint8_t *l_pInputBuffer = AMediaCodec_getInputBuffer(l_pCodec, l_iInputIndex, &l_iInputSize);
                LOGI("Sample size is: %d", (int)l_iInputSize);

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

            if (l_bDecoding) {
              // Dequeue the decoded data
              AMediaCodecBufferInfo l_cInfo;
              ssize_t l_iOutputIndex = AMediaCodec_dequeueOutputBuffer(l_pCodec, &l_cInfo, 0);

              if (l_iOutputIndex >= 0) {

                // Check whether this is set earlier
                if (l_cInfo.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM){
                  LOGD("Reached end of decoding stream");
                  l_bDecoding = false;
                } 

                // Valid index, acquire buffer
                size_t l_iOutputSize;
                uint8_t *l_pOutputBuffer = AMediaCodec_getOutputBuffer(l_pCodec, l_iOutputIndex, &l_iOutputSize);

                LOGI("Got output buffer index %d, buffer size: %d, info size: %d writing to pcm index %d",
                  (int)l_iOutputIndex,
                  (int)l_iOutputSize,
                  (int)l_cInfo.size,
                  0);

                // copy the data out of the buffer
                memcpy(a_pTargetData + l_iBytesWritten, l_pOutputBuffer, l_cInfo.size);
                l_iBytesWritten += l_cInfo.size;
                AMediaCodec_releaseOutputBuffer(l_pCodec, l_iOutputIndex, false);
              } else {

                // The outputIndex doubles as a status return if its value is < 0
                switch(l_iOutputIndex) {
                  case AMEDIACODEC_INFO_TRY_AGAIN_LATER:
                    LOGD("dequeueOutputBuffer: try again later");
                    break;
                  case AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED:
                    LOGD("dequeueOutputBuffer: output buffers changed");
                    break;
                  case AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED:
                    LOGD("dequeueOutputBuffer: output outputFormat changed");
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

          return l_iBytesWritten;        
        }

      public:
        int64_t getSize() const { return mBufferSize; }
        SAudioProperties getProperties() const { return mProperties; }
        const float* getData() const { return mBuffer.get(); }

        static CAssetDataSource* newFromCompressedAsset(AAssetManager& a_cAssetManager, const char* a_sFilename, SAudioProperties a_cTargetProperties) {
          AAsset *l_pAsset = AAssetManager_open(&a_cAssetManager, a_sFilename, AASSET_MODE_BUFFER);
          if (!l_pAsset) {
            LOGE("Failed to open asset %s", a_sFilename);
            return nullptr;
          }

          off_t a_iAssetSize = AAsset_getLength(l_pAsset);
          LOGD("Opened %s, size %ld", a_sFilename, a_iAssetSize);

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
        CAudioPlayer(const std::string &a_sSound, bool a_bLooped, oboe::AudioStreamBuilder &a_cBuilder) :
          m_iReadFrameIndex(0        ),
          m_fVolume        (1.0f     ),
          m_bPlaying       (false    ),
          m_bLooped        (a_bLooped),
          m_pSource        (nullptr  ),
          m_pStream        (nullptr  )
        {
          
          a_cBuilder.setDirection(oboe::Direction::Output)
            ->setFormatConversionAllowed(true)
            ->setSampleRate(44100)
            ->setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Fastest)
            ->setUsage(oboe::Usage::Game)
            ->setPerformanceMode(oboe::PerformanceMode::None)
            ->setSharingMode(oboe::SharingMode::Shared)
            ->setFormat(oboe::AudioFormat::Float)
            ->setChannelCount(oboe::ChannelCount::Stereo)
            ->setErrorCallback(&m_cError);

          CAssetDataSource::SAudioProperties l_cTargetProperties {
            .m_iChannelCount = 2,
            .m_iSampleRate = 44100
          };

          m_pSource = CAssetDataSource::newFromCompressedAsset(*(CGlobal::getInstance()->getAndroidApp()->activity->assetManager), a_sSound.c_str(), l_cTargetProperties);

          a_cBuilder.setCallback(this);
          oboe::Result l_eResult = a_cBuilder.openStream(m_pStream);

          if (l_eResult == oboe::Result::OK) {
            m_pStream->start();
          }
        }

        void renderAudio(float *a_pTargetData, int32_t a_iNumFrames) {
          if (m_pSource == nullptr || m_pSource->getData() == nullptr)
            return;

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
              if (++m_iReadFrameIndex >= l_iTotalSourceFrames) m_iReadFrameIndex = 0;
            }

            if (l_iFramesToRenderFromData < a_iNumFrames){
              // fill the rest of the buffer with silence
              renderSilence(&a_pTargetData[l_iFramesToRenderFromData], a_iNumFrames * properties.m_iChannelCount);
            }

          } else {
            renderSilence(a_pTargetData, a_iNumFrames * properties.m_iChannelCount);
          }
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

        void setPlaying(bool a_bPlay) { 
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
        irr::f32 m_fMasterVolume,
                 m_fSoundtrackVolume,
                 m_fGameVolume,
                 m_fMenuVolume;

        bool m_bMenu;

        std::map<enSoundTrack, CAudioPlayer *> m_mSoundTracks;
        std::map<std::wstring, CAudioPlayer *> m_mSounds;
        std::map<std::wstring, irr::f32      > m_mParameters;

        std::vector<CAudioPlayer *> m_vMenuSounds,
                                    m_vGameSounds;

        enSoundTrack m_eSoundTrack;

        oboe::AudioStreamBuilder m_cBuilder;

      public:
        CSoundInterface(irr::IrrlichtDevice *a_pDevice) : 
          m_fMasterVolume    (1.0f),
          m_fSoundtrackVolume(1.0f),
          m_fGameVolume      (1.0f),
          m_fMenuVolume      (1.0f),
          m_bMenu            (true),
          m_eSoundTrack(enSoundTrack::enStNone) 
        {
          oboe::DefaultStreamValues::SampleRate = 44100;
        }

        virtual ~CSoundInterface() {
        }

        void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) {
        }

        void setMasterVolume(irr::f32 a_fVolume) {
          m_fMasterVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }

          for (std::vector<CAudioPlayer*>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fMenuVolume * m_fMasterVolume);
          }

          for (std::map<enSoundTrack, CAudioPlayer *>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        void setSfxVolumeGame(irr::f32 a_fVolume) {
          m_fGameVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        void setSfxVolumeMenu(irr::f32 a_fVolume) {
          m_fMenuVolume = a_fVolume > 1.0f ? 1.0f : a_fVolume < 0.0f ? 0.0f : a_fVolume;

          for (std::vector<CAudioPlayer *>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }
        }

        void setSoundtrackVolume(irr::f32 a_fVolume) {
          m_fSoundtrackVolume = a_fVolume < 0.0f ? 0.0f : a_fVolume > 1.0f ? 1.0f : a_fVolume;
          for (std::map<std::wstring, CAudioPlayer *>::iterator it = m_mSounds.begin(); it != m_mSounds.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        void muteAudio() {
          for (std::map<std::wstring, CAudioPlayer *>::iterator it = m_mSounds.begin(); it != m_mSounds.end(); it++)
            it->second->setVolume(0.0f);
        }

        void unmuteAudio() {
          for (std::vector<CAudioPlayer *>::iterator it = m_vGameSounds.begin(); it != m_vGameSounds.end(); it++) {
            (*it)->setVolume(m_fGameVolume * m_fMasterVolume);
          }

          for (std::vector<CAudioPlayer*>::iterator it = m_vMenuSounds.begin(); it != m_vMenuSounds.end(); it++) {
            (*it)->setVolume(m_fMenuVolume * m_fMasterVolume);
          }

          for (std::map<enSoundTrack, CAudioPlayer *>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fSoundtrackVolume * m_fMasterVolume);
          }
        }

        irr::f32 getMasterColume() {
          return m_fMasterVolume;
        }

        irr::f32 getSfxVolumeGame() {
          return m_fGameVolume;
        }

        irr::f32 getSfxVolumeMenu() {
          return m_fMenuVolume;
        }

        irr::f32 getSoundtrackVolume() {
          return m_fSoundtrackVolume;
        }

        void startSoundtrack(enSoundTrack a_eSoundTrack) {
          if (a_eSoundTrack != m_eSoundTrack) {
            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(false);

            m_eSoundTrack = a_eSoundTrack;

            if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
              m_mSoundTracks[m_eSoundTrack]->setPlaying(true);
          }
        }

        void setSoundtrackFade(irr::f32 a_fValue) {
          for (std::map<enSoundTrack, CAudioPlayer*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
            it->second->setVolume(m_fMasterVolume * m_fSoundtrackVolume * a_fValue);
          }
        }

        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bLooped) {
          // 3d sounds not supported by Android
        }

        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, irr::f32 a_fVolume, bool a_bLooped) {
          // 3d sounds not supported by Android
        }

        void play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan) {
          if (m_mSounds.find(a_sName) != m_mSounds.end()) {
            m_mSounds[a_sName]->setPlaying(true);
          }
        }

        void clear3dSounds() {
          // 3d sounds not supported by Android
        }

        void addSoundParameter(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fMinDistance, irr::f32 a_fMaxDistance) {
          // As we do not support 3d audio in Android we just store the volume
          m_mParameters[a_sName] = a_fVolume;
        }

        void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) {
          // 3d sounds not supported by Android
        }

        void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) {
          std::wstring l_sName = a_sName;
          if (l_sName.substr(0, 5) == L"data/")
            l_sName = l_sName.substr(5);

          if (l_sName.find_last_of(L'.') != std::string::npos) {
            l_sName = l_sName.substr(0, l_sName.find_last_of(L'.')) + L".wav";
          }

          if (m_mSounds.find(l_sName) == m_mSounds.end()) {
            m_mSounds[a_sName] = new CAudioPlayer(helpers::ws2s(l_sName), false, m_cBuilder);

            if (a_bMenuSound)
              m_vMenuSounds.push_back(m_mSounds[a_sName]);
            else {
              m_vGameSounds.push_back(m_mSounds[a_sName]);
            }
          }
        }

        void assignSound (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, bool a_bDoppler) {
        }

        void assignFixed (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, const irr::core::vector3df &a_vPos) {
        }


        void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) {
          for (std::map<enSoundTrack, std::wstring>::const_iterator it = a_mSoundTracks.begin(); it != a_mSoundTracks.end(); it++) {
            if (m_mSounds.find(it->second) != m_mSounds.end()) {
              m_mSounds[it->second]->setLooping(it->first != enSoundTrack::enStFinish);
              m_mSoundTracks[it->first] = m_mSounds[it->second];
            }
          }
        }

        void setMenuFlag(bool a_bMenu) {
          m_bMenu = a_bMenu;
        }
    };

    ISoundInterface* createSoundInterface(irr::IrrlichtDevice* a_pDevice) {
      return new CSoundInterface(a_pDevice);
    }
  }
}
