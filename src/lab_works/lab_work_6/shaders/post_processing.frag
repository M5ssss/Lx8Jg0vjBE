#version 450


layout( location = 0 ) out vec4 fragColor;
layout( binding = 0 ) uniform sampler2D pixelColor;

void main() {
	vec2 vScreenSize	= textureSize(pixelColor, 0);
	vec2 vScreenCenter	= vScreenSize * 0.5;

	vec2 vDeltaDirection = vec2(vScreenCenter.x - gl_FragCoord.x, vScreenCenter.y - gl_FragCoord.y);
	float flDistanceToCenter = sqrt(pow(vDeltaDirection.x,2) + pow(vDeltaDirection.y,2)) * 0.0006;
	vec2 vRedCoord		= ( gl_FragCoord.xy + vDeltaDirection * (flDistanceToCenter * 1) )   / vScreenSize;
	vec2 vGreenCoord	= ( gl_FragCoord.xy + vDeltaDirection * (flDistanceToCenter * 1.02) )/ vScreenSize;
	vec2 vBlueCoord		= ( gl_FragCoord.xy + vDeltaDirection * (flDistanceToCenter * 1.04) )/ vScreenSize;

	float vFragColorRed		= texture(pixelColor, vRedCoord).r;
	float vFragColorGreen	= texture(pixelColor, vGreenCoord).g;  
	float vFragColorBlue	= texture(pixelColor, vBlueCoord).b;

	// mega mix
	fragColor = vec4( vFragColorRed, vFragColorGreen, vFragColorBlue, 1.f );

}