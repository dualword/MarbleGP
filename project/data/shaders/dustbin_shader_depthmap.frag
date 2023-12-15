/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// The fragment shader for the shadow map
varying vec4 vPosition;
uniform float fMaxDepth;

// Encode a float in the RGB porition of a color (taken from https://www.shadertoy.com/view/WsVGzm)
vec3 floatToRgb(float v) {
  float r = v;
  float g = mod(v * 256.0, 1.0);
  r-= g / 256.0;
  float b = mod(v * 65536.0, 1.0);
  g -= b / 256.0;
  return vec3(r,g,b);
}

void main(void) {
  vec4 vInfo = gl_TexCoord[0];
  float fDepth = vPosition.z / fMaxDepth;
  vec3 vDepthRGB = floatToRgb(vPosition.z / fMaxDepth);
  
  gl_FragColor = vec4(vDepthRGB.r, vDepthRGB.g, vDepthRGB.b, 1.0);
}
