void main()
{
  float pi = 3.1415926535897932384626433832795029;
  vec4 tmp = gl_ModelViewMatrix * gl_Vertex;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  float r = length(tmp.xyz);
  r = (atan(r) * 2 / pi)/r;
  tmp.xyz = tmp.xyz * r;
  tmp.w = 1.0;
  gl_Position = gl_ProjectionMatrix * tmp;
  gl_FrontColor = gl_Color;
  gl_FogFragCoord = length(gl_Position);
}