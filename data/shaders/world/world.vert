// Input
uniform mat4 projection;

attribute vec4 position;
attribute vec4 color;

// Output
varying vec4 v_color;
varying float fog_coord;

// External function:
vec4 to_3d_eye(in vec4 v);

void main()
{
  vec4 pos = to_3d_eye(position);

  gl_Position = projection * pos;
  fog_coord = gl_Position.w;

  v_color = color;
}
