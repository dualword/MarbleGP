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
    class CSoundInterface : public IAudioBuffer::IDeletionListener, public ISoundInterface {
      private:
        irr::f32              m_fSoundtrackVolume,
                              m_fSoundFXVolumeMenu,
                              m_fSoundFXVolumeGame,
                              m_fMasterVolume;
        enSoundTrack          m_eSoundTrack;
        irr::core::vector3df  m_vVelListener;
        CAudioDevice         *m_pDevice;
        ISound               *m_pSoundTrack;
        bool                  m_bMuteSfx,
                              m_bMenu;    /**< Are we currently in the menu or game? */


        /*
        Sound Parameters.
        Tuple Members:
        0: Volume
        1: Min Distance
        2: Max Distance
        */
        std::map<std::wstring, irr::f32      > m_mSoundParameters;
        std::map<std::wstring, IAudioBuffer *> m_mAudioBuffer;

        std::vector<ISound *> m_vNoDopplerSounds;

        std::map                   <std::wstring, ISound *>   m_m2dSounds;
        std::map<irr::s32, std::map<std::wstring, ISound *> > m_m3dSounds;
        std::map<enSoundTrack                   , ISound *>   m_mSoundTracks;

      public:
        CSoundInterface(irr::io::IFileSystem *a_pFs) : m_bMenu(true) {
          m_vVelListener = irr::core::vector3df(0.0f, 0.0f, 0.0f);
          m_eSoundTrack  = enSoundTrack::enStNone;
          m_pSoundTrack  = nullptr;
          m_bMuteSfx     = false;

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

        ~CSoundInterface() {
          clear3dSounds();

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

        void preloadSound(const std::wstring& a_sName, bool a_bMenuSound) {
          if (m_mAudioBuffer.find(a_sName) == m_mAudioBuffer.end()) {
            IAudioBuffer *p = new CAudioBufferOggVorbis(a_sName);
            p->setDeletionListener(this);
            m_mAudioBuffer[a_sName] = p;
          }
        }

        void assignSound(const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, bool a_bDoppler) {
          if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
            irr::f32 l_fVolume  = 0.6f;

            std::wstring l_sName = a_sName.substr(0, a_sName.size() - 4);
            l_sName += L".ogg";

            if (m_mSoundParameters.find(l_sName) != m_mSoundParameters.end()) {
              l_fVolume  = m_mSoundParameters[l_sName];
            }

            ISound* p = new CSound3d(m_mAudioBuffer[a_sName], a_bLoop, l_fVolume, 25, 150);
      
            if (m_m3dSounds.find(a_iId) == m_m3dSounds.end())
              m_m3dSounds[a_iId] = std::map<std::wstring, ISound *>();

            m_m3dSounds[a_iId][a_sName] = p;

            if (!a_bDoppler)
              m_vNoDopplerSounds.push_back(p);
          }
        }

        void assignFixed (const std::wstring& a_sName, irr::s32 a_iId, bool a_bLoop, const irr::core::vector3df &a_vPos) {
          if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
            printf("Assign fixed \"%ls\" to %i\n", a_sName.c_str(), a_iId);

            ISound* p = new CSound3dFixed(m_mAudioBuffer[a_sName], a_bLoop, 1.0f, 10.0f, 100.0f, a_vPos);

            if (m_m3dSounds.find(a_iId) == m_m3dSounds.end())
              m_m3dSounds[a_iId] = std::map<std::wstring, ISound *>();

            m_m3dSounds[a_iId][a_sName] = p;
          }
        }

        void setMenuFlag(bool a_bMenu) {
          m_bMenu = a_bMenu;
        }

        void createSoundFileFactory(irr::io::IFileArchive *a_pArchive) {
        }

        void setMasterVolume(irr::f32 a_fVolume) {
          m_fMasterVolume = a_fVolume;
          if (m_pSoundTrack != nullptr)
            m_pSoundTrack->setVolume(0.5f * m_fMasterVolume * m_fSoundtrackVolume);
        }

        void setSfxVolumeGame(irr::f32 a_fVolume) {
          m_fSoundFXVolumeGame = a_fVolume;
        }

        void setSfxVolumeMenu(irr::f32 a_fVolume) {
          m_fSoundFXVolumeMenu = a_fVolume;
        }

        void setSoundtrackVolume(irr::f32 a_fVolume) {
          m_fSoundtrackVolume = a_fVolume;
          if (m_pSoundTrack != nullptr)
            m_pSoundTrack->setVolume(0.5f * m_fMasterVolume * m_fSoundtrackVolume);
        }

        void muteAudio() {
          m_pDevice->mute();
        }

        void unmuteAudio() {
          m_pDevice->unmute();
        }

        void muteSoundFX(bool a_bMute) {
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

        irr::f32 getSoundtrackVolume() {
          return m_fSoundtrackVolume;
        }

        irr::f32 getMasterVolume() {
          return m_fMasterVolume;
        }

        irr::f32 getSfxVolumeGame() {
          return m_fSoundFXVolumeGame;
        }

        irr::f32 getSfxVolumeMenu() {
          return m_fSoundFXVolumeMenu;
        }

        void startSoundtrack(enSoundTrack a_eSoundTrack) {
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

        void setSoundtrackFade(irr::f32 a_fValue) {
          if (m_pSoundTrack != nullptr) {
            m_pSoundTrack->setVolume((a_fValue > 0.0f ? a_fValue : 0.0f) * 0.5f * m_fMasterVolume * m_fSoundtrackVolume);
          }
        }

        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, const irr::core::vector3df &a_vVelocity, irr::f32 a_fVolume, bool a_bLooped) {
          if (m_m3dSounds.find(a_iId) != m_m3dSounds.end() && m_m3dSounds[a_iId].find(a_sName) != m_m3dSounds[a_iId].end()) {
            ISound *p = m_m3dSounds[a_iId][a_sName];
            p->setPosition(a_vPosition);
            p->setVelocity(a_vVelocity);
            p->setVolume  (m_bMuteSfx ? 0.0f : m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume);
            p->play();
          }
          else printf("Sound not found.\n");
        }

        void play3d(irr::s32 a_iId, const std::wstring &a_sName, const irr::core::vector3df &a_vPosition, irr::f32 a_fVolume, bool a_bLooped) {
          if (m_m3dSounds.find(a_iId) != m_m3dSounds.end() && m_m3dSounds[a_iId].find(a_sName) != m_m3dSounds[a_iId].end()) {
            ISound *p = m_m3dSounds[a_iId][a_sName];
            p->setPosition(a_vPosition);
            p->setVelocity(m_vVelListener);
            p->setVolume  (m_bMuteSfx ? 0.0f : m_fMasterVolume * m_fSoundFXVolumeGame * a_fVolume);
            p->play();
          }
          else printf("Sound not found.\n");
        }

        void play2d(const std::wstring &a_sName, irr::f32 a_fVolume, irr::f32 a_fPan) {
          if (m_m2dSounds.find(a_sName) == m_m2dSounds.end()) {
            if (m_mAudioBuffer.find(a_sName) != m_mAudioBuffer.end()) {
              ISound* p = new CSound2d(m_mAudioBuffer[a_sName], false);
              m_m2dSounds[a_sName] = p;
            }
          }

          if (m_m2dSounds.find(a_sName) != m_m2dSounds.end()) {
            m_m2dSounds[a_sName]->setVolume(m_bMuteSfx ? 0.0f : m_fMasterVolume * (m_bMenu ? m_fSoundFXVolumeMenu : m_fSoundFXVolumeGame) * a_fVolume);
            m_m2dSounds[a_sName]->setPosition(irr::core::vector3df(0.0f, a_fPan, 0.0f));
            m_m2dSounds[a_sName]->play();
          }
        }

        void clear3dSounds() {
          for (std::map<irr::s32, std::map<std::wstring, ISound*> >::iterator it = m_m3dSounds.begin(); it != m_m3dSounds.end(); it++) {
            for (std::map<std::wstring, ISound*>::iterator itChild = it->second.begin(); itChild != it->second.end(); itChild++) {
              delete itChild->second;
            }
            it->second.clear();
          }
          m_m3dSounds.clear();
          m_vNoDopplerSounds.clear();
        }

        void setListenerPosition(irr::scene::ICameraSceneNode *a_pCamera, const irr::core::vector3df &a_vVel) {
          m_vVelListener = a_vVel;
          m_pDevice->updateListener(a_pCamera, m_vVelListener);

          for (std::vector<ISound*>::iterator it = m_vNoDopplerSounds.begin(); it != m_vNoDopplerSounds.end(); it++)
            (*it)->setVelocity(a_vVel);
        }

        void bufferDeleted(IAudioBuffer* a_pBuffer) {
          if (m_mAudioBuffer.find(a_pBuffer->getName()) != m_mAudioBuffer.end()) {
            m_mAudioBuffer.erase(a_pBuffer->getName());
          }
          delete a_pBuffer;
        }

        void assignSoundtracks(const std::map<enSoundTrack, std::wstring>& a_mSoundTracks) {
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
