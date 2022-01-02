// (w) 2021 by Dustbin::Games / Christian Keimel
#ifdef _DEBUG
#include <vld.h>
#endif

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <CMainClass.h>
#include <thread>
#include <chrono>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif


#if defined(_DEBUG) || defined(_LINUX)
int main(int argc, char *argv[]) {
#else
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *sCmdLine, int nShowCmd) {
#endif
  dustbin::enAppState l_eState = dustbin::enAppState::Continue;

  do {
    dustbin::CMainClass *l_pMainClass = new dustbin::CMainClass();
    std::chrono::steady_clock::time_point l_cNextStep = std::chrono::steady_clock::now();
    do {
      l_eState = l_pMainClass->run();
      l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(10);
      std::this_thread::sleep_until(l_cNextStep);
    }
    while (l_eState == dustbin::enAppState::Continue);

    delete l_pMainClass;
    l_pMainClass = nullptr;
  } 
  while (l_eState == dustbin::enAppState::Restart);

  return 0;
}