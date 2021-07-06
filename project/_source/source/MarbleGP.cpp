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

  bool l_bRestart = false;

  dustbin::enAppState l_eState = dustbin::enAppState::Continue;

  do {
    dustbin::CMainClass l_cMainClass;
    std::chrono::steady_clock::time_point l_cNextStep = std::chrono::high_resolution_clock::now();
    do {
      l_eState = l_cMainClass.run();
      l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(10);
      std::this_thread::sleep_until(l_cNextStep);
    }
    while (l_eState == dustbin::enAppState::Continue);
  } 
  while (l_eState == dustbin::enAppState::Restart);

  return 0;
}