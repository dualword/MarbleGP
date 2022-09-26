// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#ifdef _DEBUG
// #include <vld.h>
#endif

#include <helpers/CStringHelpers.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <filesystem>
#include <irrlicht.h>
#include <Windows.h>
#include <shlwapi.h>
#include "shlobj.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <streambuf>

#include <CMainClass.h>

#if defined(_DEBUG) || defined(_LINUX)
int main(int argc, char *argv[]) {
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#endif

  dustbin::helpers::clearDebugLog();

  dustbin::CMainClass *l_pMainClass = nullptr;

  dustbin::state::enState l_eState = dustbin::state::enState::None;

  do {
    TCHAR l_sPath       [MAX_PATH],
          l_sDataPath   [MAX_PATH],
          l_sTexturePath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, l_sPath))) {
      PathAppend(l_sPath, L"\\DustbinGames\\");
      if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);

#ifdef _DEBUG
      PathAppend(l_sPath, L"\\MarbleGP_debug\\");
#else
      PathAppend(l_sPath, L"\\MarbleGP\\");
#endif
      if (!PathFileExists(l_sPath)) CreateDirectory(l_sPath, NULL);
      PathAppend(l_sPath, L"MarbleGP_Setup.xml");
    }

    std::string l_sSettings = "";

    if (std::filesystem::exists(l_sPath)) {
      std::ifstream l_cFile(l_sPath);
      l_sSettings = std::string(std::istreambuf_iterator<char>(l_cFile), std::istreambuf_iterator<char>());
      l_cFile.close();
    }

    l_pMainClass = new dustbin::CMainClass(l_sSettings);

    #define INFO_BUFFER_SIZE 32767

    TCHAR  l_aName[INFO_BUFFER_SIZE];
    DWORD  l_iName = INFO_BUFFER_SIZE;

    // Get and display the name of the computer.
    if (GetComputerName(l_aName, &l_iName))
      l_pMainClass->setDeviceName(dustbin::helpers::ws2s(l_aName));

    irr::u32 l_iWidth  = 1920,
             l_iHeight = 1080;
    bool     l_bFullScreen = false;

    if (l_sSettings != "") {
      l_iWidth  = l_pMainClass->getSettingData().m_iResolutionW;
      l_iHeight = l_pMainClass->getSettingData().m_iResolutionH;

      l_bFullScreen = l_pMainClass->getSettingData().m_bFullscreen;
    }

    irr::IrrlichtDevice *l_pDevice = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2du(l_iWidth, l_iHeight), 32U, l_bFullScreen);
    l_pDevice->setEventReceiver(l_pMainClass);
    l_pMainClass->setIrrlichtDevice(l_pDevice);


    // On Windows we limit the frame rate to 125 FPS as the simulation runs at 120 FPS
    std::chrono::steady_clock::time_point l_cNextStep = std::chrono::steady_clock::now();

    do {
      l_eState = l_pMainClass->run();

      l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(8);
      std::this_thread::sleep_until(l_cNextStep);
    }
    while (l_eState != dustbin::state::enState::Restart && l_eState != dustbin::state::enState::Quit);

    {
      std::ofstream l_cFile(l_sPath);
      l_cFile << l_pMainClass->getSettings();
      l_cFile.close();
    }

    delete l_pMainClass;
    l_pMainClass = nullptr;

    l_pDevice->closeDevice();
    l_pDevice->run();
    l_pDevice->drop();
  }
  while (l_eState != dustbin::state::enState::Quit);

  dustbin::helpers::clearDebugLog();
  return 0;
}
