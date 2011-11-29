varying vec4 v_color;
varying float fog_coord;

void get_texel(inout vec4 color);

void main()
{
	vec4 color = v_color;

	get_texel(color);

	// Fog will interpolate with this color
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

	// Exponential curve constant, based on the fixed pipeline fog
	const float FOG_FACTOR = -1.20; // aprox. the same as -(0.83 / log(2))

	float f = exp2(FOG_FACTOR * fog_coord);
	gl_FragColor = mix(FOG_COLOR, color, f);
}
