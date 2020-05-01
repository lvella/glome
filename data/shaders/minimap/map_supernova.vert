attribute vec4 position;
attribute vec4 color;

uniform vec2 slerp_arc;

varying vec4 v_color;

vec3 project_vertex(vec4 vert);
vec4 color_blend(vec4 color, float z);

void main()
{
	// Scale to the real size
	vec4 tmp = slerp_arc.x * position;
	tmp.w = -slerp_arc.y;

 	vec3 v = project_vertex(tmp);

	gl_Position = vec4(v.xy, -v.z, 1.0);

	v_color = color_blend(color, v.z);
}
