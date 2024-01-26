/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#version 100
precision mediump float;
// This is the fragment shader for the solid material in the scene

uniform sampler2D tTexture1;    // The background texture
uniform sampler2D tTexture2;    // The second layer texture (hexagonal)
uniform sampler2D tTexture3;    // The third layer texture (e.g. road markings)

uniform vec3 vLight;      // The light direction

uniform float fMaxDepth;    // The maximum depth of the light
uniform float fVertical;    // Is this fragment from a vertical wall?
uniform float fMinY;        // This min Y value for vertical wall second texture alignment

uniform int iNoTextures;    // The number of use materials
uniform int iMaterial;      // The type of material

varying vec3 vNormal;     // The normal of the fragment
varying vec4 vWorldPos;   // The position of the fragment in the world

/* Varyings */

varying vec4 vTextureCoord0;

// Decode a float from the RGB porition of a color (taken from https://www.shadertoy.com/view/WsVGzm)
float rgbToFloat(vec3 rgb) { return rgb.r + (rgb.g / 256.0) + (rgb.b / 65536.0); }

void main(void) {
  // Calculate the shadowing from the light direction
  float fLight = clamp(dot(vLight, vNormal), 0.3, 1.0);
  
  // Initialize the darkening factor
  float fFactor = fLight;

  // The color from the first texture (always used)
  vec4 vTextureOne = texture2D(tTexture1, vec2(vTextureCoord0));
  
  // Do we use more than one texture?
  if (iNoTextures > 1) {
    // The position of the color to take from the second texture
    vec2 vTwoPos;
    
    // Are we a wall?
    if (iMaterial == 4 || iMaterial == 5 || iMaterial == 6) {
      vTwoPos = vec2(vTextureCoord0);
    }
    else {
      // We just calculate the position in the second texture from the world x and z coordinates (+y for beauty)
      vTwoPos = vec2(12.0 * (vWorldPos.x + vWorldPos.y), 12.0 * (vWorldPos.z + vWorldPos.y));
    }
    
    // Get the color from the second texture
    vec4 vTextureTwo = texture2D(tTexture2, vTwoPos);
    
    // We want to highlight the second texture a little
    // for a cyber-style appearance
    if (vTextureTwo.a > 0.25) {
      fFactor = max(fFactor, 0.5);
    }
    
    // Update the current color
    vTextureOne = vTextureTwo.a * vTextureTwo + (1.0 - vTextureTwo.a) * vTextureOne;
    
    // Are we using more than two textures?
    if (iNoTextures > 2) {
      // The texture coordinate of the third texture as the same as from the first
      vec4 vTextureThree = texture2D(tTexture3, vec2(vTextureCoord0));
      
      // Again we want this texture to glow a little in the dark shadow
      if (vTextureThree.a > 0.75) {
        fFactor = max(fFactor, 0.75);
      }
      
      // Update the texture
      vTextureOne = vTextureThree.a * vTextureThree + (1.0 - vTextureThree.a) * vTextureOne;
    }
  }

  if (iMaterial == 7)
    vTextureOne.a = 0.5;
  
  // Set the calculated fragment color
  gl_FragColor = fFactor * vTextureOne;
}
