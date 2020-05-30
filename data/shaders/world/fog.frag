#version 150 core

in float fog_coord;
uniform float zFar;

float fog_alpha();

void mix_fog(inout vec4 color)
{
	// Fog will interpolate with this color
	vec4 fog_color = vec4(0.0, 0.0, 0.0, fog_alpha());

	float f = fog_coord / zFar;
	color = mix(color, fog_color, f);
}
