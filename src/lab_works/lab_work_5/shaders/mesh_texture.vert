#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

out vec3 oNormal;
out vec3 oFragPos;
out vec2 oTexCoord;
out vec3 oPtangent;
out vec3 oLtangent;

uniform mat4 uMMatrix;	// Model Matrix
uniform mat4 uVMatrix;	// View Matrix
uniform mat4 uMVMatrix; // View * Model mtx
uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uNormalMatrix; // normal mtx calculated on the cpu
uniform vec3 uVLightPos;

void main()
{
	// TBN in viewspace
	vec3 vT = normalize(vec3( uVMatrix * vec4(aVertexTangent, 0.f)));
	vec3 vB = normalize(vec3( uVMatrix * vec4(aVertexBitagent, 0.f)));
	vec3 vN = normalize(vec3( uVMatrix * vec4(aVertexNormal, 0.f)));
	mat3 oInv_TBN = transpose(mat3(vT, vB, vN));

	// Point and Light coord in tangent space
	oLtangent = oInv_TBN * uVLightPos;
	oPtangent = oInv_TBN * vec3(uVMatrix * uMMatrix *vec4(aVertexPosition, 1.0));

	// 
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
	oNormal = ( mat3(uNormalMatrix) * aVertexNormal );
	oFragPos = vec3 (uMVMatrix * vec4( aVertexPosition, 1.f ));
	oTexCoord = aVertexTexCoords;
}
