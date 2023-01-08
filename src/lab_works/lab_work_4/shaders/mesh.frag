#version 450

in vec3 oNormal;
in vec3 oFragPos;

layout( location = 0 ) out vec4 fragColor;

uniform vec3 uColorAmbiant;
uniform vec3 uColorDiffuse;
uniform vec3 uColorSpecular;
uniform float uFvShininess;
uniform vec3 uLightPos;

void main()
{
	vec3 Normal = normalize(oNormal);
	vec3 FragPos = oFragPos;
	vec3 LightDir = normalize(uLightPos - FragPos);
	vec3 SpecReflection;
	float cosDiffuse;
	float cosSpecular;

	cosDiffuse = dot(Normal, LightDir);

	if ( cosDiffuse < 0 ) {
		Normal = Normal * -1;
		cosDiffuse = max(dot(Normal, LightDir), 0.0);
	}

	SpecReflection = normalize(reflect(LightDir, Normal));
	cosSpecular = max( dot( SpecReflection, -LightDir ), 0.0);
	
	fragColor = vec4( uColorAmbiant + uColorDiffuse * cosDiffuse + uColorSpecular * pow(cosSpecular, uFvShininess) , 1. );
}
