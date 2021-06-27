// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaSingleton_audio.h>
#include <sound/CSoundInterface.h>
#include <LuaBridge/LuaBridge.h>
#include <sound/CSoundEnums.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaSingleton_audio::CLuaSingleton_audio(lua_State* a_pState) {
      m_pInterface = CGlobal::getInstance()->getSoundInterface();

      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSingleton_audio>("LuaAudio")
          .addFunction("setsfxvolume"       , &CLuaSingleton_audio::setSfxVolume)
          .addFunction("setsoundtrackvolume", &CLuaSingleton_audio::setSoundtrackVolume)
          .addFunction("muteaudio"          , &CLuaSingleton_audio::muteAudio)
          .addFunction("mutesfx"            , &CLuaSingleton_audio::muteSfx)
          .addFunction("startsoundtrack"    , &CLuaSingleton_audio::startSoundTrack)
          .addFunction("setsoundtrackfade"  , &CLuaSingleton_audio::setSoundtrackFade)
          .addFunction("getsoundtrackvolume", &CLuaSingleton_audio::getSoundtrackVolume)
          .addFunction("getsfxvolume"       , &CLuaSingleton_audio::getSfxVolume)
        .endClass();

      std::error_code l_cError;
      luabridge::push(a_pState, this, l_cError);
      lua_setglobal(a_pState, "audio");
    }

    CLuaSingleton_audio::~CLuaSingleton_audio() {
    }

    void CLuaSingleton_audio::setSfxVolume(float a_fVolume) {
      m_pInterface->setSfxVolume((irr::f32)a_fVolume);
    }

    void CLuaSingleton_audio::setSoundtrackVolume(float a_fVolume) {
      m_pInterface->setSoundtrackVolume((irr::f32)a_fVolume);
    }

    void CLuaSingleton_audio::muteAudio(bool a_bMute) {
      if (a_bMute)
        m_pInterface->muteAudio();
      else
        m_pInterface->unmuteAudio();
    }

    void CLuaSingleton_audio::muteSfx(bool a_bMute) {
      m_pInterface->muteSoundFX(a_bMute);
    }

    void CLuaSingleton_audio::startSoundTrack(int a_iSoundTrack) {
      m_pInterface->startSoundtrack((enSoundTrack)a_iSoundTrack);
    }

    void CLuaSingleton_audio::setSoundtrackFade(float a_fValue) {
      m_pInterface->setSoundtrackFade((irr::f32)a_fValue);
    }

    float CLuaSingleton_audio::getSoundtrackVolume() {
      return m_pInterface->getSoundtrackVolume();
    }

    float CLuaSingleton_audio::getSfxVolume() {
      return m_pInterface->getSfxVolume();
    }
  }
}