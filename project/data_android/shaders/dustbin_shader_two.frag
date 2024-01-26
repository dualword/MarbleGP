precision mediump float;

/* Uniforms */

uniform int uTextureUsage0;
uniform sampler2D uTextureUnit0;
uniform sampler2D HexagonTexture;
uniform vec3 vLight;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec4 vVertexColor;
varying vec3 vNormal;
varying vec4 worldPos;

void main()
{
  float factor = clamp(dot(vNormal, vLight), 0.5, 1.0);
  
	vec4 Color    = texture2D(uTextureUnit0, vTextureCoord0);
  vec4 hexagon  = texture2D(HexagonTexture, vec2(12.0 * (worldPos.x + worldPos.y), 12.0 * (worldPos.z + worldPos.y)));
  vec4 theColor = hexagon.a * hexagon + (1.0 - hexagon.a) * Color;
  
	gl_FragColor = factor * theColor;
}
