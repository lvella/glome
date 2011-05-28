void proj(inout vec4 v)
{
  v = gl_ModelViewMatrix * v;
  v = vec4(v.xyz / (1.0 - v.w), 1.0);
}

void main()
{
  vec4 zero = vec4(0.0, 0.0, 0.0, -1.0);

  float x = 0.002 * gl_Vertex.x;
  vec4 side = vec4(x, 0.0, 0.0, -sqrt(1.0 - x*x));

  float z = 0.005 * gl_Vertex.y;
  vec4 front = vec4(0.0, 0.0, z, -sqrt(1.0 - z*z));

  proj(zero);
  proj(side);
  proj(front);

  float side_len = length(side);
  side = vec4(normalize(cross(front.xyz, zero.xyz)) * side_len, 0.0);
  front = front + side;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;

  gl_FrontColor = gl_Color;
  gl_Position = gl_ProjectionMatrix * front;
}
