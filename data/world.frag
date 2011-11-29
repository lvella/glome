varying vec4 v_color;
varying vec2 v_texcoord;
varying float fog_coord;

uniform sampler2D texbase;
uniform bool has_tex;

// TODO: put this function in a separated place
void get_texel(inout vec4 color)
{
	if(has_tex) {
		vec4 texel = texture2D(texbase, v_texcoord);
		if(texel.a < 0.004)
			discard;

		color = color * texel;
	}
}

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
