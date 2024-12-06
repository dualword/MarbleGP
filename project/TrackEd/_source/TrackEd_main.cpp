// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <irrlicht.h>

#include <CTrackEd_MainClass.h>

#include <chrono>
#include <thread>

#if defined(_DEBUG) || defined(_LINUX)
int main(int argc, char* argv[]) {
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#endif

  dustbin::tracked::CTrackEd_MainClass l_cMainClass;

  // On Windows we limit the frame rate to 125 FPS as the simulation runs at 60 FPS
  std::chrono::steady_clock::time_point l_cNextStep = std::chrono::steady_clock::now();

  while (l_cMainClass.run()) {
    l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(16);
    std::this_thread::sleep_until(l_cNextStep);
  }

  return 0;
}