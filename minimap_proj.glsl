void main()
{
  const float pi = 3.1415926535897932384626433832795029;
  const vec4 center = vec4(0.0, 0.0, 0.0, -1.0);
  vec4 tmp;
  float r;

  tmp = gl_ModelViewMatrix * center;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  r = length(tmp.xyz);
  r = (atan(r) * 2.0 / pi)/r;
  tmp.xyz = tmp.xyz * r;
  tmp.w = 1.0;

  gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) / 2.0;

  tmp.xy = tmp.xy + gl_Vertex.xy / 20.0;

  gl_Position = gl_ProjectionMatrix * tmp;
  gl_FrontColor = gl_Color;
}
