/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// This is the fragment shader for the solid material in the scene

uniform sampler2D tTexture1;    // The background texture
uniform sampler2D tTexture2;    // The second layer texture (hexagonal)
uniform sampler2D tTexture3;    // The third layer texture (e.g. road markings)
uniform sampler2D tShadow;      // The shadow map texture
uniform sampler2D tShadow2;     // The transparent shadow map texture
uniform sampler2D tShadow3;     // The color map for the transparent shadows

uniform vec3 vLight;      // The light direction

uniform float fMaxDepth;    // The maximum depth of the light
uniform float fVertical;    // Is this fragment from a vertical wall?
uniform float fMinY;        // This min Y value for vertical wall second texture alignment

uniform int iShadowMode;    // Are we rendering shadows (!= 0)?
uniform int iNoTextures;    // The number of use materials
uniform int iMaterial;      // The type of material

varying vec3 vNormal;     // The normal of the fragment
varying vec4 vWorldPos;   // The position of the fragment in the world

vec2 aOffsetArray[16];    // The offset array to smoothen the shadow

// Decode a float from the RGB porition of a color (taken from https://www.shadertoy.com/view/WsVGzm)
float rgbToFloat(vec3 rgb) { return rgb.r + (rgb.g / 256.0) + (rgb.b / 65536.0); }

// Test the fragment agains the shadow map (return 0 if the fragment is not inside a shadow)
vec4 shadowMapTest(vec2 vPosition, float fDistance) {
  vec4 vDepth = texture2D(tShadow, vec2(vPosition));
  float fDepthShadow = (fMaxDepth * rgbToFloat(vDepth.rgb));
  
  if (fDistance > fDepthShadow) return vec4(0.0, 0.0, 0.0, 1.0);
  
  // Do we render the transparent shadows as well?
  if (iShadowMode > 1) {
    vDepth = texture2D(tShadow2, vec2(vPosition));
    fDepthShadow = (fMaxDepth * rgbToFloat(vDepth.rgb));
    
    if (fDistance > fDepthShadow) {
      if (iShadowMode > 2) {
        return texture2D(tShadow3, vec2(vPosition));
      }
      else {
        return vec4(0.0, 0.0, 0.0, 0.5);
      }
    }
  }
  
  return vec4(0.0, 0.0, 0.0, 0.0);
}

void main(void) {
  // Initialize th eoffset array
  aOffsetArray[ 0] = vec2( 0.0,  0.0);
  aOffsetArray[ 1] = vec2( 0.0,  1.0);
  aOffsetArray[ 2] = vec2( 1.0,  1.0);
  aOffsetArray[ 3] = vec2(-1.0, -1.0);
  aOffsetArray[ 4] = vec2(-2.0,  0.0);
  aOffsetArray[ 5] = vec2( 0.0, -2.0);
  aOffsetArray[ 6] = vec2( 2.0, -2.0);
  aOffsetArray[ 7] = vec2(-2.0,  2.0);
  aOffsetArray[ 8] = vec2( 3.0,  0.0);
  aOffsetArray[ 9] = vec2( 0.0,  3.0);
  aOffsetArray[10] = vec2( 3.0,  3.0);
  aOffsetArray[11] = vec2(-3.0, -3.0);
  aOffsetArray[12] = vec2(-4.0,  0.0);
  aOffsetArray[13] = vec2( 0.0, -4.0);
  aOffsetArray[14] = vec2( 4.0, -4.0);
  aOffsetArray[15] = vec2(-4.0,  4.0);

  // Calculate the shadowing from the light direction
  float fLight = clamp(dot(vLight, vNormal), 0.3, 1.0);
  
  // Initialize the darkening factor
  float fFactor = fLight;
  vec4 vShadowColor = vec4(0.0, 0.0, 0.0, 0.0);
  
  // Do we render shadows?
  if (iShadowMode > 0) {
    // The distance of the current fragment from the light source
    float fDepthScene  = gl_TexCoord[2].z;
    
    // Calculate the amount of shadow obscuring the current fragment shadow
    for (int i = 0; i < 16; i++) {
      vec4 vTest = shadowMapTest(gl_TexCoord[1].xy + aOffsetArray[i] / 4096.0, fDepthScene);
      float fAlphaNew = vTest.a / 25.0;
      float fAlphaOld = vShadowColor.a;
      vShadowColor = mix(vShadowColor, vTest, 0.5);
      vShadowColor.a = fAlphaNew + fAlphaOld;
    }
  }
  
  // The color from the first texture (always used)
  vec4 vTextureOne = texture2D(tTexture1, vec2(gl_TexCoord[0]));
  
  // Do we use more than one texture?
  if (iNoTextures > 1) {
    // The position of the color to take from the second texture
    vec2 vTwoPos;
    
    // If we are in a non-vertical section
    if (fVertical == 0.0) {
      // We just calculate the position in the second texture from the world x and z coordinates (+y for beauty)
      vTwoPos = vec2(12.0 * (vWorldPos.x + vWorldPos.y), 12.0 * (vWorldPos.z + vWorldPos.y));
    }
    else {
      // If we are vertical we use the min Y value of the object instead of the world position Y ...
      vTwoPos = vec2(12.0 * (vWorldPos.x + fMinY), 12.0 * (vWorldPos.z + fMinY));
      
      // ... and add some additional offset to the texture's Y coordinate depending on the normal of the fragment
      if (vNormal.x == 0.0) {
        vTwoPos.y += 12.0 * (vWorldPos.y - fMinY);
      }
      else {
        vTwoPos.x += 12.0 * (vWorldPos.y - fMinY);
      }
    }
    
    vTextureOne = mix(vTextureOne, vShadowColor, min(fFactor, vShadowColor.a));
    
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
      vec4 vTextureThree = texture2D(tTexture3, vec2(gl_TexCoord[0]));
      
      // Again we want this texture to glow a little in the dark shadow
      if (vTextureThree.a > 0.75) {
        fFactor = max(fFactor, 0.75);
      }
      
      // Update the texture
      vTextureOne = vTextureThree.a * vTextureThree + (1.0 - vTextureThree.a) * vTextureOne;
    }
  }
  else {
    if (iMaterial == 3) {
      // Marbles are also supposed to glow a little in the dark (Material 3)
      fFactor = max(fFactor, 0.75);
      
      if (vShadowColor.a > fFactor)
        fFactor = vShadowColor.a;
        
      vTextureOne = mix(vShadowColor, vTextureOne, fFactor);
    }
    else {
      vTextureOne = mix(vTextureOne, vShadowColor, min(fFactor, vShadowColor.a));
    }
  }
  
  // Set the calculated fragment color
  gl_FragColor = fFactor * vTextureOne;
}
