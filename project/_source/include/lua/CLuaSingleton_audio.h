// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <lua/ILuaObject.h>
#include <lua.hpp>

namespace dustbin {

  namespace audio {
    class CSoundInterface;
  }

  namespace lua {
    /**
    * @class CLuaSingleton_audio
    * @author Christian Keimel
    * This class provides access to the system sounds
    */
    class CLuaSingleton_audio : public ILuaObject {
      private:
        audio::CSoundInterface *m_pInterface;

      public:
        CLuaSingleton_audio(lua_State *a_pState);
        virtual ~CLuaSingleton_audio();

        void setMasterVolume(float a_fVolume);
        void setSfxVolumeGame(float a_fVolume);
        void setSfxVolumeMenu(float a_fVolume);
        void setSoundtrackVolume(float a_fVolume);
        void muteAudio(bool a_bMute);
        void muteSfx(bool a_bMute);

        void startSoundTrack(int a_iSoundTrack);
        void setSoundtrackFade(float a_fValue);

        float getSoundtrackVolume();
        float getSfxVolume();
    };
  }
}
