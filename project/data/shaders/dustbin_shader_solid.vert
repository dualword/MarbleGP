/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// The vertex shader for solid materials

uniform mat4 mWorldViewProj;    // The world view projection matrix
uniform mat4 mWorld;            // The world matrix
uniform mat4 mLightMatrix;      // The light matrix
uniform vec3 vLight;            // The light direction
uniform float fMaxDepth;        // The maximum depth of the light
uniform int iRttSize;           // The size of the shadow map

varying vec3 vNormal;     // The normal of the vertex
varying vec4 vShadowPos;  // The position in the shadow map
varying vec4 vWorldPos;   // The world position

void main() 
{
  gl_Position = mWorldViewProj * gl_Vertex;
  
  // Calculate the position in the shadow map
  vShadowPos = mLightMatrix * gl_Vertex;
  vWorldPos  = gl_Vertex;
  
  // Calculate the normal
  vNormal = mat3(mWorld) * gl_Normal;
  // gl_Normal = vNormal;

  // Update the texture coordinates
  // 0: the position in the texure
  // 1: the position in the shadow map
  // 2: the shadow position
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = mLightMatrix * gl_Vertex;
  gl_TexCoord[1] = gl_TexCoord[1] / gl_TexCoord[1].w;
  gl_TexCoord[1] = (gl_TexCoord[1] + 1.0) * 0.5;
  gl_TexCoord[2] = vec4(vShadowPos.x, vShadowPos.y, vShadowPos.z, iRttSize);
}