//varying vec4 v_color;
varying vec3 frac_coord;
varying float fog_coord;

void main()
{
	int i;
	vec2 c, z;
	vec3 norm;
	
	norm = normalize(frac_coord);
	z = c = vec2(frac_coord.xy / (1.0 - frac_coord.z));

	for(i=0; i < 20; i++) {
		float x = (z.x * z.x - z.y * z.y) + c.x;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 4.0)
			discard;
        	
		z.x = x;
		z.y = y;
	}

	// Fog will interpolate with this color
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1.0);

	// Exponential curve constant, based on the fixed pipeline fog
	const float FOG_FACTOR = -1.20; // aprox. the same as -(0.83 / log(2))
	//TODO: Simple use gl_FragCoord.z
	float f = exp2(FOG_FACTOR * fog_coord);
	gl_FragColor = mix(FOG_COLOR, vec4(1.0, 1.0, 1.0, 1.0), f);
}
