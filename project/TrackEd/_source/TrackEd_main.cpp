// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <irrlicht.h>

#include <CTrackEd_MainClass.h>

#if defined(_DEBUG) || defined(_LINUX)
int main(int argc, char* argv[]) {
#else
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
#endif

  dustbin::tracked::CTrackEd_MainClass l_cMainClass;

  while (l_cMainClass.run());

  return 0;
}