/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <CMainClass.h>
#include <thread>
#include <chrono>
#include <string>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))


/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* a_pState) {
  dustbin::enAppState l_eState = dustbin::enAppState::Continue;

  do {
    dustbin::CMainClass l_cMainClass(a_pState);
    std::chrono::steady_clock::time_point l_cNextStep = std::chrono::steady_clock::now();
    do {
      l_eState = l_cMainClass.run();
      l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(10);
      std::this_thread::sleep_until(l_cNextStep);
    }
    while (l_eState == dustbin::enAppState::Continue);
  } 
  while (l_eState == dustbin::enAppState::Restart);
}
