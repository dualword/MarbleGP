// (w) 2021 by Dustbin::Games / Christian Keimel
#include <vorbis/vorbisfile.h>
#include <sound/OggHelpers.h>
#include <sound/CSoundData.h>
#include <CGlobal.h>
#include <ogg/ogg.h>
#include <iostream>
#include <AL/alc.h>
#include <AL/al.h>
#include <fstream>
#include <vector>

/* Start of functions taken from https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/ */

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)

bool check_al_errors(const std::string& filename, const std::uint_fast32_t line)
{
  ALenum error = alGetError();
  if(error != AL_NO_ERROR)
  {
    std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
    switch(error)
    {
    case AL_INVALID_NAME:
      std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
      break;
    case AL_INVALID_ENUM:
      std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
      break;
    case AL_INVALID_VALUE:
      std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
      break;
    case AL_INVALID_OPERATION:
      std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
      break;
    case AL_OUT_OF_MEMORY:
      std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
      break;
    default:
      std::cerr << "UNKNOWN AL ERROR: " << error;
    }
    std::cerr << std::endl;
    return false;
  }
  return true;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename,
  const std::uint_fast32_t line,
  alFunction function,
  Params... params)
  ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
{
  auto ret = function(std::forward<Params>(params)...);
  check_al_errors(filename, line);
  return ret;
}

template<typename alFunction, typename... Params>
auto alCallImpl(const char* filename,
  const std::uint_fast32_t line,
  alFunction function,
  Params... params)
  ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
{
  function(std::forward<Params>(params)...);
  return check_al_errors(filename, line);
}






#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
{
  ALCenum error = alcGetError(device);
  if(error != ALC_NO_ERROR)
  {
    std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n" ;
    switch(error)
    {
    case ALC_INVALID_VALUE:
      std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
      break;
    case ALC_INVALID_DEVICE:
      std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
      break;
    case ALC_INVALID_CONTEXT:
      std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
      break;
    case ALC_INVALID_ENUM:
      std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
      break;
    case ALC_OUT_OF_MEMORY:
      std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
      break;
    default:
      std::cerr << "UNKNOWN ALC ERROR: " << error;
    }
    std::cerr << std::endl;
    return false;
  }
  return true;
}

template<typename alcFunction, typename... Params>
auto alcCallImpl(const char* filename, 
  const std::uint_fast32_t line, 
  alcFunction function, 
  ALCdevice* device, 
  Params... params)
  ->typename std::enable_if_t<std::is_same_v<void,decltype(function(params...))>,bool>
{
  function(std::forward<Params>(params)...);
  return check_alc_errors(filename,line,device);
}

template<typename alcFunction, typename ReturnType, typename... Params>
auto alcCallImpl(const char* filename,
  const std::uint_fast32_t line,
  alcFunction function,
  ReturnType& returnValue,
  ALCdevice* device, 
  Params... params)
  ->typename std::enable_if_t<!std::is_same_v<void,decltype(function(params...))>,bool>
{
  returnValue = function(std::forward<Params>(params)...);
  return check_alc_errors(filename,line,device);
}
/* End of functions taken from https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/ */



namespace dustbin {
  namespace sound {

    CAudioDevice::CAudioDevice() : m_pALContext(nullptr), m_pAlDevice(nullptr), m_bInitialized(true) {
      m_pAlDevice = alcOpenDevice(nullptr);
      if (!m_pAlDevice)
      {
        m_pAlDevice = nullptr;
      }

      if (m_pAlDevice != nullptr) {
        if (!alcCall(alcCreateContext, m_pALContext, m_pAlDevice, m_pAlDevice, nullptr) || !m_pALContext)
        {
          std::cerr << "ERROR: Could not create audio context" << std::endl;
          /* probably exit program */
          m_bInitialized = false;
        }
        else {
          ALCboolean l_bContextMadeCurrent = false;
          if (!alcCall(alcMakeContextCurrent, l_bContextMadeCurrent, m_pAlDevice, m_pALContext) || l_bContextMadeCurrent != ALC_TRUE)
          {
            std::cerr << "ERROR: Could not make audio context current" << std::endl;
            /* probably exit or give up on having sound */
            m_bInitialized = false;
          }
          else {
            alCall(alDistanceModel, AL_INVERSE_DISTANCE_CLAMPED);
            alCall(alListenerf, AL_GAIN, 0.95f);
            // alCall(alListenerf, AL_MIN_GAIN, 0.0f);
          }
        }
      }
      else m_bInitialized = false;
    }

