#ifdef _RASPBERRY

#include <irrlicht.h>
#include <state/IState.h>
#include <thread>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <CMainClass.h>

int main(int argc, char *argv[]) {
  dustbin::CMainClass *l_pMainClass = nullptr;
  dustbin::state::enState l_eState;

  do {
    std::string l_sSettings = "";

    struct passwd *pw = getpwuid(getuid());
    std::string l_sPath = pw->pw_dir;

    l_sPath += "/.DustbinGames/MarbleGP";

    std::string l_sCmd = std::string("mkdir -p ") + l_sPath;
    system(l_sCmd.c_str());

    l_sPath += "/MarbleGP_Setup.xml";

    std::ifstream l_cFile(l_sPath.c_str());
    if (l_cFile.is_open()) {
      l_sSettings = std::string(std::istreambuf_iterator<char>(l_cFile), std::istreambuf_iterator<char>());
      l_cFile.close();
    }

    irr::IrrlichtDevice *l_pNull = irr::createDevice(irr::video::EDT_NULL);
    irr::core::dimension2du l_cDim = l_pNull->getVideoModeList()->getDesktopResolution();
    l_pNull->drop();

    l_cDim.Width /= 2;
    l_cDim.Height /= 2;

    printf("Screen Size: %i, %i\n", l_cDim.Width, l_cDim.Height);

    l_pMainClass = new dustbin::CMainClass(l_sSettings);

    irr::SIrrlichtCreationParameters l_cParams;
    l_cParams.DriverType       = irr::video::EDT_OGLES2;
    l_cParams.Fullscreen       = true;
    l_cParams.WindowSize       = l_cDim;
    l_cParams.Bits             = 24;
    l_cParams.ZBufferBits      = 32;
    l_cParams.Vsync            = true;
    l_cParams.OGLES2ShaderPath = "shaders_es/";
    l_cParams.EventReceiver    = l_pMainClass;

    irr::IrrlichtDevice *l_pDevice = irr::createDeviceEx(l_cParams);
    
    l_pMainClass->setIrrlichtDevice(l_pDevice);

    do {
      l_eState = l_pMainClass->run();
    }
    while (l_eState != dustbin::state::enState::Restart && l_eState != dustbin::state::enState::Quit);

    l_pDevice->closeDevice();
    l_pDevice->run();
    l_pDevice->drop();

    {
      std::ofstream l_cFile(l_sPath);
      l_cFile << l_pMainClass->getSettings();
      l_cFile.close();
    }

    delete l_pMainClass;
    l_pMainClass = nullptr;
  }
  while (l_eState != dustbin::state::enState::Quit);

  return 0;
}

#endif