/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace shaders {
    /**
    * The supported materials
    */
    enum class enMaterialType {
      SolidOne,     // Solid with one texture
      SolidTwo,     // Solid with two textures, the second is blended on top of the first one
      SolidThree,   // Solid with three textures, texture two and three are blended in
      Marble,       // Marble texture which comes which uses a bit less of the shadow
      Wall1,        // Wall with a single texture
      Wall2,        // Wall textures which use the texture coordinates for all texture layers
      Wall3,        // Wall textures which use the texture coordinates for all texture layers
      Transparent,  // Transparent Marble
      ShadowMap,    // The shadow map, must not be used on nodes
      ShadowMap2,   // The transparent shadow map, must not be used on nodes
      ShadowMap3,   // The transparent color map, must not be used on nodes
      ShadowMap4,   // The marble shadow map, must not be used on nodes
      ShadowMapNo, 
      
      Count
    };

    /**
    * The possible shadow rendering options
    */
    enum class enShadowMode {
      Off,          // No shadows
      Solid,        // Solid shadows are rendered
      SolidTrans,   // Solid and transparent shadows are rendered
      TransColor,   // Solid, transparent and transparent color shadows are rendered

      Count
    };

    /**
    * The shadow rendering options
    */
    enum class enShadowRender {
      Off,      // No shadows
      Static,   // Only static shadows
      All,      // Static and moving shadows

      Count
    };

    /**
    * An enum to define the quality, i.e. shadow map
    * resolution, of the shadows
    */
    enum class enShadowQuality {
      Top   = 0,  // 16384x16384
      High  = 1,  // 8192x8192
      HiMid = 2,  // 4096x4096
      LoMid = 3,  // 2048x2048
      Low   = 4,  // 1024x1024
      Poor  = 5,  // 512x512

      Count
    };

    /**
    * This enumeration defines which
    * shadow map shall be rendered
    */
    enum class enShadowMap {
      Solid       =  1,   // The shadow map for the solid shadows
      Marbles     =  2,   // The shadow map for the marbles
      Moving      =  4,   // The shadow map for moving objects
      Transparent =  8,   // The shadow map for the (semi)transparent shadows
      TranspColor = 16,   // The map with the colors for the (semi)transparent shadows

      Count
    };

    /**
    * An enumeration for the type of object
    */
    enum enObjectType {
      Static,   // A static object
      Moving,   // A moving object
      Marble    // A Marble
    };

    /**
    * Helper function converting a shadow quality enum value to a resolution
    * (Note: we always use square textures for the shadow map)
    * @param a_eQuality the quality setting we request the shadow map size for
    * @return the shadow map size for a given shadow quality
    * @see enShadowQuality
    */
    irr::s32 shadowQualityToSize(enShadowQuality a_eQuality);
  }
}
