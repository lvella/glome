varying vec4 color;
varying float fog_coord;

void main()
{
  vec4 tmp = gl_ModelViewMatrix * gl_Vertex;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;
  gl_Position = gl_ProjectionMatrix * tmp;
  color = gl_Color;
  fog_coord = gl_Position.z;
}
