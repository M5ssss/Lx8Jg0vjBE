#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

out vec3 oNormal;
out vec3 oFragPos;

uniform mat4 uMVMatrix;
uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uNormalMatrix; // normal mtx calculated on the cpu

void main()
{
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
	oNormal = ( mat3(uNormalMatrix) * aVertexNormal );
	oFragPos = vec3 (uMVMatrix * vec4( aVertexPosition, 1.f ));
}
