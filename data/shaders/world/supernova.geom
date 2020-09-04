#version 150 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Input
uniform mat4 projection;
uniform vec4 center;

in vec4 pos_3d[];
in vec3 vdirection[];

// Output
out vec3 normal;
out vec3 frag_pos;
out vec3 direction;
out float fog_coord;

float sq_distance(vec4 a, vec4 b)
{
	vec4 delta = a - b;
	return dot(delta, delta);
}

void main()
{
	if(
		sq_distance(pos_3d[0], pos_3d[1]) > 9.0 ||
		sq_distance(pos_3d[1], pos_3d[2]) > 9.0 ||
		sq_distance(pos_3d[2], pos_3d[0]) > 9.0
	) {
		return;
	}

	uint i;
	for(i = 0u; i < 3u; ++i) {
		vec4 pos = pos_3d[i];
		normal = pos.xyz - center.xyz;
		frag_pos = pos.xyz;

		gl_Position = projection * pos;
		fog_coord = gl_Position.w;

		direction = vdirection[i];

		EmitVertex();
	}
	EndPrimitive();
}
