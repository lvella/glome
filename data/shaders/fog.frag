varying float fog_coord;
uniform float zFar;

void mix_fog(inout vec4 color)
{
	// Fog will interpolate with this color
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

	float f = fog_coord / zFar;
	color = mix(color, FOG_COLOR, f);
}
