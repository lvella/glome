#version 150 core

// Input
uniform mat4 projection;

in vec4 position;
in vec4 color;

// Output
out vec4 v_color;
out float fog_coord;

// External function:
vec4 to_3d_eye(in vec4 v);

void main()
{
  vec4 pos = to_3d_eye(position);

  gl_Position = projection * pos;
  fog_coord = gl_Position.w;

  v_color = color;
}
