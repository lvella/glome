// Input
uniform vec4 transformation[2];

vec4 quat_rotate(in vec4 rotation[2], in vec4 p);

vec4 to_4d_eye(in vec4 v)
{
	return quat_rotate(transformation, v);
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
