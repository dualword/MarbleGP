uniform sampler2D myTexture;
uniform sampler2D HexagonTexture;
uniform sampler2D TopTexture;
uniform sampler2D ShadowMapSampler;

uniform vec3 vLight;

varying vec3 normalVec;
varying vec4 worldPos;

float testShadow(vec2 smTexCoord, float realDistance)
{
	vec4 texDepth = texture2D(ShadowMapSampler, vec2(smTexCoord));
	float extractedDistance = texDepth.r;

	return (extractedDistance <= realDistance) ? 0.0 : 1.0;
}

void main(void) {
  float factor = clamp(dot(normalVec, vLight), 0.5, 1.0);
  
  vec4 color    = texture2D(myTexture, vec2(gl_TexCoord[0]));
  vec4 hexagon  = texture2D(HexagonTexture, vec2(12.0 * (worldPos.x + worldPos.y), 12.0 * (worldPos.z + worldPos.y)));
  vec4 topColor = texture2D(TopTexture, vec2(gl_TexCoord[0]));
  vec4 theColor = hexagon.a * hexagon + (1.0 - hexagon.a) * color;
  
  theColor = topColor.a * topColor + (1.0 - topColor.a) * theColor;
  
  vec4 MVar = gl_TexCoord[0];
  vec4 SMPos = gl_TexCoord[1];
  
  // SMPos.xy  = SMPos.xy / SMPos.w / 2.0 + vec2(0.5, 0.5);
  vec2 clampedSMPos = clamp(SMPos.xy, vec2(0.0, 0.0), vec2(1.0, 1.0));
  
  if (clampedSMPos.x == SMPos.x && clampedSMPos.y == SMPos.y && SMPos.z > 0.0 && SMPos.z < MVar.z) {
    float realDist = MVar.x / MVar.z - 0.002;
    factor = testShadow(SMPos.xy, realDist);   
  }
  
  gl_FragColor = factor * theColor;
}
