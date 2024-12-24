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
#include <commands/CCmdEditTrack.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor. All work is done here
    * @param m_pEditTrack the edit track command
    */
    CCmdAddMarbleGP::CCmdAddMarbleGP(CCmdEditTrack *a_pEditTrack) : ICommand("CmdAddMarbleGP") {
      std::string l_sFolder = "data/levels/editor/";

      irr::io::IFileSystem      *l_pFs      = tracked::IGlobal::getInstance()->getFileSystem();
      irr::video::IVideoDriver  *l_pDrv     = tracked::IGlobal::getInstance()->getVideoDriver();
      irr::scene::ISceneManager *l_pOldSmgr = tracked::IGlobal::getInstance()->getSceneManager();

      printf("Working dir: %s\n", l_pFs->getWorkingDirectory().c_str());

      irr::io::IXMLWriter *l_pXmlWriter = l_pFs->createXMLWriter((l_sFolder + "track.xml").c_str());

      if (l_pXmlWriter) {
        irr::scene::ISceneManager *l_pSmgr = tracked::IGlobal::getInstance()->getSceneManager()->createNewSceneManager(false);

        scenenodes::CMarbleGPSceneNodeFactory* l_pFactory = new scenenodes::CMarbleGPSceneNodeFactory(l_pSmgr);
        l_pSmgr->registerSceneNodeFactory(l_pFactory);
        l_pFactory->drop();

        animators::CAnimatorFactory* l_pFactory2 = new animators::CAnimatorFactory(tracked::IGlobal::getInstance()->getDevice());
        l_pSmgr->registerSceneNodeAnimatorFactory(l_pFactory2);
        l_pFactory2->drop();

        irr::scene::ISceneNode *l_pSkyboxOld = findNode(l_pOldSmgr->getRootSceneNode(), irr::scene::ESNT_SKY_BOX);

        if (l_pSkyboxOld != nullptr) {
          l_pSkyboxOld->clone(l_pSmgr->getRootSceneNode(), l_pSmgr);
        }
        
        irr::scene::ISceneNode *l_pWorld = l_pSmgr->addSceneNode("WorldNode", l_pSmgr->getRootSceneNode());

        {
          irr::scene::IMeshSceneNode *l_pTrack = a_pEditTrack->getTrackMesh();
          irr::scene::IMeshWriter *l_pWriter = l_pSmgr->createMeshWriter(irr::scene::EMWT_OBJ);
          if (l_pWriter) {
            irr::io::IWriteFile *l_pFile = l_pFs->createAndWriteFile((l_sFolder + "/track.obj").c_str());
            if (l_pFile) {
              l_pWriter->writeMesh(l_pFile, l_pTrack->getMesh());
              l_pFile->drop();
            }
            l_pWriter->drop();
          }

          irr::scene::ISceneNode *l_pNewTrack = l_pSmgr->addMeshSceneNode(
            l_pSmgr->getMesh((l_sFolder + "/track.obj").c_str()),
            l_pWorld,
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
                l_pNewTrack->getMaterial(i).setTexture(j, l_pDrv->getTexture(l_pTrack->getMaterial(i).getTexture(j)->getName().getPath()));
              }
            }
          }

          scenenodes::CPhysicsNode *l_pPhy = reinterpret_cast<scenenodes::CPhysicsNode *>(l_pSmgr->addSceneNode(scenenodes::g_PhysicsNodeName, l_pNewTrack));

          irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
          if (l_pAttr != nullptr) {
            l_pPhy->serializeAttributes(l_pAttr);

            irr::u32 l_aRespawn[] = {
              tracked::c_iBufferIdxGround,
              tracked::c_iBufferIdxWallInner,
              tracked::c_iBufferIdxWallOuter,
              
              0xFFFF
            };

            for (irr::u32 i = 0; l_aRespawn[i] != 0xFFFF; i++) {
              irr::u32 l_iRespawn = a_pEditTrack->getBufferIndex(l_aRespawn[i]);
              if (l_iRespawn != 0xFFFF) {
                std::string s = "Respawn_" + std::to_string(l_iRespawn + 1);
                l_pAttr->addBool(s.c_str(), true);
              }
            }

            l_pPhy->deserializeAttributes(l_pAttr);
            l_pAttr->drop();
          }
        }

        {
          irr::scene::ISceneNode *l_pStart = findNode(l_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId);
          if (l_pStart != nullptr) {
            l_pStart->clone(l_pWorld, l_pSmgr);
          }
        }

        std::map<tracked::ITrackPart *, irr::s32> l_mRespawnIDs;

        {
          std::vector<tracked::ITrackPart *> l_vTrack = a_pEditTrack->getTrack();

          scenenodes::CCheckpointNode *l_pFirst = nullptr;
          scenenodes::CCheckpointNode *l_pLast  = nullptr;

          for (auto l_pPart : l_vTrack) {
            if (l_pPart->getType() == c_sTypeCheckpoint) {
              tracked::CTrackPart_Checkpoint *l_pCheckpoint = dynamic_cast<tracked::CTrackPart_Checkpoint *>(l_pPart);
              if (l_pCheckpoint) {
                irr::scene::ISceneNode *l_pOldCheck = l_pCheckpoint->getCheckpointNode();

                if (l_pOldCheck != nullptr) {
                  irr::scene::ISceneNode *l_pNewCheck = l_pOldCheck->clone(l_pWorld, l_pSmgr);

                  scenenodes::CCheckpointNode *l_pCheck = reinterpret_cast<scenenodes::CCheckpointNode *>(l_pSmgr->addSceneNode(scenenodes::g_CheckpointName, l_pNewCheck));
                  l_pCheck->setID(l_pCheckpoint->getId());
                  l_mRespawnIDs[l_pPart] = l_pCheck->getID();

                  irr::s32 l_iRespawn = -1;

                  irr::scene::ISceneNode *l_pRespawn = l_pCheckpoint->getRespawnNode();
                  if (l_pRespawn != nullptr) {
                    scenenodes::CRespawnNode *l_pNode = reinterpret_cast<scenenodes::CRespawnNode *>(l_pSmgr->addSceneNode(scenenodes::g_RespawndName, l_pWorld));
                    if (l_pNode != nullptr) {
                      l_pNode->setID(l_pRespawn->getID());
                      l_pNode->setPosition(l_pRespawn->getPosition());
                      l_pNode->setRotation(l_pRespawn->getRotation());

                      l_iRespawn = l_pNode->getID();
                      printf("Respawn: %i\n", l_iRespawn);
                    }
                  }

                  if (l_pFirst == nullptr) {
                    irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
                    if (l_pAttr != nullptr) {
                      l_pCheck->serializeAttributes(l_pAttr);
                      l_pAttr->setAttribute("FirstInLap", true);
                      l_pAttr->setAttribute("Respawn", l_iRespawn);
                      l_pCheck->deserializeAttributes(l_pAttr);
                      l_pAttr->drop();
                    }

                    irr::scene::ISceneNode *l_pLapCount = l_pSmgr->addMeshSceneNode(
                      l_pSmgr->getMesh("data/objects/lapcount.3ds"),
                      l_pCheck,
                      -1,
                      irr::core::vector3df(0.0f, 0.585f, 0.0f),
                      irr::core::vector3df(0.0f),
                      irr::core::vector3df(0.5f)
                    );

                    l_pLapCount->getMaterial(0).setTexture(0, l_pSmgr->getVideoDriver()->getTexture("data/textures/checkpoint_white.png"));
                    l_pLapCount->getMaterial(1).setTexture(0, l_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

                    irr::scene::ISceneNodeAnimator *l_pAnimator = l_pSmgr->createSceneNodeAnimator("MarbleGpTexture", l_pLapCount);

                    if (l_pAnimator != nullptr) {
                      irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
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
                      irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
                      if (l_pAttr != nullptr) {
                        l_pLast->serializeAttributes(l_pAttr);
                        l_pAttr->setAttribute("Next_1", l_pCheck->getID());
                        l_pAttr->setAttribute("Next_2", -1);
                        l_pLast->deserializeAttributes(l_pAttr);
                        l_pAttr->drop();
                      }

                      l_pAttr = l_pFs->createEmptyAttributes();
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
            irr::io::IAttributes *l_pAttr = l_pFs->createEmptyAttributes();
            if (l_pAttr != nullptr) {
              l_pLast->serializeAttributes(l_pAttr);
              l_pAttr->setAttribute("Next_1", l_pFirst->getID());
              l_pAttr->setAttribute("Next_2", -1);
              l_pLast->deserializeAttributes(l_pAttr);
              l_pAttr->drop();
            }
          }
        }

        {
          scenenodes::CAiPathNode *l_pAi = reinterpret_cast<scenenodes::CAiPathNode *>(l_pSmgr->addSceneNode(scenenodes::g_AiPathNodeName, l_pWorld));
          if (l_pAi != nullptr) {
            tracked::CAiPath *l_pAiPath = new tracked::CAiPath(a_pEditTrack->getTrack().front());
            l_pAiPath->addAiToNode(l_pAi);
          }
        }

        {
          scenenodes::CDustbinLight *l_pLight = reinterpret_cast<scenenodes::CDustbinLight *>(findNode(l_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_DustbinLightId));
          if (l_pLight != nullptr) {
            l_pLight->clone(l_pSmgr->getRootSceneNode(), l_pSmgr);
          }
        }

        {
          irr::scene::ISceneNode *l_pRostrum = findNode(l_pOldSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RostrumNodeId);
          if (l_pRostrum != nullptr)
            l_pRostrum->clone(l_pWorld, l_pSmgr);
        }

        {
          std::vector<irr::scene::ISceneNode *> l_vSpeedNodes = a_pEditTrack->getSpeedNodes();

          for (auto l_pNode: l_vSpeedNodes) {
            l_pNode->clone(l_pWorld, l_pSmgr);
          }
        }

        l_pSmgr->saveScene(l_pXmlWriter, l_pFs->getWorkingDirectory());
        l_pSmgr->drop();
        l_pXmlWriter->drop();
      }
    }

    CCmdAddMarbleGP::~CCmdAddMarbleGP() {
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