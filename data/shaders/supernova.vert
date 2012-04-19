// Input
uniform mat4 transform;
uniform mat4 projection;
uniform vec4 center;
uniform vec2 slerp_arc;

attribute vec4 position;
attribute vec4 color;

// Output
varying vec3 normal;
varying float v_color;
varying vec3 frag_pos;
varying float fog_coord;
varying vec3 direction;

void main()
{
	// Scale to the real size
	vec4 tmp;
	tmp.xyz = slerp_arc.x * position.xyz;
	tmp.w = -slerp_arc.y;

	tmp = transform * tmp;
  
	// Ortographic projection to 3-D
	tmp.xyz = tmp.xyz / (1.0 - tmp.w);
	tmp.w = 1.0;

	normal = normalize(tmp.xyz - center.xyz);
	frag_pos = normalize(tmp.xyz);
	
	gl_Position = projection * tmp;
	fog_coord = gl_Position.z;
	
	direction = position.xyz;

	v_color = color.r;
}
