#version 150 core

attribute vec4 position;
attribute vec4 texcoord;
attribute vec4 color;

uniform bool has_tex;

varying vec2 v_texcoord;
varying vec4 v_color;

vec3 project_vertex(vec4 vert);
vec4 color_blend(vec4 color, float z);

void main()
{
  const vec4 center = vec4(0.0, 0.0, 0.0, -1.0);
  vec3 v;

  if(has_tex) {
  	v = project_vertex(center);
    v.xy = v.xy + position.xy * 0.045;
    v_texcoord = (1.0 + position.xy) * 0.5;
  } else {
    v = project_vertex(position);
    v_texcoord = texcoord.st;
  }

  v_color = color_blend(color, v.z);

  gl_Position = vec4(v.xy, -v.z, 1.0);
  gl_PointSize = 1.0;
}
