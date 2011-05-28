void proj(inout vec4 v)
{
  v = gl_ModelViewMatrix * v;
  v = vec4(v.xyz / (1.0 - v.w), 1.0);
}

void main()
{
  vec4 origin = vec4(0.0, 0.0, 0.0, -1.0);

  float x = 0.003 * gl_Vertex.x;
  vec4 side = vec4(x, 0.0, 0.0, -sqrt(1.0 - x*x));

  /*float z = 0.005 * gl_Vertex.y;
  vec4 front = vec4(0.0, 0.0, z, -sqrt(1.0 - z*z));
  
  //projs...
  
  side = vec4(normalize(cross(front.xyz, origin.xyz)) * side_len, 0.0);
  front = front + side;*/

  proj(origin);
  proj(side);

  float side_len = length(side - origin);

  origin = origin + vec4(side_len * gl_Vertex.x, side_len * gl_Vertex.y, 0.0, 0.0);
  gl_Position = gl_ProjectionMatrix * origin;
  gl_FogFragCoord = gl_Position.z;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;

  gl_FrontColor = gl_Color;
}
