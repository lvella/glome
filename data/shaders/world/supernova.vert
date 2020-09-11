#version 150 core

// Input
uniform vec2 slerp_arc;

in vec4 position;

// Output
out vec4 pos_3d;
out vec3 vdirection;

// External functions
vec4 to_3d_eye(in vec4 v);

void main()
{
	// Scale to the real size
	vec4 pos;
	pos.xyz = slerp_arc.x * position.xyz;
	pos.w = -slerp_arc.y;

	pos_3d = to_3d_eye(pos);

	vdirection = position.xyz;
}
