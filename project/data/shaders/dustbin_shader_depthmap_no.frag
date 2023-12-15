/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// This is the fragment shader for materials that don't cast shadows
varying vec4 vPosition;
uniform float fMaxDepth;

void main(void) {
  // The color is always white, i.e. far away from the light source
  gl_FragColor = vec4(1.0, 1.0, 1.0, 0.0);
}
