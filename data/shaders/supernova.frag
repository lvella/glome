varying vec4 v_color;
varying vec3 frac_coord;

void mix_fog(inout vec4 color);

void main()
{
	int i;
	vec2 c, z;
	vec3 norm;
	
	norm = normalize(frac_coord);
	z = c = norm.xy / (1.0 - norm.z);

	for(i=0; i < 20; i++) {
		float x = (z.x * z.x - z.y * z.y) + c.x;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > 4.0)
			discard;
        	
		z.x = x;
		z.y = y;
	}

	vec4 color = v_color;
	mix_fog(color);
	gl_FragColor = color;
}
