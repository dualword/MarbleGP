// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <shaders/CDustbinShaders.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <gui/CGuiTextField.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuDetectGfx
    * @author Christian Keimel
    * This menu is used for the auto-detection of suitable GFX settings
    */
    class CMenuDetectGfx : public IMenuHandler {
      private:
        bool m_bStop;   /**< Stop the auto detection */

      public:
        CMenuDetectGfx(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), m_bStop(false) {
          m_pGui->clear();
        }

        virtual ~CMenuDetectGfx() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "btnStopAutoDetection") {
                m_bStop = true;
              }
            }
          }


          return l_bRet;
        }

        virtual bool run() override {
          CGlobal *l_pGlobal = CGlobal::getInstance();
          l_pGlobal->getSettingData().m_iShadows = 0;

          irr::ITimer *l_pTimer = m_pDevice->getTimer();

          std::vector<irr::core::recti> l_vViewports;

          irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();
          irr::core::dimension2du l_cDim = irr::core::dimension2du(l_cScreen.Width, l_cScreen.Height);

          irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(enFont::Big, l_cDim);

          l_vViewports.push_back(irr::core::recti(irr::core::position2di(0, 0), l_cDim));

          m_pSmgr->clear();
          m_pSmgr->loadScene("data/levels/four_obstacles/track.xml");

          irr::scene::ISceneNode *l_pEmpty = m_pSmgr->addEmptySceneNode();
          l_pEmpty->setPosition(irr::core::vector3df(0.0f, 35.0f, 0.0f));
          irr::scene::ICameraSceneNode *l_pCamera = m_pSmgr->addCameraSceneNode(l_pEmpty, irr::core::vector3df(550.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f));
          irr::scene::ISceneNodeAnimator *l_pAnim = m_pSmgr->createRotationAnimator(irr::core::vector3df(0.0f, 0.125f, 0.0f));
          l_pEmpty->addAnimator(l_pAnim);
          l_pAnim->drop();

          shaders::CDustbinShaders *l_pShader = CGlobal::getInstance()->getShader();
          l_pShader->addLightCamera();

          helpers::addNodeToShader(
            l_pShader, 
            m_pSmgr->getRootSceneNode(), 
            nullptr, 
            helpers::countMeshSceneNodes(m_pSmgr->getRootSceneNode())
          );

          std::wstring l_sHeadline = L"Framerate Detection";

          //          Text       Frames FPS         Size                 Position               Position FPS
          std::tuple<std::wstring, int, int, irr::core::dimension2du, irr::core::vector2di, irr::core::vector2di> l_aData[] = {
            std::make_tuple(L"Shadows Off: ", 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L"Static Low: " , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L"Static High: ", 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L"Low: "        , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L"Medium: "     , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L"High: "       , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di()),
            std::make_tuple(L""             , 0, 0, irr::core::dimension2du(), irr::core::vector2di(), irr::core::vector2di())
          };

          irr::core::dimension2du l_cHead = l_pFont->getDimension(l_sHeadline.c_str());
          irr::core::dimension2du l_cText;

          irr::core::vector2di l_cPos = irr::core::vector2di(0, 5 * l_cHead.Height / 2);

          for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
            std::get<3>(l_aData[i]) = l_pFont->getDimension(std::get<0>(l_aData[i]).c_str());
            std::get<4>(l_aData[i]) = irr::core::vector2di(0, l_cPos.Y);

            if (std::get<3>(l_aData[i]).Width  > l_cText.Width ) l_cText .Width  = std::get<3>(l_aData[i]).Width;
            if (std::get<3>(l_aData[i]).Height > l_cText.Height) l_cText .Height = std::get<3>(l_aData[i]).Height;

            l_cPos.Y += 5 * l_cHead.Height / 4;
          }

          for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
            std::get<4>(l_aData[i]).X = l_cScreen.Width / 2 - std::get<3>(l_aData[i]).Width;
          }

          l_cPos.Y += l_cHead.Height / 2;

          irr::core::dimension2du l_cBoxSize = irr::core::dimension2du(2 * l_cText.Width, l_cPos.Y);

          irr::core::dimension2du l_cBtnSize = l_pFont->getDimension(L"Stop Auto Detection");
          l_cBtnSize.Width  = 5 * l_cBtnSize.Width  / 4;
          l_cBtnSize.Height = 5 * l_cBtnSize.Height / 4;

          gui::CMenuButton *l_pBtn = new gui::CMenuButton(m_pGui->getRootGUIElement());
          l_pBtn->setText(L"Stop Auto Detection");
          l_pBtn->setRelativePosition(irr::core::recti(
            irr::core::vector2di(l_cScreen.Width / 2 - l_cBtnSize.Width / 2, l_cScreen.Height / 2 + l_cBoxSize.Height / 2 + l_cBtnSize.Height), 
            l_cBtnSize
          ));
          l_pBtn->setName("btnStopAutoDetection");
          l_pBtn->setToolTipText(L"Cancel auto detection of the graphics settings.");

          irr::core::recti l_cBox = irr::core::recti(
            irr::core::vector2di(l_cScreen.Width / 2 - l_cBoxSize.Width / 2 - l_cHead.Height, l_cScreen.Height / 2 - l_cBoxSize.Height / 2),
            l_cBoxSize + irr::core::dimension2du(2 * l_cHead.Height, 0)
          );

          for (int i = 0; std::get<0>(l_aData[i]) != L""; i++) {
            std::get<4>(l_aData[i]).Y += l_cBox.UpperLeftCorner.Y;

            std::get<5>(l_aData[i]) = irr::core::vector2di(l_cScreen.Width / 2, std::get<4>(l_aData[i]).Y);
          }

          irr::core::dimension2du l_cFps = irr::core::dimension2du(l_cBox.getWidth() / 2, l_cHead.Height);

          irr::u32 l_iFlags = 0;
          for (int i = 0; i < 7; i++) {
            if (i < 6) 
              l_iFlags = helpers::convertForShader(i, l_pShader);

            irr::u32 l_iStart = l_pTimer->getRealTime() + 1000;
            irr::u32 l_iFrame = 0;

            bool l_bCount = false;

            l_pShader->clearShadowMaps();
            l_pShader->startShadowMaps();
            l_pShader->renderShadowMap((irr::u32)shaders::enShadowMap::TranspColor | (irr::u32)shaders::enShadowMap::Transparent | (irr::u32)shaders::enShadowMap::Solid);
            l_pShader->endShadowMaps();

            while (m_pDevice->run() && !m_bStop) {
              if (l_iFlags != 0) {
                l_pShader->startShadowMaps();
                l_pShader->renderShadowMap(l_iFlags);
                l_pShader->endShadowMaps();
              }

              m_pDrv->beginScene(true, true);
              l_pShader->renderScene();
              m_pGui->drawAll();

              m_pDrv->draw2DRectangle(irr::video::SColor(192, 192, 192, 192), l_cBox);
              l_pFont->draw(l_sHeadline.c_str(), 
                irr::core::recti(
                  l_cBox.UpperLeftCorner + irr::core::vector2di(0, l_cHead.Height / 2), 
                  irr::core::dimension2du(l_cBox.getWidth(), l_cHead.Height)
                ),
                irr::video::SColor(0xFF, 0, 0, 0),
                true, 
                true
              );

              for (int j = 0; std::get<0>(l_aData[j]) != L""; j++) {
                l_pFont->draw(
                  std::get<0>(l_aData[j]).c_str(), 
                  irr::core::recti(
                    std::get<4>(l_aData[j]), 
                    std::get<3>(l_aData[j])
                  ),
                  i == j ? irr::video::SColor(0xFF, 0x80, 0x40, 0x00) : irr::video::SColor(0xFF, 0, 0, 0),
                  false,
                  true
                );

                if (j == i) {
                  if (!l_bCount) {
                    l_pFont->draw(L"Framecount", irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0x80, 0x40, 0x00), false, true);
                  }
                  else {
                    l_pFont->draw(std::to_wstring(l_iFrame).c_str(), irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0x80, 0x40, 0x00), false, true);
                  }
                }
                else if (j < i) {
                  l_pFont->draw((std::to_wstring(std::get<2>(l_aData[j])) + L" FPS").c_str(), irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0, 0, 0), false, true);
                }
                else {
                  l_pFont->draw(L"---", irr::core::recti(std::get<5>(l_aData[j]), l_cFps), irr::video::SColor(0xFF, 0, 0, 0), false, true);
                }
              }

              m_pDrv->endScene();
              std::wstring s = L"Current Framerate: " + std::to_wstring(m_pDrv->getFPS()) + L" FPS";
              m_pDevice->setWindowCaption(s.c_str());

              if (!l_bCount) {
                if (l_pTimer->getRealTime() > l_iStart) {
                  printf("Start Frame Count...\n");
                  l_bCount = true;
                }
              }
              else {
                l_iFrame++;
                if (l_pTimer->getRealTime() > l_iStart + 4000) {
#ifdef _WINDOWS
                  if (l_iFrame / 4 > 100 && i != 6)
#else
                  if (l_iFrame / 4 > 30 && i != 6)
#endif
                    l_pGlobal->getSettingData().m_iShadows = i;
                  else 
                    i = 6;

                  std::get<2>(l_aData[i]) = l_iFrame / 4;
                  break;
                }
              }
            }
          }
          m_pSmgr->clear();
          l_pShader->clear();
          printf("Shadow settings: %i\n", l_pGlobal->getSettingData().m_iShadows);

          createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);

          return true;
        }
    };

    IMenuHandler *createMenuDetectGfx(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuDetectGfx(a_pDevice, a_pManager, a_pState);
    }
  }
}