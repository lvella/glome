void proj(inout vec4 v)
{
  v = gl_ModelViewMatrix * v;
  v = vec4(v.xyz / (1.0 - v.w), 1.0);
}

void main()
{
  vec4 origin = vec4(0.0, 0.0, 0.0, -1.0);

  float z = 0.007 * gl_Vertex.y;
  vec4 front = vec4(0.0, 0.0, z, -sqrt(1.0 - z*z));

  float x = 0.002 * gl_Vertex.x;
  vec4 side = vec4(x, 0.0, 0.0, -sqrt(1.0 - x*x));

  proj(origin);
  proj(front);
  proj(side);
  float side_len = length(side.xyz - origin.xyz);

  vec3 dfront = front.xyz - origin.xyz;
  vec3 dside = cross(dfront, origin.xyz);
  vec3 dup;
  
  float vec_len = length(dside);
  // Due some empirical testing, this is satisfactory...
  if(vec_len > 0.000000001) {
    dside /= vec_len;
  } else {
    dside = vec3(0.0, gl_Vertex.x, 0.0);
  }

  dup = normalize(cross(dside, dfront));

  if(dfront.z * gl_Vertex.y < 0.0)
    dup = -dup;

  float displace_size = side_len * gl_Vertex.y;

  dside *= displace_size * gl_Vertex.x;
  dup *= displace_size;

  origin.xyz += dfront + dside + dup;
  gl_Position = gl_ProjectionMatrix * origin;
  gl_FogFragCoord = gl_Position.z;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[0].st = (gl_Vertex.xy + 1.0) * 0.5;

  gl_FrontColor = gl_Color;
}
