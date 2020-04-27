// Input
uniform mat4 transform;
uniform mat4 old_transform;
uniform mat4 projection;

attribute float endpoint;
attribute vec4 position;

// Output
varying vec4 v_color;
varying float fog_coord;

void main()
{
  vec4 tmp;
  if(endpoint > 0.5) {
    tmp = transform * position;
  } else {
    tmp = old_transform * position;
  }

  // Ortographic projection to 3-D
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;

  gl_Position = projection * tmp;
  fog_coord = gl_Position.w * 6.0;
  v_color = vec4(1.0, 1.0, 1.0, 1.0);
}
