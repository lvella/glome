uniform mat4 transform;
uniform mat4 projection;
uniform float half_width;

attribute vec4 position;
attribute vec4 color;
attribute float radius;

varying vec4 v_color;
varying float fog_coord;

// Maps a 4-D unit vector to euclidian 3-D space
void proj3d(inout vec4 v)
{
  v.xyz = v.xyz / (1.0 - v.w);
  v.w = 1.0;
}

// Given a radius and the W coordinate of the 4-D center of a sphere,
// calculate the new diameter after 4-D → 3-D projection
float radius_map(float r, float w)
{
  float center_angle = acos(w);
  float p1d = sin(center_angle + r) / (1.0f - cos(center_angle + r));
  float p2d = sin(center_angle - r) / (1.0f - cos(center_angle - r));

  return (p2d - p1d) * 0.5;
}

void main()
{
  vec4 border;
  vec4 center = transform * position;
  float proj_radius = radius_map(radius, center.w);

  proj3d(center);

  border = projection * vec4(center.x + proj_radius, center.yzw);
  gl_Position = center = projection * center;

  gl_PointSize = abs(center.x / center.w - border.x / border.w) * half_width;

  fog_coord = gl_Position.z;
  v_color = color;
}
