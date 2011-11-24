attribute vec4 position;
attribute vec4 texcoord;
attribute vec4 color;

uniform mat4 camera;
uniform mat4 transform;
uniform bool has_tex;

varying vec2 v_texcoord;
varying vec4 v_color;

vec3 project_vertex(vec4 vert) {
  const float two_by_pi = 0.63661977236758134307553505349005744813783858296183;
  vec3 ret;
  float r, R;
  
  ret = vert.xyz / (1.0 - vert.w);
  R = length(ret);
  r = atan(R) * two_by_pi;
  ret = ret * (r / R);
  
  return ret;
}

void main()
{
  const vec4 center = vec4(0.0, 0.0, 0.0, -1.0);
  const vec4 pale_color = vec4(0.25, 0.65, 0.25, 1.0);
  vec3 v;

  if(has_tex) {
  	v = project_vertex(camera * (transform * center));
    v.xy = v.xy + position.xy * 0.045;
    v_texcoord = (1.0 + position.xy) * 0.5;
  } else {
    v = project_vertex(camera * (transform * position));
    v_texcoord = texcoord.st;
  }

  v_color = mix(pale_color, color, (v.z + 1.0) * 0.5);

  gl_Position = vec4(v.xy, -v.z, 1.0);
}
