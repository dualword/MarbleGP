/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#include <scenenodes/CSkyBoxFix.h>

namespace dustbin {
  namespace scenenodes {
    CSkyBoxFix::CSkyBoxFix(irr::video::IVideoDriver *a_pDrv, irr::scene::ISceneManager *a_pSmgr, irr::io::IFileSystem *a_pFs, const std::string &l_sIrrFile) {
      m_bSkyBoxCreated = false;
      m_pSkyBox        = nullptr;

      irr::io::IXMLReaderUTF8 *l_pXml = a_pFs->createXMLReaderUTF8(l_sIrrFile.c_str());

      enSkyboxState l_eState = enSbNoSkyBox;

      irr::core::array<irr::video::ITexture *> l_aTextures;

      irr::core::array<irr::core::stringc> l_aSkyboxTextures;

      if (l_pXml != nullptr) {
        while (l_pXml->read()) {
          irr::core::stringc l_sName = l_pXml->getNodeName();

          if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            switch (l_eState) {
              case enSbNoSkyBox: {
                if (l_sName == "node") {
                  for (irr::u32 i = 0; i < l_pXml->getAttributeCount(); i++) {
                    irr::core::stringc l_sAttrib = l_pXml->getAttributeName(i);
                    if (l_sAttrib == "type") {
                      irr::core::stringc l_sValue = l_pXml->getAttributeValue(i);
                      if (l_sValue == "skyBox") {
                        l_eState = enSbSkyBox;
                      }
                    }
                  }
                }
                break;
              }

              case enSbSkyBox: {
                if (l_sName == "materials") {
                  l_eState = enSbMaterial;
                }
                break;
              }

              case enSbMaterial: {
                if (l_sName == "attributes") {
                  l_eState = enSbAttributes;
                }
                break;
              }

              case enSbAttributes: {
                if (l_sName == "texture") {
                  for (irr::u32 i = 0; i < l_pXml->getAttributeCount(); i++) {
                    irr::core::stringc l_sAttr = l_pXml->getAttributeName(i);
                    if (l_sAttr == "value") {
                      irr::core::stringc l_sValue = l_pXml->getAttributeValue(i);
                      if (l_sValue != "") {
                        l_aSkyboxTextures.push_back(l_sValue);
                      }
                    }
                  }
                }
                break;
              }

              default:
                break;
            }
          }
          else if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            switch (l_eState) {
              case enSbAttributes: {
                if (l_sName == "attributes") {
                  l_eState = enSbMaterial;
                }
                break;
              }

              case enSbMaterial: {
                if (l_sName == "materials") {
                  l_eState = enSbSkyBox;
                }
                break;
              }

              case enSbSkyBox: {
                if (l_sName == "node") {
                  l_eState = enSbNoSkyBox;
                }
                break;
              }

              default:
                break;
            }
          }
        }
        l_pXml->drop();
      }

      if (l_aSkyboxTextures.size() == 6) {
        a_pDrv->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);

        m_bSkyBoxCreated = true;

        m_pSkyBox = a_pSmgr->addSkyBoxSceneNode(
          a_pDrv->getTexture(l_aSkyboxTextures[4].c_str()),
          a_pDrv->getTexture(l_aSkyboxTextures[5].c_str()),
          a_pDrv->getTexture(l_aSkyboxTextures[1].c_str()),
          a_pDrv->getTexture(l_aSkyboxTextures[3].c_str()),
          a_pDrv->getTexture(l_aSkyboxTextures[0].c_str()),
          a_pDrv->getTexture(l_aSkyboxTextures[2].c_str()),
          nullptr, -46
        );

        a_pDrv->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);
      }
    }

    void CSkyBoxFix::hideOriginalSkybox(irr::scene::ISceneNode *a_pInput) {
      if (!m_bSkyBoxCreated) return;

      if (a_pInput->getType() == irr::scene::ESNT_SKY_BOX && a_pInput->getID() != -46) {
        a_pInput->setVisible(false);
        a_pInput->getSceneManager()->addToDeletionQueue(a_pInput);
      }
      else {
        for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pInput->getChildren().begin(); it != a_pInput->getChildren().end(); it++) {
          hideOriginalSkybox(*it);
        }
      }

      if (a_pInput == a_pInput->getSceneManager()->getRootSceneNode() && m_pSkyBox != nullptr)
        m_pSkyBox->setID(-1);
    }
  }
}
