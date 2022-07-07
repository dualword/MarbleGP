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

    CSound3d::CSound3d(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist) : ISound(a_pBuffer), m_bLoop(a_bLoop), m_bPlaying(false), m_fVolume(a_fVolume) {
    }

    CSound3d::~CSound3d() {
    }

    void CSound3d::play() {
    }

    void CSound3d::stop() {
    }

    void CSound3d::setPosition(const irr::core::vector3df& a_cPos) {
    }

    void CSound3d::setVelocity(const irr::core::vector3df& a_cVel) {
    }

    void CSound3d::setVolume(irr::f32 a_fVolume) {
    }

    CSound3dFixed::CSound3dFixed(IAudioBuffer* a_pBuffer, bool a_bLoop, irr::f32 a_fVolume, irr::f32 a_fMinDist, irr::f32 a_fMaxDist, const irr::core::vector3df& a_cPos) : CSound3d(a_pBuffer, a_bLoop, a_fVolume, a_fMinDist, a_fMaxDist) {
    }

    CSound3dFixed::~CSound3dFixed() {
    }

    void CSound3dFixed::setPosition(const irr::core::vector3df& a_cPos) {
    }

    void CSound3dFixed::setVelocity(const irr::core::vector3df& a_cVel) {
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