// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <animators/CAnimatorFactory.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinLight.h>
#include <scenenodes/CRespawnNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CAiPathNode.h>
#include <scenenodes/CSpeedNode.h>
#include <scenenodes/CDustbinId.h>

#include <editor/CTrackPart_FlatSegment.h>
#include <editor/CTrackPart_Checkpoint.h>
#include <editor/CAiPath.h>

#include <commands/CCmdAddMarbleGP.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <platform/CPlatform.h>
#include <filesystem>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param m_pEditTrack the edit track command
    */
    CCmdAddMarbleGP::CCmdAddMarbleGP(CCmdEditTrack *a_pEditTrack) : 
      ICommand    ("CmdAddMarbleGP"),
      m_pFs       (tracked::IGlobal::getInstance()->getFileSystem()),
      m_pDrv      (tracked::IGlobal::getInstance()->getVideoDriver()),
      m_pOldSmgr  (tracked::IGlobal::getInstance()->getSceneManager()),
      m_pNewSmgr  (nullptr),
      m_pWorld    (nullptr),
      m_pEditTrack(a_pEditTrack)
    {
      m_sFolder = helpers::ws2s(platform::portableGetTrackEdPath()) + "/data/levels/" + m_pEditTrack->getUUID();
      std::filesystem::create_directories(m_sFolder);

      printf("Working dir: %s\n", m_pFs->getWorkingDirectory().c_str());

      irr::io::IXMLWriter *l_pXmlWriter = m_pFs->createXMLWriter((m_sFolder + "/track.xml").c_str());

      if (l_pXmlWriter) {
        m_pNewSmgr = tracked::IGlobal::getInstance()->getSceneManager()->createNewSceneManager(false);

        scenenodes::CMarbleGPSceneNodeFactory *l_pFactory = new scenenodes::CMarbleGPSceneNodeFactory(m_pNewSmgr);
        m_pNewSmgr->registerSceneNodeFactory(l_pFactory);
        l_pFactory->drop();

        animators::CAnimatorFactory *l_pFactory2 = new animators::CAnimatorFactory(tracked::IGlobal::getInstance()->getDevice());
        m_pNewSmgr->registerSceneNodeAnimatorFactory(l_pFactory2);
        l_pFactory2->drop();

        irr::scene::ISceneNode *l_pSkyboxOld = findNode(m_pOldSmgr->getRootSceneNode(), irr::scene::ESNT_SKY_BOX);

        if (l_pSkyboxOld != nullptr) {
          l_pSkyboxOld->clone(m_pNewSmgr->getRootSceneNode(), m_pNewSmgr);
        }

        m_pWorld = m_pNewSmgr->addSceneNode("WorldNode", m_pNewSmgr->getRootSceneNode());

        exportAndPrepareTrack();
        addCheckpoints();
        addAiLightAndRostrum();

        m_pNewSmgr->saveScene(l_pXmlWriter, m_pFs->getWorkingDirectory());
        l_pXmlWriter->drop();

        createThumbnail();
        createInfoFile();

        m_pNewSmgr->drop();

        helpers::addLogMessage(L"Track added to MarbleGP", irr::ELL_INFORMATION, 3000);
      }
      else helpers::addLogMessage(L"Error while trying to add track to MarbleGP", irr::ELL_ERROR, 3000);
    }

    CCmdAddMarbleGP::~CCmdAddMarbleGP() {
    }

    /**
    * Export the track and add it to the new scene manager
    */
    void CCmdAddMarbleGP::exportAndPrepareTrack() {
      irr::scene::IMeshSceneNode *l_pTrack = m_pEditTrack->getTrackMesh();
      irr::scene::IMeshWriter *l_pWriter = m_pNewSmgr->createMeshWriter(irr::scene::EMWT_OBJ);
      if (l_pWriter) {
        irr::io::IWriteFile *l_pFile = m_pFs->createAndWriteFile((m_sFolder + "/track.obj").c_str());
        if (l_pFile) {
          l_pWriter->writeMesh(l_pFile, l_pTrack->getMesh());
          l_pFile->drop();
        }
        l_pWriter->drop();
      }
      else {
        helpers::addLogMessage(L"Error trying to export track mesh.", irr::ELL_ERROR, 3000);
        return;
      }

      irr::scene::ISceneNode *l_pNewTrack = m_pNewSmgr->addMeshSceneNode(
        m_pNewSmgr->getMesh(std::string("data/levels/" + m_pEditTrack->getUUID() + "/track.obj").c_str()),
        m_pWorld,
        -1,
        irr::core::vector3df(0.0f),
        irr::core::vector3df(0.0f),
        irr::core::vector3df(50.0f)
      );

      for (irr::u32 i = 0; i < l_pTrack->getMaterialCount() && i < l_pNewTrack->getMaterialCount(); i++) {
        for (irr::u32 j = 0; j < 8; j++) {
          irr::video::ITexture *l_pTexture = l_pTrack->getMaterial(i).getTexture(j);
          if (l_pTexture == nullptr)
            break;
          else {
            printf("Material %i, Texture %i set to %s\n", i, j, l_pTrack->getMaterial(i).getTexture(j)->getName().getPath().c_str());
            l_pNewTrack->getMaterial(i).setTexture(j, m_pDrv->getTexture(l_pTrack->getMaterial(i).getTexture(j)->getName().getPath()));
          }
        }
      }

      scenenodes::CPhysicsNode *l_pPhysics = reinterpret_cast<scenenodes::CPhysicsNode *>(m_pNewSmgr->addSceneNode(scenenodes::g_PhysicsNodeName, l_pNewTrack));

      irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();
      if (l_pAttr != nullptr) {
        l_pPhysics->serializeAttributes(l_pAttr);

        irr::u32 l_aRespawn[] = {
          tracked::c_iBufferIdxGround,
          tracked::c_iBufferIdxWallInner,
          tracked::c_iBufferIdxWallOuter,
              
          0xFFFF
        };

        for (irr::u32 i = 0; l_aRespawn[i] != 0xFFFF; i++) {
          irr::u32 l_iRespawn = m_pEditTrack->getBufferIndex(l_aRespawn[i]);
          if (l_iRespawn != 0xFFFF) {
            std::string s = "Respawn_" + std::to_string(l_iRespawn + 1);
            l_pAttr->addBool(s.c_str(), true);
          }
        }

        l_pPhysics->deserializeAttributes(l_pAttr);
        l_pAttr->drop();
      }    

      irr::scene::ISceneNode *l_pStart = findNode(m_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId);
      if (l_pStart != nullptr) {
        l_pStart->clone(m_pWorld, m_pNewSmgr);
      }
    }

    /**
    * Add the checkpoints to the track
    */
    void CCmdAddMarbleGP::addCheckpoints() {
      std::map<tracked::ITrackPart *, irr::s32> l_mRespawnIDs;
      std::vector<tracked::ITrackPart *> l_vTrack = m_pEditTrack->getTrack();

      scenenodes::CCheckpointNode *l_pFirst = nullptr;
      scenenodes::CCheckpointNode *l_pLast  = nullptr;

      for (auto l_pPart : l_vTrack) {
        if (l_pPart->getType() == c_sTypeCheckpoint) {
          tracked::CTrackPart_Checkpoint *l_pCheckpoint = dynamic_cast<tracked::CTrackPart_Checkpoint *>(l_pPart);
          if (l_pCheckpoint) {
            irr::scene::ISceneNode *l_pOldCheck = l_pCheckpoint->getCheckpointNode();

            if (l_pOldCheck != nullptr) {
              irr::scene::ISceneNode *l_pNewCheck = l_pOldCheck->clone(m_pWorld, m_pNewSmgr);

              scenenodes::CCheckpointNode *l_pCheck = reinterpret_cast<scenenodes::CCheckpointNode *>(m_pNewSmgr->addSceneNode(scenenodes::g_CheckpointName, l_pNewCheck));
              l_pCheck->setID(l_pCheckpoint->getId());
              l_mRespawnIDs[l_pPart] = l_pCheck->getID();

              irr::s32 l_iRespawn = -1;

              irr::scene::ISceneNode *l_pRespawn = l_pCheckpoint->getRespawnNode();
              if (l_pRespawn != nullptr) {
                scenenodes::CRespawnNode *l_pNode = reinterpret_cast<scenenodes::CRespawnNode *>(m_pNewSmgr->addSceneNode(scenenodes::g_RespawndName, m_pWorld));
                if (l_pNode != nullptr) {
                  l_pNode->setID(l_pRespawn->getID());
                  l_pNode->setPosition(l_pRespawn->getPosition());
                  l_pNode->setRotation(l_pRespawn->getRotation());

                  l_iRespawn = l_pNode->getID();
                  printf("Respawn: %i\n", l_iRespawn);
                }
              }

              if (l_pFirst == nullptr) {
                irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();
                if (l_pAttr != nullptr) {
                  l_pCheck->serializeAttributes(l_pAttr);
                  l_pAttr->setAttribute("FirstInLap", true);
                  l_pAttr->setAttribute("Respawn", l_iRespawn);
                  l_pCheck->deserializeAttributes(l_pAttr);
                  l_pAttr->drop();
                }

                irr::scene::ISceneNode *l_pLapCount = m_pNewSmgr->addMeshSceneNode(
                  m_pNewSmgr->getMesh("data/objects/lapcount.3ds"),
                  l_pCheck,
                  -1,
                  irr::core::vector3df(0.0f, 0.585f, 0.0f),
                  irr::core::vector3df(0.0f),
                  irr::core::vector3df(0.5f)
                );

                l_pLapCount->getMaterial(0).setTexture(0, m_pNewSmgr->getVideoDriver()->getTexture("data/textures/checkpoint_white.png"));
                l_pLapCount->getMaterial(1).setTexture(0, m_pNewSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

                irr::scene::ISceneNodeAnimator *l_pAnimator = m_pNewSmgr->createSceneNodeAnimator("MarbleGpTexture", l_pLapCount);

                if (l_pAnimator != nullptr) {
                  irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();
                  if (l_pAttr != nullptr) {
                    l_pLapCount->serializeAttributes(l_pAttr);
                    l_pAttr->setAttribute("Information", "Lapcount");
                    l_pAttr->setAttribute("Texture", 0);
                    l_pAttr->drop();
                  }
                }
                l_pAnimator->drop();

                l_pFirst = l_pCheck;
              }
              else {
                if (l_pLast != nullptr) {
                  irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();
                  if (l_pAttr != nullptr) {
                    l_pLast->serializeAttributes(l_pAttr);
                    l_pAttr->setAttribute("Next_1", l_pCheck->getID());
                    l_pAttr->setAttribute("Next_2", -1);
                    l_pLast->deserializeAttributes(l_pAttr);
                    l_pAttr->drop();
                  }

                  l_pAttr = m_pFs->createEmptyAttributes();
                  if (l_pAttr != nullptr) {
                    l_pCheck->serializeAttributes(l_pAttr);
                    l_pAttr->setAttribute("Respawn", l_iRespawn);
                    l_pCheck->deserializeAttributes(l_pAttr);
                    l_pAttr->drop();
                  }
                }
              }


              l_pLast = l_pCheck;
            }
          }
        }
      }

      if (l_pFirst != nullptr && l_pLast != nullptr) {
        irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();
        if (l_pAttr != nullptr) {
          l_pLast->serializeAttributes(l_pAttr);
          l_pAttr->setAttribute("Next_1", l_pFirst->getID());
          l_pAttr->setAttribute("Next_2", -1);
          l_pLast->deserializeAttributes(l_pAttr);
          l_pAttr->drop();
        }
      }
   }

    /**
    * Add the AI, light and rostrum scene nodes
    */
    void CCmdAddMarbleGP::addAiLightAndRostrum() {
      scenenodes::CAiPathNode *l_pAi = reinterpret_cast<scenenodes::CAiPathNode *>(m_pNewSmgr->addSceneNode(scenenodes::g_AiPathNodeName, m_pWorld));
      if (l_pAi != nullptr) {
        tracked::CAiPath *l_pAiPath = new tracked::CAiPath(m_pEditTrack->getTrack().front());
        l_pAiPath->addAiToNode(l_pAi);
        l_pAiPath->addSpeedIndicators(m_pWorld, m_pNewSmgr);
      }

      scenenodes::CDustbinLight *l_pLight = reinterpret_cast<scenenodes::CDustbinLight *>(findNode(m_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_DustbinLightId));
      if (l_pLight != nullptr) {
        l_pLight->clone(m_pNewSmgr->getRootSceneNode(), m_pNewSmgr);
      }

      irr::scene::ISceneNode *l_pRostrum = findNode(m_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RostrumNodeId);
      if (l_pRostrum != nullptr)
        l_pRostrum->clone(m_pWorld, m_pNewSmgr);
    }

    /**
    * Create and save the thumbnail
    */
    void CCmdAddMarbleGP::createThumbnail() {
      irr::video::ITexture *l_pRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "thumbnail");
      if (l_pRtt != nullptr) {
        irr::core::vector3df l_cPos;
        irr::core::vector3df l_cTgt;

        m_pEditTrack->getThumbnailCamera(l_cPos, l_cTgt);

        irr::scene::ICameraSceneNode *l_pOldCam = m_pOldSmgr->getActiveCamera();

        m_pDrv->setRenderTarget(l_pRtt, true, true);
        irr::scene::ICameraSceneNode *l_pCam = m_pOldSmgr->addCameraSceneNode(m_pNewSmgr->getRootSceneNode(), l_cPos, l_cTgt);
        l_pCam->setAspectRatio(1.0f);

        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_USER_EVENT;
        l_cEvent.UserEvent.UserData1 = c_iRenderScene;
        l_cEvent.UserEvent.UserData2 = c_iRenderScene;

        tracked::IGlobal::getInstance()->getDevice()->postEventFromUser(l_cEvent);

        m_pDrv->setRenderTarget(nullptr);

        irr::video::IImage *l_pImg = m_pDrv->createImage(l_pRtt, irr::core::vector2di(0, 0), irr::core::dimension2du(512, 512));
        m_pDrv->writeImageToFile(l_pImg, (m_sFolder + "/thumbnail.png").c_str());
            
        l_pImg->drop();

        m_pOldSmgr->setActiveCamera(l_pOldCam);
      }
    }

    /**
    * Write the info XML file
    */
    void CCmdAddMarbleGP::createInfoFile() {
        irr::io::IXMLWriter *l_pInfo = m_pFs->createXMLWriter((m_sFolder + "/info.xml").c_str());
        if (l_pInfo) {
          l_pInfo->writeXMLHeader();
          l_pInfo->writeElement(L"marbles3", false);
          l_pInfo->writeLineBreak();

          l_pInfo->writeElement(L"position", false);
          l_pInfo->writeText(L"99");
          l_pInfo->writeClosingTag(L"position");
          l_pInfo->writeLineBreak();

          l_pInfo->writeElement(L"name", false);
          l_pInfo->writeText(helpers::s2ws(m_pEditTrack->getTrackName()).c_str());
          l_pInfo->writeClosingTag(L"name");
          l_pInfo->writeLineBreak();

          l_pInfo->writeClosingTag(L"marbles3");
          l_pInfo->drop();
        }
    }

      irr::scene::ISceneNode *CCmdAddMarbleGP::findNode(irr::scene::ISceneNode *a_pParent, irr::scene::ESCENE_NODE_TYPE a_eType) {
      if (a_pParent->getType() == a_eType)
        return a_pParent;

      for (auto l_pChild: a_pParent->getChildren()) {
        irr::scene::ISceneNode *l_pRet = findNode(l_pChild, a_eType);
        if (l_pRet != nullptr)
          return l_pRet;
      }

      return nullptr;
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdAddMarbleGP::isFinished() {
      return true;
    }
  }
}