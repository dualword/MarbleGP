/* Attributes */

attribute vec3 inVertexPosition;
attribute vec3 inVertexNormal;
attribute vec4 inVertexColor;
attribute vec2 inTexCoord0;
attribute vec2 inTexCoord1;

/* Uniforms */

uniform mat4 mWorldViewProj;
uniform mat4 uWVMatrix;
uniform mat4 uNMatrix;
uniform mat4 uTMatrix0;
uniform mat4 uTMatrix1;
uniform mat4 mWorld;

/* Varyings */

varying vec2 vTextureCoord0;
varying vec2 vTextureCoord1;
varying vec3 vNormal;
varying vec4 worldPos;

void main()
{
	gl_Position = mWorldViewProj * vec4(inVertexPosition, 1.0);
	gl_PointSize = 1.0;
  
  worldPos = vec4(inVertexPosition, 1.0);
  vNormal = mat3(mWorld) * inVertexNormal;

	vec4 TextureCoord0 = vec4(inTexCoord0.x, inTexCoord0.y, 1.0, 1.0);
	vTextureCoord0 = vec4(uTMatrix0 * TextureCoord0).xy;

	vec4 TextureCoord1 = vec4(inTexCoord1.x, inTexCoord1.y, 1.0, 1.0);
	vTextureCoord1 = vec4(uTMatrix1 * TextureCoord1).xy;
}
