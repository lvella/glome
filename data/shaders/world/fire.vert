#version 150 core

uniform mat4 projection;
uniform float half_width;

in vec4 position;
in vec4 color;
in float radius;

out vec4 v_color;
out float fog_coord;

// External functions
vec4 to_4d_eye(in vec4 v);
vec4 proj3d(in vec4 v);

// Given a radius and the W coordinate of the 4-D center of a sphere,
// calculate the new diameter after 4-D → 3-D projection
float radius_map(float r, float w)
{
  float center_angle = acos(w);
  float p1d = sin(center_angle + r) / (1.0 - cos(center_angle + r));
  float p2d = sin(center_angle - r) / (1.0 - cos(center_angle - r));

  return (p2d - p1d) * 0.5;
}

void main()
{
  vec4 center = to_4d_eye(position);
  float proj_radius = radius_map(radius, center.w);

  center = proj3d(center);

  vec4 border = projection * vec4(center.x + proj_radius, center.yzw);
  gl_Position = center = projection * center;

  gl_PointSize = abs(center.x / center.w - border.x / border.w) * half_width;

  fog_coord = gl_Position.z;
  v_color = color;
}
