#version 150 core

// Input
uniform mat4 projection;
uniform vec4 transformation[2];
uniform vec4 old_transform[2];

in vec4 position;
in uint endpoint;

// Output
out vec4 v_color;
out float fog_coord;

// External functions
vec4 proj3d(in vec4 v);
vec4 quat_rotate(in vec4 rotation[2], in vec4 p);

void main()
{
  vec4 tmp = quat_rotate(
    endpoint != 0u ? transformation : old_transform,
    position
  );

  gl_Position = projection * proj3d(tmp);
  fog_coord = gl_Position.w * 6.0;
  v_color = vec4(1.0, 1.0, 1.0, 1.0);
}
