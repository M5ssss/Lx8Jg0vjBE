#version 450

in vec3 oNormal;
in vec3 oFragPos;
in vec2 oTexCoord;
in vec3 oPtangent;
in vec3 oLtangent;

layout( location = 0 ) out vec4 fragColor;
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
	vec3 vLightDir = normalize(oLtangent - vFragPos);
	vec3 vViewDir = normalize(-vFragPos);
	vec3 vSpecReflection;

	float flCosDiffuse;
	float flCosSpecular;
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


	// diffuse factor & normal flip
	flCosDiffuse = max(dot(vNormal, vLightDir), 0.0);


	// specular factor
	vSpecReflection = reflect(-vLightDir, vNormal);
	flCosSpecular = max( dot( vViewDir, vSpecReflection ), 0.0);

	// mega mix
	fragColor = vec4( vColorAmbiant + vColorDiffuse * flCosDiffuse + vColorSpecular * pow(flCosSpecular, vFvShininess) , flTransparency );
}
