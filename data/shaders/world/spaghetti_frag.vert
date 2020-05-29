#version 150 core

// Input
uniform mat4 projection;

in vec4 position;
in vec4 color;
in float center_dist;

// Output
out vec4 v_color;
out float fog_coord;
noperspective out float c_dist;

// External function:
vec4 to_3d_eye(in vec4 v);

void main()
{
  c_dist = center_dist;

  vec4 pos = to_3d_eye(position);

  gl_Position = projection * pos;
  fog_coord = gl_Position.w;

  v_color = color;
}
