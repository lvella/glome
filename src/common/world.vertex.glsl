// Input
uniform mat4 modelview;
//uniform mat4 projection;
attribute vec4 position;

// Output
varying vec4 color;
varying float fog_coord;

void main()
{
  vec4 tmp = modelview * position;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;
  gl_Position = gl_ProjectionMatrix * tmp;
  color = gl_Color;
  fog_coord = gl_Position.z;
}
