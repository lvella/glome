// Input
uniform mat4 transformation;

vec4 to_4d_eye(in vec4 v)
{
	return transformation * v;
}

// Maps a 4-D unit vector to euclidian 3-D space
// via stereographic projection.
vec4 proj3d(in vec4 v)
{
	return vec4(v.xyz / (1.0 - v.w), 1.0);
}

vec4 to_3d_eye(in vec4 v)
{
	return proj3d(to_4d_eye(v));
}
