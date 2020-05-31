#version 150 core

vec4 quat_mul(in vec4 a, in vec4 b)
{
	return vec4(
		a.x*b.x - a.y*b.y - a.z*b.z - a.w*b.w,
		a.x*b.y + a.y*b.x + a.z*b.w - a.w*b.z,
		a.x*b.z - a.y*b.w + a.z*b.x + a.w*b.y,
		a.x*b.w + a.y*b.z - a.z*b.y + a.w*b.x
	);
}

vec4 quat_rotate(in vec4 rotation[2], in vec4 p)
{
	return quat_mul(quat_mul(rotation[0], p), rotation[1]);
}
