/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {

  namespace scenenodes {
    const int g_AiPathNodeId = MAKE_IRR_ID('a', 'i', 'p', 'a');
    const irr::c8 g_AiPathNodeName[] = "AiPathNode";

    /**
    * @class CAiPathNode
    * @authod Christian Keimel
    * This node stores the necessary AI data
    */
    class CAiPathNode : public irr::scene::ISceneNode {
      public:
        enum enSegmentType {
          Default,
          Jump,
          Block,
          Loop
        };

        typedef struct SAiPathSection {
          int                  m_iIndex;      /**< Index of the section. Only necessary for (de)serialization */
          int                  m_iNode;       /**< ID of the AI node (only for integrity testing) */
          irr::core::vector3df m_cPosition;   /**< The position of the segment */
          irr::core::vector3df m_cSideVector; /**< The side direction of the segment (normalized) */
          irr::core::vector3df m_cNormal;     /**< This segment's normal */
          irr::f32             m_fWidth;      /**< Width of the segment */
          irr::f32             m_fFactor;     /**< Factor for the width of the segment */
          irr::f32             m_fMinSpeed;   /**< Minimum speed (for jumps only) */
          irr::f32             m_fMaxSpeed;   /**< Maximum speed (for jumps only) */
          irr::f32             m_fBestSpeed;  /**< Best speed (for jumps only) */
          irr::s32             m_iTag;        /**< A tag for turnoffs and blockers */
          enSegmentType        m_eType;       /**< The type of this segment */

          std::vector<irr::s32        > m_vNextIndices;   /**< Indices of the next segments (for de-serialization only) */
          std::vector<SAiPathSection *> m_vNextSegments;  /**< List of the next segments */
          std::vector<SAiPathSection *> m_vPrevSegments;  /**< The previous segments */

          SAiPathSection();
          SAiPathSection(const std::string &a_sData);
          ~SAiPathSection();

          irr::core::line3df getSegmentLine();

          void draw(irr::video::IVideoDriver *a_pDrv, bool a_bIsSelected);

          std::string serialize();

          /**
          * Add a segment to the vector of next sections
          * @param a_pNext the next section to add
          */
          void addToNextSections(SAiPathSection *a_pNext);

          /**
          * Add a segment to the vector of previous sections
          * @param a_pPrev the previous section to add
          */
          void addToPrevSections(SAiPathSection *a_pPrev);
        } 
        SAiPathSection;

        std::vector<SAiPathSection *> m_vSections;
        SAiPathSection *m_pSelected;

        bool m_bNewlyCreated;

      private:

        irr::core::aabbox3df      m_cBox;
        irr::video::IVideoDriver *m_pDrv;
        bool                      m_bStartNode;   /**< Is this the node for the race start? Only necessary if more nodes overlap */

      public:
        CAiPathNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CAiPathNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        virtual void OnRegisterSceneNode();

        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;

        void addPathSection(CAiPathNode::SAiPathSection *a_pNew);

        /**
        * Is this a starting path? Only necessary for the start if two paths overlap
        * @return true if this section is marked as a startup section
        */
        bool isStartupPath() const;
    };
  }
}