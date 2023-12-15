/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// The vertex shader for the shadow map

uniform mat4 mLightMatrix;  // The matrix of the light camera
uniform float fMaxDepth;    // The maximum distance of the light

varying vec4 vPosition;   // The world position

void main()
{
  vPosition = mLightMatrix * gl_Vertex;   // Calculate the world position
  
	gl_Position = vPosition;  // Update the GL variable
  
  // The first texture coordinate is interpolated by GLSL and used
  // to transfer the depth of the vertex to the fragment shader
  gl_TexCoord[0] = vec4(fMaxDepth, 0, vPosition.z, fMaxDepth);
  
  // The coordinate in the normal texture
  gl_TexCoord[1].xy = gl_MultiTexCoord0.xy;
}