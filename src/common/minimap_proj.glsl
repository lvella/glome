uniform bool proj_only;

void main()
{
  const float two_by_pi = 0.63661977236758134307553505349005744813783858296183;
  const vec4 center = vec4(0.0, 0.0, 0.0, -1.0);
  const vec4 pale_color = vec4(0.25, 0.65, 0.25, 1.0);
  vec4 vert;
  float r;

  vert = proj_only ? gl_Vertex : center;
  vert = gl_ModelViewMatrix * vert;
  vert.xyz = vert.xyz / (1.0 - vert.w);
  r = length(vert.xyz);
  r = (atan(r) * two_by_pi)/r;
  vert.xyz = vert.xyz * r;
  vert.w = 1.0;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  if(!proj_only)
  {
    gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;
    vert.xy = vert.xy + gl_Vertex.xy * 0.05;
  } 

  gl_FrontColor = pale_color + (gl_Color - pale_color) * (vert.z + 1.0) * 0.5;
  gl_Position = gl_ProjectionMatrix * vert;
}
