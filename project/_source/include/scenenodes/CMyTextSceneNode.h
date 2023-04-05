// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <irrlicht.h>
#include "ITextSceneNode.h"
#include "IBillboardTextSceneNode.h"
#include "IGUIFont.h"
#include "IGUIFontBitmap.h"
#include "ISceneCollisionManager.h"
#include "SMesh.h"

namespace irr {
  namespace scene {
    class ICameraSceneNode;
  }
}

namespace dustbin
{
  namespace scenenodes
  {
    class CMyTextNode : public irr::scene::ITextSceneNode
    {
    public:

      //! constructor
      CMyTextNode(ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
        irr::gui::IGUIFont* font, irr::scene::ISceneCollisionManager* coll,
        const irr::core::vector3df& position = irr::core::vector3df(0,0,0), const wchar_t* text=0,
        irr::video::SColor color=irr::video::SColor(100,0,0,0));

      //! destructor
      virtual ~CMyTextNode();

      virtual void OnRegisterSceneNode() IRR_OVERRIDE;

      //! renders the node.
      virtual void render() IRR_OVERRIDE;

      //! returns the axis aligned bounding box of this node
      virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const IRR_OVERRIDE;

      //! sets the text string
      virtual void setText(const wchar_t* text) IRR_OVERRIDE;

      //! get the text string
      virtual const wchar_t* getText() const IRR_OVERRIDE;

      //! sets the color of the text
      virtual void setTextColor(irr::video::SColor color) IRR_OVERRIDE;

      //! get the color of the text
      virtual irr::video::SColor getTextColor() const IRR_OVERRIDE;

      //! set the font used to draw the text
      virtual void setFont(irr::gui::IGUIFont* font) IRR_OVERRIDE;

      //! Get the font used to draw the text
      virtual irr::gui::IGUIFont* getFont() const IRR_OVERRIDE;

      //! Returns type of the scene node
      virtual irr::scene::ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return irr::scene::ESNT_TEXT; }

    private:

      irr::core::stringw Text;
      irr::video::SColor Color;
      irr::gui::IGUIFont* Font;
      irr::scene::ISceneCollisionManager* Coll;
      irr::core::aabbox3d<irr::f32> Box;
    };

    class CMyBillboardText : public irr::scene::IBillboardTextSceneNode
    {
    public:

      CMyBillboardText(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* mgr, irr::s32 id,
        irr::gui::IGUIFont* font,const wchar_t* text,
        const irr::core::vector3df& position, const irr::core::dimension2d<irr::f32>& size,
        irr::video::SColor colorTop, irr::video::SColor shade_bottom);

      //! destructor
      virtual ~CMyBillboardText();

      //! sets the vertex positions etc
      virtual void OnAnimate(irr::u32 timeMs) IRR_OVERRIDE;

      //! registers the node into the transparent pass
      virtual void OnRegisterSceneNode() IRR_OVERRIDE;

      //! renders the node.
      virtual void render() IRR_OVERRIDE;

      //! returns the axis aligned bounding box of this node
      virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const IRR_OVERRIDE;

      //! sets the text string
      virtual void setText(const wchar_t* text) IRR_OVERRIDE;

      //! get the text string
      virtual const wchar_t* getText() const IRR_OVERRIDE;

      //! Get the font used to draw the text
      virtual irr::gui::IGUIFont* getFont() const IRR_OVERRIDE;

      //! sets the size of the billboard
      virtual void setSize(const irr::core::dimension2d<irr::f32>& size) IRR_OVERRIDE;

      //! gets the size of the billboard
      virtual const irr::core::dimension2d<irr::f32>& getSize() const IRR_OVERRIDE;

      virtual irr::video::SMaterial& getMaterial(irr::u32 i) IRR_OVERRIDE;

      //! returns amount of materials used by this scene node.
      virtual irr::u32 getMaterialCount() const IRR_OVERRIDE;

      //! Returns type of the scene node
      virtual irr::scene::ESCENE_NODE_TYPE getType() const IRR_OVERRIDE { return irr::scene::ESNT_BILLBOARD_TEXT; }

      //! Set the color of all vertices of the billboard
      //! \param overallColor: the color to set
      virtual void setColor(const irr::video::SColor & overallColor) IRR_OVERRIDE;

      //! Set the color of the top and bottom vertices of the billboard
      //! \param topColor: the color to set the top vertices
      //! \param bottomColor: the color to set the bottom vertices
      virtual void setColor(const irr::video::SColor & topColor, const irr::video::SColor & bottomColor) IRR_OVERRIDE;

      //! Gets the color of the top and bottom vertices of the billboard
      //! \param topColor: stores the color of the top vertices
      //! \param bottomColor: stores the color of the bottom vertices
      virtual void getColor(irr::video::SColor & topColor, irr::video::SColor & bottomColor) const IRR_OVERRIDE;

      virtual void setSize(irr::f32 height, irr::f32 bottomEdgeWidth, irr::f32 topEdgeWidth) IRR_OVERRIDE
      {
        setSize(irr::core::dimension2df(bottomEdgeWidth, height));
      }

      virtual void getSize(irr::f32& height, irr::f32& bottomEdgeWidth, irr::f32& topEdgeWidth) const IRR_OVERRIDE
      {
        height = Size.Height;
        bottomEdgeWidth = Size.Width;
        topEdgeWidth = Size.Width;
      }

      virtual const irr::core::aabbox3d<irr::f32>& getTransformedBillboardBoundingBox(const irr::scene::ICameraSceneNode* camera) IRR_OVERRIDE;

      //! Get the amount of mesh buffers.
      virtual irr::u32 getMeshBufferCount() const
      {
        return Mesh ? Mesh->getMeshBufferCount() : 0;
      }

      //! Get pointer to the mesh buffer.
      virtual irr::scene::IMeshBuffer* getMeshBuffer(irr::u32 nr) const
      {
        return Mesh ? Mesh->getMeshBuffer(nr) : 0;
      }

    protected:
      void updateMesh(const irr::scene::ICameraSceneNode* camera);

    private:

      irr::core::stringw Text;
      irr::gui::IGUIFontBitmap* Font;

      irr::core::dimension2d<irr::f32> Size;
      irr::core::aabbox3d<irr::f32> BBox;
      irr::video::SMaterial Material;

      irr::video::SColor ColorTop;
      irr::video::SColor ColorBottom;
      struct SSymbolInfo
      {
        irr::u32 bufNo;
        irr::f32 Width;
        irr::f32 Kerning;
        irr::u32 firstInd;
        irr::u32 firstVert;
      };

      irr::core::array < SSymbolInfo > Symbol;

      irr::scene::SMesh *Mesh;
    };

  } // end namespace scene
} // end namespace irr
