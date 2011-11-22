// Input
uniform mat4 transform;
uniform mat4 camera;
uniform mat4 projection;

attribute vec4 position;
attribute vec4 color;

// Output
varying vec4 frag_color;
varying float fog_coord;

void main()
{
  vec4 tmp = camera * (transform * position);
  
  // Ortographic projection to 3-D
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;

  gl_Position = projection * tmp;
  fog_coord = gl_Position.z;

  frag_color = vec4(0.0, 1.0, 0.5, 1.0);
  frag_color = color;
}
