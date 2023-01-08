#version 450
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec4 aVertexColor;

out vec4 eVertexColor;

uniform float uTranslationX;
uniform float uTranslationY;
uniform mat4 uMtxModel;
uniform mat4 uMtxView;
uniform mat4 uMtxProj;
uniform mat4 uMtxClip;

void main() {
	vec4 mtx_Transformed;

	mtx_Transformed = uMtxModel * vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y + uTranslationY, aVertexPosition.z, 1);
	gl_Position = uMtxProj * uMtxView * mtx_Transformed;
	gl_Position = uMtxClip * vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y + uTranslationY, aVertexPosition.z, 1);
	eVertexColor = aVertexColor;
}
