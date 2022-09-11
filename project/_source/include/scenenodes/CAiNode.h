/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {

  namespace scenenodes {
    const int g_AiNodeId = MAKE_IRR_ID('a', 'i', 'n', 'd');
    const irr::c8 g_AiNodeName[] = "AiNode";

    /**
    * @class CAiNode
    * @authod Christian Keimel
    * This node stores the necessary AI data
    */
    class CAiNode : public irr::scene::ISceneNode {
      public:
        struct SAiPathNode;

        enum class enLinkType {
          Default,
          Cruise,
          TimeAttack,
          Offensive,
          Defensive,
          MayBeBlocked
        };

        struct SAiLink {
          SAiPathNode *m_pNext;       /**< The next path node*/
          SAiPathNode *m_pThis;       /**< The previous path node */
          enLinkType   m_eType;       /**< Type of this link */
          int          m_iNext;       /**< Index of the next path node (for de-serialization) */
          int          m_iPriority;   /**< A priority index for multiple links of a path node */
          irr::f32     m_fLinkLength; /**< Length of the link line */

          irr::core::line3df m_cLinkLine;   /**< The 3d line that represents this link */

          std::string m_sBlocking;

          SAiLink();
          SAiLink(const std::string &a_sData, SAiPathNode *a_pThis);

          std::string serialize();
        };

        struct SAiPathNode {
          int m_iIndex;   /**< The index (important for de-serialization) */

          irr::core::vector3df m_cPos;      /**< The position */
          irr::core::vector3df m_cNormal;   /**< The normal */

          std::vector<SAiLink *> m_vNext;      /**< The following path nodes */

          bool m_bJump;
          irr::f32 m_fMinVel;
          irr::f32 m_fMaxVel;
          irr::scene::ISceneNode *m_pBox;

          SAiPathNode();
          SAiPathNode(const std::string &a_sData);
          ~SAiPathNode();

          std::string serialize();

          void fillNextVector(const std::vector<SAiPathNode *> &a_cPathData);
        };

      private:
        irr::core::aabbox3df m_cBox;
        std::vector<SAiPathNode *> m_vPathData;

        std::map<irr::s32, std::vector<irr::core::line3df>> m_mDebug;

        void updatePathIndices();

      public:
        CAiNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CAiNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        virtual void OnRegisterSceneNode();

        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;

        void setDebugLines(int a_iMarbleId, const std::vector<irr::core::line3df> &a_vLines);

        /**
        * Get the defined path from the AI scene-node.
        * Ownership is passed to the calling object
        * @return the defined path
        */
        std::vector<SAiPathNode *> &getPath();
      };
  }
}