varying float fog_coord;
const float zFar = 1.7;

void mix_fog(inout vec4 color)
{
	// Fog will interpolate with this color
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

	float factor = 1.0 / (1.0 - (1.0 / (1.0 + zFar)));
	float f = factor + factor / (fog_coord - (1.0 + zFar));
	color = mix(FOG_COLOR, color, f);

	/*float f = fog_coord / zFar;
	color = mix(color, FOG_COLOR, f);*/
}
