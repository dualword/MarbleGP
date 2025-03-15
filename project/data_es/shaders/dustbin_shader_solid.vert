/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#version 100
// The vertex shader for solid materials

/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;

uniform mat4 mWorldViewProj;    // The world view projection matrix
uniform mat4 mWorld;            // The world matrix
uniform mat4 mLightMatrix;      // The light matrix
uniform vec3 vLight;            // The light direction
uniform float fMaxDepth;        // The maximum depth of the light
uniform int iRttSize;           // The size of the shadow map

varying vec3 vNormal;     // The normal of the vertex
varying vec4 vWorldPos;   // The world position

varying vec4 vTextureCoord0;

void main() 
{
  gl_Position = mWorldViewProj * vec4(inVertexPosition, 1.0);
  gl_PointSize = 1.0;
  
  vWorldPos  = vec4(inVertexPosition, 1.0);
  
  // Calculate the normal
  vNormal = mat3(mWorld) * inVertexNormal;

  vTextureCoord0 = vec4(inTexCoord0, 1.0, 1.0);
}