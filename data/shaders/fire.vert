uniform mat4 transform;
uniform mat4 projection;

attribute vec4 position;
attribute vec4 color;
attribute float size;

varying vec4 v_color;
varying float fog_coord;

void main()
{
  vec4 tmp = transform * position;
  
  // Ortographic projection to 3-D
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;

  gl_Position = projection * tmp;
  fog_coord = gl_Position.z;

  //TODO: Correct 4D perspective
  gl_PointSize = size;

  v_color = color;
}
