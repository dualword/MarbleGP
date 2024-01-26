precision mediump float;

/* Uniforms */

uniform int uTextureUsage0;
uniform sampler2D uTextureUnit0;
uniform vec3 vLight;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec3 vNormal;

void main()
{
  float factor = clamp(dot(vNormal, vLight), 0.5, 1.0);
	vec4 Color = texture2D(uTextureUnit0, vTextureCoord0);

	gl_FragColor = factor * Color;
}
