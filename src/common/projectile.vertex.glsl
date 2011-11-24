// Input
uniform mat4 transform;
uniform mat4 camera;
uniform mat4 projection;

attribute vec4 position;
attribute vec4 color;

// Output
varying vec4 v_color;
varying vec2 v_texcoord;
varying float fog_coord;

// Simple globals
mat4 modelview = camera * transform;

void proj(inout vec4 v)
{
  v = modelview * v;
  v = vec4(v.xyz / (1.0 - v.w), 1.0);
}

void main()
{
  vec4 origin = vec4(0.0, 0.0, 0.0, -1.0);

  float z = 0.004 * position.y;
  vec4 front = vec4(0.0, 0.0, z, -sqrt(1.0 - z*z));

  float x = 0.001 * position.x;
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

  if(dfront.z * position.y < 0.0)
    dup = -dup;

  float displace_size = side_len * position.y;

  dside *= displace_size * position.x;
  dup *= displace_size;

  origin.xyz += dfront + dside + dup;
  gl_Position = projection * origin;
  fog_coord = gl_Position.z;

  v_texcoord = (position.xy + 1.0) * 0.5;
  v_color = color;
}
