/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// The vertex shader for solid materials

uniform mat4 mLightMatrix;    // The world view projection matrix

void main() 
{
  gl_Position = mLightMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
}