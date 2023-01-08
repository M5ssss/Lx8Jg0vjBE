#version 450


layout( location = 0 ) out vec4 fragColor;

layout( binding = 0 ) uniform sampler2D   fragPos;
layout( binding = 1 ) uniform sampler2D   fragNorm;
layout( binding = 2 ) uniform sampler2D   fragAmb;
layout( binding = 3 ) uniform sampler2D   fragDiff;
layout( binding = 4 ) uniform sampler2D   fragSpec;

void main() {

	ivec2 uvCoord	= ivec2(gl_FragCoord.xy);
	vec4 vFragPos	= texelFetch(fragPos,	uvCoord, 0);
	vec4 vFragNorm	= texelFetch(fragNorm,	uvCoord, 0);
	vec4 vFragAmb	= texelFetch(fragAmb,	uvCoord, 0);
	vec4 vFragDiff	= texelFetch(fragDiff,	uvCoord, 0);
	vec4 vFragSpec	= texelFetch(fragSpec,	uvCoord, 0);


	vec3 oLtangent = vec3(vFragPos.w, vFragNorm.w, vFragAmb.w);
	vec3 vLightDir = normalize(oLtangent - vFragPos.xyz);
	vec3 vViewDir = normalize(-vFragPos.xyz);
	vec3 vSpecReflection;


	float flCosDiffuse;
	float flCosSpecular;
	float flTransparency = vFragDiff.w;

	// diffuse factor & normal flip
	flCosDiffuse = max(dot(vFragNorm.xyz, vLightDir), 0.0);


	// specular factor
	vSpecReflection = reflect(-vLightDir, vFragNorm.xyz);
	flCosSpecular = max( dot( vViewDir, vSpecReflection ), 0.0);
	
	// mega mix
	fragColor = vec4( vFragAmb.rgb + vFragDiff.rgb * flCosDiffuse + vFragSpec.rgb * pow(flCosSpecular, vFragSpec.w) , flTransparency );

}