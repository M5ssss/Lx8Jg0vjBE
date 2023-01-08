#version 450
layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec4 aVertexColor;

out vec4 eVertexColor;

uniform float uTranslationX;
uniform float uTranslationY;

void main() {
	gl_Position = vec4(aVertexPosition.x + uTranslationX, aVertexPosition.y + uTranslationY, 0, 1);
	eVertexColor = aVertexColor;
}
