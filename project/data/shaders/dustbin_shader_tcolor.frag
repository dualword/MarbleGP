/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

// This is the fragment shader for the transparent color in the scene (for the shadows)

uniform sampler2D tTexture1;    // The texture

void main(void) {
  vec4 tColor = texture2D(tTexture1, vec2(gl_TexCoord[0]));
  gl_FragColor = tColor;
}
