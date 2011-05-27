void main()
{
  float z = 0.005 * gl_Vertex.y;
  vec4 vert = vec4(0.0, 0.0, z, -sqrt(1.0 - z));

  vert = gl_ModelViewMatrix * vert;
  vert = vec4(vert.xyz / (1.0 - vert.w), 1.0);

  gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;
    vert.xy = vert.xy + gl_Vertex.xy * 0.05;

  gl_FrontColor = pale_color + (gl_Color - pale_color) * (vert.z + 1.0) * 0.5;
  gl_Position = gl_ProjectionMatrix * vert;
}
