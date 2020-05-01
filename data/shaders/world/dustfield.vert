// Input
uniform mat4 old_transform;
uniform mat4 projection;

attribute float endpoint;
attribute vec4 position;

// Output
varying vec4 v_color;
varying float fog_coord;

// External functions
vec4 to_4d_eye(in vec4 v);
vec4 proj3d(in vec4 v);

void main()
{
  vec4 tmp;
  if(endpoint > 0.5) {
    tmp = to_4d_eye(position);
  } else {
    tmp = old_transform * position;
  }

  gl_Position = projection * proj3d(tmp);
  fog_coord = gl_Position.w * 6.0;
  v_color = vec4(1.0, 1.0, 1.0, 1.0);
}
