// (w) 2021 by Dustbin::Games / Christian Keimel
#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <CMainClass.h>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif


#if defined(_DEBUG) || defined(_LINUX)
int main(int argc, char *argv[]) {
#else
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *sCmdLine, int nShowCmd) {
#endif

  dustbin::CMainClass l_cMainClass;

  l_cMainClass.run();

  return 0;
}