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

#include <irrlicht.h>
#include <vector>
#include <malloc.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

/**
* Our saved state data.
*/
struct saved_state {
  float angle;
  int32_t x;
  int32_t y;
};

class MyEventReceiver : public irr::IEventReceiver {
private:
  std::vector<irr::video::SColor> m_vColors;
  std::vector<irr::video::SColor>::iterator m_itColors;
  bool m_bSwitch;

public:
  MyEventReceiver() {
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0, 0xFF, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0, 0, 0xFF));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0xFF, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0, 0xFF));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
    m_vColors.push_back(irr::video::SColor(0xFF, 0x80, 0x80, 0x80));
    m_vColors.push_back(irr::video::SColor(0xFF, 0, 0, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0, 0xFF));
    m_vColors.push_back(irr::video::SColor(0xFF, 0x80, 0, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0, 0x80, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0, 0, 0x80));
    m_vColors.push_back(irr::video::SColor(0xFF, 0x80, 0x80, 0));
    m_vColors.push_back(irr::video::SColor(0xFF, 0xFF, 0x80, 0x80));
    m_vColors.push_back(irr::video::SColor(0xFF, 0x80, 0x80, 0x80));

    m_itColors = m_vColors.begin();
    m_bSwitch = true;
  }

  irr::video::SColor& getColor() {
    m_bSwitch = true;
    return *m_itColors;
  }

  virtual bool OnEvent(const irr::SEvent& a_cEvent) {
    if (m_bSwitch) {
      m_itColors++;

      if (m_itColors == m_vColors.end())
        m_itColors = m_vColors.begin();

      m_bSwitch = false;
    }

    return false;
  }
};

/**
* Shared state for our app.
*/
struct engine {
  struct android_app* app;

  irr::IrrlichtDevice *m_pDevice;
  irr::video::IVideoDriver *m_pDrv;
  irr::scene::ISceneManager *m_pSmgr;
  irr::gui::IGUIEnvironment *m_pGui;
  irr::scene::ISceneNode *m_pNode;
  irr::gui::IGUITab *m_pTab;

  MyEventReceiver *m_pReceiver;

  int animating;
  int32_t width;
  int32_t height;
  struct saved_state state;
};

void portable_setupDevice(irr::IrrlichtDevice* a_pDevice) {

}

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct engine* engine) {
  engine->m_pReceiver = new MyEventReceiver();

  irr::SIrrlichtCreationParameters l_cParams;
  l_cParams.DriverType = irr::video::EDT_OGLES1;				// android:glEsVersion in AndroidManifest.xml should be "0x00010000" (requesting 0x00020000 will also guarantee that ES1 works)
                                                        // l_cParams.DriverType = irr::video::EDT_OGLES2;				// android:glEsVersion in AndroidManifest.xml should be "0x00020000"
  l_cParams.WindowSize = irr::core::dimension2d<irr::u32>(0, 0);	// using 0,0 it will automatically set it to the maximal size
  l_cParams.PrivateData = engine->app;
  l_cParams.Bits = 24;
  l_cParams.ZBufferBits = 32;
  l_cParams.AntiAlias  = 0;
  l_cParams.EventReceiver = engine->m_pReceiver;

  engine->m_pDevice = irr::createDeviceEx(l_cParams);
  engine->m_pDrv = engine->m_pDevice->getVideoDriver();
  engine->m_pSmgr = engine->m_pDevice->getSceneManager();
  engine->m_pDevice->getFileSystem()->addFileArchive("raw/Data.zip");
  engine->m_pGui = engine->m_pDevice->getGUIEnvironment();

  engine->m_pTab = engine->m_pGui->addTab(irr::core::recti(0, 0, 500, 500));
  engine->m_pTab->setDrawBackground(true);
  engine->m_pTab->setBackgroundColor(engine->m_pReceiver->getColor());

  engine->m_pGui->addButton(irr::core::recti(0, 500, 500, 750), nullptr, -1, L"Hello World");

  irr::io::IFileSystem *l_pFs = engine->m_pDevice->getFileSystem();

  irr::core::stringc mediaPath = "media/";

  for (irr::u32 i = 0; i < l_pFs->getFileArchiveCount(); ++i ) {
    irr::io::IFileArchive* archive = l_pFs->getFileArchive(i);
    if ( archive->getType() == irr::io::EFAT_ANDROID_ASSET )
    {
      archive->addDirectoryToFileList(mediaPath);

      const irr::io::IFileList *l_pList = archive->getFileList();

      for (irr::u32 j = 0; j < l_pList->getFileCount(); j++) {
        irr::io::path l_sPath = l_pList->getFileName(j);
        bool b = l_pList->isDirectory(j);
        printf("--> %s (%s)\n", l_sPath.c_str(), b ? "folder" : "file");
      }
    }
  }


  if (!l_pFs->existFile(mediaPath + "media/data.zip")) {
    printf("Data file not found.\n");
  }
  else {
    l_pFs->addFileArchive(mediaPath + "media/data.zip");
  }

  if (!l_pFs->existFile("data.zip")) {
    printf("Data file not found.\n");
  }
  else {
    l_pFs->addFileArchive("data.zip");
  }

  if (!l_pFs->existFile(mediaPath + "media/raw/data.zip")) {
    printf("Data file not found.\n");
  }
  else {
    l_pFs->addFileArchive(mediaPath + "media/raw/data.zip");
  }

  if (!l_pFs->existFile(mediaPath + "media/res/raw/data.zip")) {
    printf("Data file not found.\n");
  }
  else {
    l_pFs->addFileArchive(mediaPath + "media/res/raw/data.zip");
  }

  portable_setupDevice(engine->m_pDevice);

  if (!engine->m_pSmgr->loadScene("media/startup.xml")) {
    printf("Could not load scene.\n");
  }

  engine->m_pNode = engine->m_pSmgr->getSceneNodeFromName("headline");

  engine->m_pSmgr->setActiveCamera(engine->m_pSmgr->addCameraSceneNode(nullptr, irr::core::vector3df(0.0f, 2.5f, -15.0f), irr::core::vector3df(0.0f)));

  return 0;
}

/**
* Just the current frame in the display.
*/

static void engine_draw_frame(struct engine* engine) {
  // if (engine->display == NULL) {
  // No display.
  // 	return;
  // }
  // if (engine->m_pDevice != nullptr) {
  if (engine->m_pDevice->run()) {
    engine->m_pDrv->beginScene(true, true, irr::video::SColor(255, 128, 128, 255));
    engine->m_pSmgr->drawAll();
    engine->m_pGui->drawAll();
    engine->m_pDrv->endScene();
  }
}


/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
  struct engine engine;

  memset(&engine, 0, sizeof(engine));
  state->userData = &engine;
  engine.app = state;
  engine_init_display(&engine);


  if (state->savedState != NULL) {
    // We are starting with a previous saved state; restore from it.
    engine.state = *(struct saved_state*)state->savedState;
  }

  engine.animating = 1;

  // loop waiting for stuff to do.

  irr::f32 l_fRot = 0.0f;

  while (1) {
    engine_draw_frame(&engine);

    engine.m_pTab->setBackgroundColor(engine.m_pReceiver->getColor());

    if (engine.m_pNode != nullptr) {
      l_fRot += 1.0f;
      engine.m_pNode->setRotation(irr::core::vector3df(0.0f, l_fRot, 0.0f));
    }
  }
}
