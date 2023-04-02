// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
//
// I modified the original code with a visibility check in the render method
// and moved it into my namepace
#pragma once

#include "IBillboardSceneNode.h"
#include "SMeshBuffer.h"

namespace dustbin {
  namespace scenenodes {

    //! Scene node which is a billboard. A billboard is like a 3d sprite: A 2d element,
    //! which always looks to the camera.
    class CMyBillboard : virtual public irr::scene::IBillboardSceneNode
    {
    public:

      //! constructor
      CMyBillboard(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
        const irr::core::vector3df& position, const irr::core::dimension2d<irr::f32>& size,
        irr::video::SColor colorTop=irr::video::SColor(0xFFFFFFFF),
        irr::video::SColor colorBottom=irr::video::SColor(0xFFFFFFFF));

      virtual ~CMyBillboard();

      //! pre render event
      virtual void OnRegisterSceneNode() IRR_OVERRIDE;

      //! render
      virtual void render() IRR_OVERRIDE;

      //! returns the axis aligned bounding box of this node
      virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const IRR_OVERRIDE;

      //! sets the size of the billboard
      virtual void setSize(const irr::core::dimension2d<irr::f32>& size) IRR_OVERRIDE;

      //! Sets the widths of the top and bottom edges of the billboard independently.
      virtual void setSize(irr::f32 height, irr::f32 bottomEdgeWidth, irr::f32 topEdgeWidth) IRR_OVERRIDE;

      //! gets the size of the billboard
      virtual const irr::core::dimension2d<irr::f32>& getSize() const IRR_OVERRIDE;

      //! Gets the widths of the top and bottom edges of the billboard.
      virtual void getSize(irr::f32& height, irr::f32& bottomEdgeWidth, irr::f32& topEdgeWidth) const IRR_OVERRIDE;

      virtual irr::video::SMaterial& getMaterial(irr::u32 i) IRR_OVERRIDE;

      //! returns amount of materials used by this scene node.
      virtual irr::u32 getMaterialCount() const IRR_OVERRIDE;

      //! Set the color of all vertices of the billboard
      //! \param overallColor: the color to set
      virtual void setColor(const irr::video::SColor& overallColor) IRR_OVERRIDE;

      //! Set the color of the top and bottom vertices of the billboard
      //! \param topColor: the color to set the top vertices
      //! \param bottomColor: the color to set the bottom vertices
      virtual void setColor(const irr::video::SColor& topColor,
        const irr::video::SColor& bottomColor) IRR_OVERRIDE;

      //! Gets the color of the top and bottom vertices of the billboard
      //! \param[out] topColor: stores the color of the top vertices
      //! \param[out] bottomColor: stores the color of the bottom vertices
      virtual void getColor(irr::video::SColor& topColor,
        irr::video::SColor& bottomColor) const IRR_OVERRIDE;

      //! Get the real boundingbox used by the billboard (which depends on the active camera)
      virtual const irr::core::aabbox3d<irr::f32>& getTransformedBillboardBoundingBox(const irr::scene::ICameraSceneNode* camera) IRR_OVERRIDE;

      //! Get the amount of mesh buffers.
      virtual irr::u32 getMeshBufferCount() const override
      {
        return Buffer ? 1 : 0;
      }

      //! Get pointer to the mesh buffer.
      virtual irr::scene::IMeshBuffer* getMeshBuffer(irr::u32 nr) const override
      {
        if ( nr == 0 )
          return Buffer;
        return 0;
      }

      //! Writes attributes of the scene node.
      virtual void serializeAttributes(irr::io::IAttributes* out, irr::io::SAttributeReadWriteOptions* options=0) const IRR_OVERRIDE;

      //! Reads attributes of the scene node.
      virtual void deserializeAttributes(irr::io::IAttributes* in, irr::io::SAttributeReadWriteOptions* options=0) IRR_OVERRIDE;

      //! Returns type of the scene node
      virtual irr::scene::ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return irr::scene::ESNT_BILLBOARD; }

      //! Creates a clone of this scene node and its children.
      virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* newParent=0, irr::scene::ISceneManager* newManager=0) IRR_OVERRIDE;

    protected:
      void updateMesh(const irr::scene::ICameraSceneNode* camera);

    private:

      //! Size.Width is the bottom edge width
      irr::core::dimension2d<irr::f32> Size;
      irr::f32 TopEdgeWidth;

      //! BoundingBox which is large enough to contain the billboard independent of the camera
      // TODO: BUG - still can be wrong with scaling < 1. Billboards should calculate relative coordinates for their mesh
      // and then use the node-scaling. But needs some work...
      /** Note that we can't use the real boundingbox for culling because at that point
      the camera which is used to calculate the billboard is not yet updated. So we only
      know the real boundingbox after rendering - which is too late for culling. */
      irr::core::aabbox3d<irr::f32> BBoxSafe;

      irr::scene::SMeshBuffer* Buffer;
    };


  } // end namespace scene
} // end namespace irr
