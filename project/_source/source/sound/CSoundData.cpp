// (w) 2021 by Dustbin::Games / Christian Keimel
#include <sound/CSoundData.h>
#include <CGlobal.h>
#include <iostream>
#include <fstream>
#include <vector>

namespace dustbin {
  namespace sound {

    CAudioDevice::CAudioDevice() : m_bInitialized(true) {
    }

    CAudioDevice::~CAudioDevice() {
      if (m_bInitialized) {
      }
    }

    void CAudioDevice::updateListener(irr::scene::ICameraSceneNode* a_pCamera, const irr::core::vector3df &a_vVelocity) {
      if (m_bInitialized) {
      }
    }

    void CAudioDevice::mute() {
    }

    void CAudioDevice::unmute() {
    }




    IAudioBuffer::IAudioBuffer(const std::wstring &a_sName) : m_iReferenceCount(0), m_sName(a_sName), m_pListener(nullptr) {
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

    const std::wstring& IAudioBuffer::getName() {
      return m_sName;
    }

    void IAudioBuffer::setDeletionListener(IDeletionListener* a_pListener) {
      m_pListener = a_pListener;
    }





    ISound::ISound(IAudioBuffer *a_pBuffer) : m_pBuffer(a_pBuffer) {
      if (m_pBuffer != nullptr)
        m_pBuffer->grab();
    }

    ISound::~ISound() {
      if (m_pBuffer != nullptr)
        m_pBuffer->drop();
    }

    CSound2d::CSound2d(IAudioBuffer* a_pBuffer, bool a_bLoop) : ISound(a_pBuffer), m_bLoop(a_bLoop), m_bPlaying(false) {
    }

    CSound2d::~CSound2d() {
    }

    void CSound2d::play() {
    }

    void CSound2d::stop() {
    }

    void CSound2d::setPosition(const irr::core::vector3df &a_cPos) {
    }

    void CSound2d::setVelocity(const irr::core::vector3df& a_cVel) {
      // 2d sounds don't move
    }

    void CSound2d::setVolume(irr::f32 a_fVolume) {
    }






  }
}