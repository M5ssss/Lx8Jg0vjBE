#version 450
in vec4 eVertexColor;

layout( location = 0 ) out vec4 eFragColor;

uniform float uBrightness;

void main() {
	eFragColor = eVertexColor;
	eFragColor *= uBrightness;
}