    CAudioDevice::~CAudioDevice() {
      if (m_bInitialized) {
        ALCboolean l_bContextMadeCurrent = false;
        if (!alcCall(alcMakeContextCurrent, l_bContextMadeCurrent, m_pAlDevice, nullptr))
        {
          /* what can you do? */
        }

        if (!alcCall(alcDestroyContext, m_pAlDevice, m_pALContext))
        {
          /* not much you can do */
        }

        ALCboolean l_bClosed;
        if (!alcCall(alcCloseDevice, l_bClosed, m_pAlDevice, m_pAlDevice))
        {
          /* do we care? */
        }
      }
    }

    ALCdevice *CAudioDevice::getDevice() {
      return m_pAlDevice;
    }

    ALCcontext *CAudioDevice::getContext() {
      return m_pALContext;
    }

    void CAudioDevice::updateListener(irr::scene::ICameraSceneNode* a_pCamera, const irr::core::vector3df &a_vVelocity) {
      if (m_bInitialized) {
        alCall(alListener3f, AL_POSITION, a_pCamera->getAbsolutePosition().X, a_pCamera->getAbsolutePosition().Y, a_pCamera->getAbsolutePosition().Z);

        irr::core::vector3df l_vDirection = a_pCamera->getTarget() - a_pCamera->getAbsolutePosition(),
                             l_vUp        = a_pCamera->getUpVector();

        float l_aDirectionVect[6];   
        l_aDirectionVect[0] = l_vDirection.X;
        l_aDirectionVect[1] = l_vDirection.Y;
        l_aDirectionVect[2] = l_vDirection.Z;
        l_aDirectionVect[3] = l_vUp.X;
        l_aDirectionVect[4] = l_vUp.Y;
        l_aDirectionVect[5] = l_vUp.Z;
        alCall(alListenerfv, AL_ORIENTATION, l_aDirectionVect);
        alCall(alListener3f, AL_VELOCITY, a_vVelocity.X, a_vVelocity.Y, a_vVelocity.Z);
      }
    }

    void CAudioDevice::mute() {
      alCall(alListenerf, AL_GAIN, 0.0f);
    }

    void CAudioDevice::unmute() {
      alCall(alListenerf, AL_GAIN, 0.95f);
    }




    IAudioBuffer::IAudioBuffer(const std::wstring &a_sName) : m_iReferenceCount(0), m_iBuffer(0), m_sName(a_sName), m_pListener(nullptr) {
    }

    IAudioBuffer::~IAudioBuffer() {
    }

    void IAudioBuffer::grab() {
      m_iReferenceCount++;
    }

    bool IAudioBuffer::drop() {
      m_iReferenceCount--;

      if (m_iReferenceCount <= 0) {
        if (m_pListener != nullptr)
          m_pListener->bufferDeleted(this);

        return true;
      }

      return false;
    }

    ALuint IAudioBuffer::getBuffer() {
      return m_iBuffer;
    }

    const std::wstring& IAudioBuffer::getName() {
      return m_sName;
    }

    void IAudioBuffer::setDeletionListener(IDeletionListener* a_pListener) {
      m_pListener = a_pListener;
    }





    ISound::ISound(IAudioBuffer *a_pBuffer) : m_pBuffer(a_pBuffer), m_iSource(0) {
      if (m_pBuffer != nullptr)
        m_pBuffer->grab();
    }

    ISound::~ISound() {
      if (m_pBuffer != nullptr)
        m_pBuffer->drop();
    }

    ALuint ISound::getSource() {
      return m_iSource;
    }

