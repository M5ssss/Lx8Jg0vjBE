#version 450

in vec3 oNormal;
in vec3 oFragPos;
in vec2 oTexCoord;
in vec3 oPtangent;
in vec3 oLtangent;

layout( location = 0 ) out vec4 fragPos;
layout( location = 1 ) out vec4 fragNorm;
layout( location = 2 ) out vec4 fragAmb;
layout( location = 3 ) out vec4 fragDiff;
layout( location = 4 ) out vec4 fragSpec;

layout( binding = 0 ) uniform sampler2D uAmbiantMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShineMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

uniform vec3 uColorAmbiant;
uniform vec3 uColorDiffuse;
uniform vec3 uColorSpecular;
uniform float uFvShininess;
uniform vec3 uFLightPos;

uniform bool uHasAmbiantMap;
uniform bool uHasDiffuseMap;
uniform bool uHasSpecularMap;
uniform bool uHasShineMap;
uniform bool uHasNormalMap;

void main()
{
	// variables	
	vec3 vNormal = vec3(0.f, 0.f, 1.f);
	vec3 vFragPos = oPtangent;
	float flTransparency = 1.f;

	vec3 vColorAmbiant;
	vec3 vColorDiffuse;
	vec3 vColorSpecular;
	float vFvShininess;

	// colors and textures
	if (uHasDiffuseMap) {
		vec4 vDiffuseTex = texture(uDiffuseMap, oTexCoord);
		vColorDiffuse = vDiffuseTex.xyz;
		flTransparency = vDiffuseTex.w;
		if (flTransparency < 0.5) {discard;}
	}
	else {vColorDiffuse = uColorDiffuse;}

	if (uHasAmbiantMap) {vColorAmbiant = texture(uAmbiantMap, oTexCoord).xyz;}
	else {vColorAmbiant = uColorAmbiant;}

	if (uHasSpecularMap) {vColorSpecular = texture(uSpecularMap, oTexCoord).xxx;}
	else {vColorSpecular = uColorSpecular;}

	if (uHasShineMap) {vFvShininess = texture(uShineMap, oTexCoord).x;}
	else {vFvShininess = uFvShininess;}

	// normal map
	if (uHasNormalMap) {
		vNormal = texture(uNormalMap, oTexCoord).xyz * 2 - 1;
		vNormal = normalize(vNormal);
	}

	// draw to gbuffer
	fragPos		= vec4(vFragPos,		oLtangent.x);
	fragNorm	= vec4(vNormal,			oLtangent.y);
	fragAmb		= vec4(vColorAmbiant,	oLtangent.z);
	fragDiff	= vec4(vColorDiffuse, 	flTransparency);
	fragSpec.rgb= vColorSpecular;
	fragSpec.a  = vFvShininess;
}
