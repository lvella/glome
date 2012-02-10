varying vec4 v_color;
varying vec3 normal;

void mix_fog(inout vec4 color);

void main()
{
	vec3 norm = normalize(normal);
	vec4 color;
	
	color = (norm.z < 0.1) ? vec4(1.0, 1.0, 0.3, 1.0) : vec4(0.5, 0.0, 0.0, 1.0);
	color = v_color * color;
	mix_fog(color);
	gl_FragColor = color;
}
