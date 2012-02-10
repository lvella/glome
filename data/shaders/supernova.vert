// Input
uniform mat4 transform;
uniform mat4 projection;
uniform vec2 slerp_arc;

attribute vec4 position;
attribute vec4 color;

// Output
varying vec4 v_color;
varying vec3 frac_coord;
varying float fog_coord;

void main()
{
  vec4 tmp = slerp_arc.x * position;
  tmp.w = -slerp_arc.y;
  tmp = transform * tmp;
  
  // Ortographic projection to 3-D
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;

  gl_Position = projection * tmp;
  fog_coord = gl_Position.z;

  v_color = color;
  frac_coord = position.xyz;
}
