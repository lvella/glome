// Input
uniform mat4 projection;

uniform vec4 center;
uniform vec2 slerp_arc;

attribute vec4 position;

// Output
varying vec3 normal;
varying vec3 frag_pos;
varying vec3 direction;
varying float fog_coord;

// External functions
vec4 to_3d_eye(in vec4 v);

void main()
{
	// Scale to the real size
	vec4 pos;
	pos.xyz = slerp_arc.x * position.xyz;
	pos.w = -slerp_arc.y;

	pos = to_3d_eye(pos);

	normal = normalize(pos.xyz - center.xyz);
	frag_pos = normalize(pos.xyz);

	gl_Position = projection * pos;
	fog_coord = gl_Position.w;

	direction = position.xyz;
}
