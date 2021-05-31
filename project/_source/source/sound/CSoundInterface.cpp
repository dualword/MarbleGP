// (w) 2021 by Dustbin::Games / Christian Keimel
#include <sound/CSoundInterface.h>
#include <sound/CSoundData.h>
#include <iostream>
#include <AL/alc.h>
#include <fstream>
#include <AL/al.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace audio {

    void CSoundInterface::preloadSound(const std::wstring& a_sName) {
      if (m_mAudioBuffer.find(a_sName) == m_mAudioBuffer.end()) {
        IAudioBuffer *p = new CAudioBufferOggVorbis(a_sName);
        p->setDeletionListener(this);
        m_mAudioBuffer[a_sName] = p;
      }
    }

    void CSoundInterface::assignSound(const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, bool a_bDoppler) {
      if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
        irr::f32 l_fVolume  = 0.6f,
                 l_fMinDist = 50.0f,
                 l_fMaxDist = 500.0f;

        std::wstring l_sName = a_sName.substr(0, a_sName.size() - 4);
        l_sName += L".ogg";

        if (m_mSoundParameters.find(l_sName) != m_mSoundParameters.end()) {
          l_fVolume  = std::get<0>(m_mSoundParameters[l_sName]);
          l_fMinDist = std::get<1>(m_mSoundParameters[l_sName]);
          l_fMaxDist = std::get<2>(m_mSoundParameters[l_sName]);
        }

        ISound* p = new CSound3d(m_mAudioBuffer[a_sName], a_bLoop, l_fVolume, l_fMinDist, l_fMaxDist);
      
        if (m_m3dSounds.find(a_iId) == m_m3dSounds.end())
          m_m3dSounds[a_iId] = std::map<std::wstring, ISound *>();

        m_m3dSounds[a_iId][a_sName] = p;

        if (!a_bDoppler)
          m_vNoDopplerSounds.push_back(p);
      }
    }

    void CSoundInterface::assignFixed (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, const irr::core::vector3df &a_vPos) {
      if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
        printf("Assign fixed \"%ls\" to %i\n", a_sName.c_str(), a_iId);

        ISound* p = new CSound3dFixed(m_mAudioBuffer[a_sName], a_bLoop, 1.0f, 10.0f, 100.0f, a_vPos);

        if (m_m3dSounds.find(a_iId) == m_m3dSounds.end())
          m_m3dSounds[a_iId] = std::map<std::wstring, ISound *>();

        m_m3dSounds[a_iId][a_sName] = p;
      }
    }

    CSoundInterface::CSoundInterface(irr::io::IFileSystem *a_pFs) {
      m_vVelListener = irr::core::vector3df(0.0f, 0.0f, 0.0f);
      m_eSoundTrack  = enSoundTrack::enStNone;
      m_pSoundTrack  = nullptr;
      m_bMuteSfx     = false;

      m_fSoundtrackVolume = 1.0f;
      m_fSoundFXVolume    = 1.0f;

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
                else if (l_sAttr == L"mindistance") {
                  l_fMinDist = std::stof(l_sValue);
                }
                else if (l_sAttr == L"maxdistance") {
                  l_fMaxDist = std::stof(l_sValue);
                }
                else if (l_sAttr == L"vol") {
                  l_fVolume = std::stof(l_sValue);
                }
              }

              if (l_sName != L"") {
                printf("SFX Param: \"%ls\": %.2f, %.2f, %.2f\n", l_sName.c_str(), l_fVolume, l_fMinDist, l_fMaxDist);
                m_mSoundParameters[l_sName] = std::make_tuple(l_fVolume, l_fMinDist, l_fMaxDist);
              }
            }
          }
          l_pReader->drop();
        }
      }

      m_pDevice = new CAudioDevice();
    }

    CSoundInterface::~CSoundInterface() {
      clear3dSounds();

      for (std::map<std::wstring, ISound *>::iterator it = m_m2dSounds.begin(); it != m_m2dSounds.end(); it++) {
        delete it->second;
      }

      for (std::map<enSoundTrack, ISound*>::iterator it = m_mSoundTracks.begin(); it != m_mSoundTracks.end(); it++) {
        it->second->stop();
        delete it->second;
      }

      m_mSoundParameters.clear();
      delete m_pDevice;

      for (std::map<std::wstring, IAudioBuffer *>::iterator it = m_mAudioBuffer.begin(); it != m_mAudioBuffer.end(); it++)
        delete it->second;

      m_mAudioBuffer.clear();
    }

    void CSoundInterface::createSoundFileFactory(irr::io::IFileArchive *a_pArchive) {
    }

    void CSoundInterface::setSfxVolume(irr::f32 a_fVolume) {
      m_fSoundFXVolume = a_fVolume;
    }

    void CSoundInterface::setSoundtrackVolume(irr::f32 a_fVolume) {
      m_fSoundtrackVolume = a_fVolume;
      if (m_pSoundTrack != nullptr)
        m_pSoundTrack->setVolume(m_fSoundtrackVolume);
    }

    void CSoundInterface::muteAudio() {
      m_pDevice->mute();
    }

    void CSoundInterface::unmuteAudio() {
      m_pDevice->unmute();
    }

    void CSoundInterface::muteSoundFX(bool a_bMute) {
      for (std::map<std::wstring, ISound *>::iterator it = m_m2dSounds.begin(); it != m_m2dSounds.end(); it++)
        it->second->setVolume(0.0f);

      for (std::map<irr::s32, std::map<std::wstring, ISound*> >::iterator it = m_m3dSounds.begin(); it != m_m3dSounds.end(); it++) {
        for (std::map<std::wstring, ISound*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
          it2->second->setVolume(0.0f);
        }
      }

      m_bMuteSfx = a_bMute;
      printf("SoundFX %s\n", m_bMuteSfx ? "active" : "muted");
    }

    irr::f32 CSoundInterface::getSoundtrackVolume() {
      return m_fSoundtrackVolume;
    }

    void CSoundInterface::startSoundtrack(enSoundTrack a_eSoundTrack) {
      if (a_eSoundTrack != m_eSoundTrack) {
        if (m_pSoundTrack != nullptr)
          m_pSoundTrack->stop();

        m_eSoundTrack = a_eSoundTrack;

        if (m_mSoundTracks.find(m_eSoundTrack) != m_mSoundTracks.end())
          m_pSoundTrack = m_mSoundTracks[m_eSoundTrack];
        else
          m_pSoundTrack = nullptr;


        if (m_pSoundTrack != nullptr) {
          m_pSoundTrack->setVolume(m_fSoundtrackVolume);
          m_pSoundTrack->play();
        }
      }
    }

    void CSoundInterface::setSoundtrackFade(irr::f32 a_fValue) {
      if (m_pSoundTrack != nullptr) {
        m_pSoundTrack->setVolume(a_fValue * m_fSoundtrackVolume);
      }
    }

    void CSoundInterface::play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bLooped) {
      if (m_m3dSounds.find(a_iId) != m_m3dSounds.end() && m_m3dSounds[a_iId].find(a_sName) != m_m3dSounds[a_iId].end()) {
        ISound *p = m_m3dSounds[a_iId][a_sName];
        p->setPosition(a_vPosition);
        p->setVelocity(a_vVelocity);
        p->setVolume  (m_bMuteSfx ? 0.0f : m_fSoundFXVolume * a_fVolume);
        p->play();
      }
    }

    void CSoundInterface::play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, irr::f32 a_fVolume, bool a_bLooped) {
      if (m_m3dSounds.find(a_iId) != m_m3dSounds.end() && m_m3dSounds[a_iId].find(a_sName) != m_m3dSounds[a_iId].end()) {
        ISound *p = m_m3dSounds[a_iId][a_sName];
        p->setPosition(a_vPosition);
        p->setVelocity(m_vVelListener);
        p->setVolume  (m_bMuteSfx ? 0.0f : m_fSoundFXVolume * a_fVolume);
        p->play();
      }
    }

    void CSoundInterface::play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan) {
      if (m_m2dSounds.find(a_sName) == m_m2dSounds.end()) {
        if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
          ISound* p = new CSound2d(m_mAudioBuffer[a_sName], false);
          m_m2dSounds[a_sName] = p;
        }
      }

      if (m_m2dSounds.find(a_sName) != m_m2dSounds.end()) {
        m_m2dSounds[a_sName]->setVolume(m_bMuteSfx ? 0.0f : m_fSoundFXVolume * a_fVolume);
        m_m2dSounds[a_sName]->setPosition(irr::core::vector3df(0.0f, a_fPan, 0.0f));
        m_m2dSounds[a_sName]->play();
      }
    }

    void CSoundInterface::clear3dSounds() {
      for (std::map<irr::s32, std::map<std::wstring, ISound*> >::iterator it = m_m3dSounds.begin(); it != m_m3dSounds.end(); it++) {
        for (std::map<std::wstring, ISound*>::iterator itChild = it->second.begin(); itChild != it->second.end(); itChild++) {
          delete itChild->second;
        }
        it->second.clear();
      }
      m_m3dSounds.clear();
      m_vNoDopplerSounds.clear();
    }

    void CSoundInterface::addSoundParameter(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fMinDistance, irr::f32 a_fMaxDistance) {
      m_mSoundParameters[a_sName] = std::make_tuple(a_fVolume, a_fMinDistance, a_fMaxDistance);
    }

    void CSoundInterface::setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) {
      m_vVelListener = a_vVel;
      m_pDevice->updateListener(a_pCamera, m_vVelListener);

      for (std::vector<ISound*>::iterator it = m_vNoDopplerSounds.begin(); it != m_vNoDopplerSounds.end(); it++)
        (*it)->setVelocity(a_vVel);
    }

    void CSoundInterface::bufferDeleted(IAudioBuffer* a_pBuffer) {
      if (m_mAudioBuffer.find(a_pBuffer->getName()) != m_mAudioBuffer.end()) {
        m_mAudioBuffer.erase(a_pBuffer->getName());
      }
      delete a_pBuffer;
    }

    void CSoundInterface::assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) {
      for (std::map<enSoundTrack, std::wstring>::const_iterator it = a_mSoundTracks.begin(); it != a_mSoundTracks.end(); it++) {
        if (m_mAudioBuffer.find(it->second) != m_mAudioBuffer.end()) {
          ISound *p = new CSound2d(m_mAudioBuffer[it->second], it->second.find(L"_result") == std::string::npos);
          m_mSoundTracks[it->first] = p;
        }
      }
    }

  }
}