    CSound3d::CSound3d(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist) : ISound(a_pBuffer), m_bLoop(a_bLoop), m_bPlaying(false) {
      alCall(alGenSources, 1, &m_iSource);
      alCall(alSourcef, m_iSource, AL_PITCH, 1.0f);
      alCall(alSourcef, m_iSource, AL_GAIN, a_fVolume);
      alCall(alSource3f, m_iSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
      alCall(alSource3f, m_iSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      alCall(alSourcei, m_iSource, AL_LOOPING, m_bLoop ? AL_TRUE : AL_FALSE);
      alCall(alSourcef, m_iSource, AL_MAX_DISTANCE, a_fMaxDist);
      alCall(alSourcef, m_iSource, AL_REFERENCE_DISTANCE, a_fMinDist);
      alCall(alSourcef, m_iSource, AL_ROLLOFF_FACTOR, 1.0f);
      alCall(alSourcei, m_iSource, AL_SOURCE_RELATIVE, AL_FALSE);
      alCall(alSourcef, m_iSource, AL_MIN_GAIN, 0.0f);
      alCall(alSourcei, m_iSource, AL_BUFFER, m_pBuffer->getBuffer());
    }

    CSound3d::~CSound3d() {
      alCall(alDeleteSources, 1, &m_iSource);
    }

    void CSound3d::play() {
      if (m_iSource == 0) return;

      if (!m_bLoop || !m_bPlaying) {
        alCall(alSourcePlay, m_iSource);
        m_bPlaying = true;
      }
    }

    void CSound3d::stop() {
      if (m_iSource == 0) return;
      alCall(alSourceStop, m_iSource);
    }

    void CSound3d::setPosition(const irr::core::vector3df& a_cPos) {
      if (m_iSource == 0) return;
      alCall(alSource3f, m_iSource, AL_POSITION, a_cPos.X, a_cPos.Y, a_cPos.Z);
    }

    void CSound3d::setVelocity(const irr::core::vector3df& a_cVel) {
      if (m_iSource == 0) return;
      alCall(alSource3f, m_iSource, AL_VELOCITY, a_cVel.X, a_cVel.Y, a_cVel.Z);
    }

    void CSound3d::setVolume(irr::f32 a_fVolume) {
      if (m_iSource == 0) return;
      alCall(alSourcef, m_iSource, AL_GAIN, a_fVolume);
    }

    CSound3dFixed::CSound3dFixed(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist, const irr::core::vector3df& a_cPos) : CSound3d(a_pBuffer, a_bLoop, a_fVolume, a_fMinDist, a_fMaxDist) {
      if (m_iSource > 0) {
        alCall(alSourcei, m_iSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alCall(alSource3f, m_iSource, AL_POSITION, a_cPos.X, a_cPos.Y, a_cPos.Z);
      }
    }

    CSound3dFixed::~CSound3dFixed() {
    }

    void CSound3dFixed::setPosition(const irr::core::vector3df& a_cPos) {
    }

    void CSound3dFixed::setVelocity(const irr::core::vector3df& a_cVel) {
    }



    CSound2d::CSound2d(IAudioBuffer* a_pBuffer, bool a_bLoop) : ISound(a_pBuffer), m_bLoop(a_bLoop), m_bPlaying(false) {
      alCall(alGenSources, 1, &m_iSource);
      alCall(alSourcef, m_iSource, AL_PITCH, 1.0f);
      alCall(alSourcef, m_iSource, AL_GAIN, 1.0f);
      alCall(alSource3f, m_iSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
      alCall(alSource3f, m_iSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
      alCall(alSourcei, m_iSource, AL_LOOPING, m_bLoop ? AL_TRUE : AL_FALSE);
      alCall(alSourcei, m_iSource, AL_SOURCE_RELATIVE, AL_TRUE);
      alCall(alSourcei, m_iSource, AL_BUFFER, m_pBuffer->getBuffer());
    }

    CSound2d::~CSound2d() {
      alCall(alDeleteSources, 1, &m_iSource);
    }

    void CSound2d::play() {
      if (m_iSource == 0) return;

      if (!m_bLoop || !m_bPlaying) {
        alCall(alSourcePlay, m_iSource);
        m_bPlaying = true;
      }
    }

    void CSound2d::stop() {
      if (m_iSource == 0) return;
      alCall(alSourceStop, m_iSource);
      m_bPlaying = false;
    }

    void CSound2d::setPosition(const irr::core::vector3df &a_cPos) {
      if (m_iSource == 0) return;
      alCall(alSource3f, m_iSource, AL_POSITION, a_cPos.X, a_cPos.Y, a_cPos.Z);
    }

    void CSound2d::setVelocity(const irr::core::vector3df& a_cVel) {
      // 2d sounds don't move
    }

    void CSound2d::setVolume(irr::f32 a_fVolume) {
      if (m_iSource == 0) return;
      alCall(alSourcef, m_iSource, AL_GAIN, a_fVolume);
    }






    CAudioBufferOggVorbis::CAudioBufferOggVorbis(const std::wstring& a_sName) : IAudioBuffer(a_sName) {
      irr::io::IReadFile* l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(irr::core::stringc(irr::core::stringw(a_sName.c_str())).c_str());

      char   *l_pBuffer = new char[l_pFile->getSize()];
      size_t  l_iSize   = (size_t)l_pFile->getSize();

      l_pFile->read((void *)l_pBuffer, l_pFile->getSize());
      l_pFile->drop();

      ov_callbacks l_cCallbacks;
      SOggFile     l_cFile;

      l_cFile.m_pCurrent  = l_cFile.m_pFileData = l_pBuffer;
      l_cFile.m_iFileSize = l_iSize;

      OggVorbis_File *l_pOggFile = new OggVorbis_File();
      memset(l_pOggFile, 0, sizeof(OggVorbis_File));

      l_cCallbacks.read_func  = readOggCallback;
      l_cCallbacks.seek_func  = seekOggCallback;
      l_cCallbacks.close_func = closeOggCallback;
      l_cCallbacks.tell_func  = tellOggCallback;

      int l_iResult = ov_open_callbacks((void *)&l_cFile, l_pOggFile, nullptr, -1, l_cCallbacks);

      vorbis_info *l_pVorbisInfo = ov_info(l_pOggFile,-1);

  
      long l_iPCMSize = (long)ov_pcm_total(l_pOggFile, -1);
      int l_iPos = 0;
      bool l_bEof = false;

      std::vector<char> l_vPcm;

      int l_iCurrentSection;

      while (!l_bEof) {
        char p[4096];
        long l_iRead = ov_read(l_pOggFile, p, 4096, 0, 2, 1, &l_iCurrentSection);

        if (l_iRead == 0)
          l_bEof = true;
        else if (l_iRead < 0) {
          printf("Error %s\n", l_iRead == OV_HOLE ? "OV_HOLE" : l_iRead == OV_EBADLINK ? "OV_EBADLINK" : l_iRead == OV_EINVAL ? "OV_EINVAL" : std::to_string(l_iRead).c_str());
          l_bEof = true;
        }
        else {
          l_vPcm.insert(l_vPcm.end(), p, p + l_iRead);
          // memcpy(&l_pPCM[l_iPos], p, l_iRead);
          l_iPos += l_iRead;
        }
      }

      ALenum l_eFormat;
      if (l_pVorbisInfo->channels == 1)
        l_eFormat = AL_FORMAT_MONO16;
      else if (l_pVorbisInfo->channels == 2)
        l_eFormat = AL_FORMAT_STEREO16;
      else {
        std::cerr << "ERROR: unrecognised wave format" << std::endl;
        return;
      }

      alCall(alGenBuffers, 1, &m_iBuffer);
      alCall(alBufferData, m_iBuffer, l_eFormat, &l_vPcm[0], (ALsizei)l_vPcm.size(), l_pVorbisInfo->rate);

      ov_clear(l_pOggFile);
      delete l_pOggFile;
      delete []l_pBuffer;
    }

    CAudioBufferOggVorbis::~CAudioBufferOggVorbis() {
      if (m_iBuffer > 0) {
        printf("*** Delete ogg-vorbis audio buffer \"%ls\"\n", getName().c_str());
        alCall(alDeleteBuffers, 1, &m_iBuffer);
      }
    }
  }
}