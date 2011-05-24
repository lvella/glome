void main()
{
  const float two_by_pi = 0.63661977236758134307553505349005744813783858296183;
  const vec4 center = vec4(0.0, 0.0, 0.0, -1.0);
  const vec4 pale_color = vec4(0.25, 0.65, 0.25, 1.0);
  vec4 tmp;
  float r;

  tmp = gl_ModelViewMatrix * center;
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  r = length(tmp.xyz);
  r = (atan(r) * two_by_pi)/r;
  tmp.xyz = tmp.xyz * r;
  tmp.w = 1.0;

  gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;

  tmp.xy = tmp.xy + gl_Vertex.xy * 0.05;

  gl_Position = gl_ProjectionMatrix * tmp;
  gl_FrontColor = pale_color + (gl_Color - pale_color) * (tmp.z + 1.0) * 0.5;
}
