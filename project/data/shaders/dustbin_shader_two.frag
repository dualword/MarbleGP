uniform sampler2D myTexture;
uniform sampler2D HexagonTexture;

uniform vec3 vLight;

varying vec3 normalVec;
varying vec4 worldPos;

void main(void) {
  float factor = clamp(dot(normalVec, vLight), 0.5, 1.0);
  
  vec4 color   = texture2D(myTexture, vec2(gl_TexCoord[0]));
  vec4 hexagon = texture2D(HexagonTexture, vec2(12.0 * (worldPos.x + worldPos.y), 12.0 * (worldPos.z + worldPos.y)));
  
  vec4 theColor = hexagon.a * hexagon + (1.0 - hexagon.a) * color;
  
  gl_FragColor = factor * theColor;
}
