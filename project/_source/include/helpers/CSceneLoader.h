// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace gui {
    class IProgressCallback;
  }

  namespace helpers {
    /**
    * This function loads a scene to a given scene manager reporting loading progress to a progress callback
    * @param a_sFileName name of the file to load
    * @param a_pSmgr the scene manager to load the scene to
    * @param a_pFs the Irrlicht file system
    * @param a_pLogger the Irrlicht logger
    * @param a_pProgress the progress callback to call
    * @return "true" if the scene was loaded successfully, "false" otherwise
    */
    bool loadScene(const std::string &a_sFileName, irr::scene::ISceneManager *a_pSmgr, irr::io::IFileSystem *a_pFs, irr::ILogger *a_pLogger, gui::IProgressCallback *a_pProgress);
  }
}
