uniform mat4 camera;
uniform mat4 projection;
uniform float size;
uniform vec4 coord;

attribute vec4 position;
attribute vec4 color;

varying vec2 v_texcoord;
varying vec4 v_color;
varying float fog_coord;

void main()
{
  vec4 tmp = camera * coord;
  
  // Ortographic projection to 3-D
  tmp.xyz = tmp.xyz / (1.0 - tmp.w);
  tmp.w = 1.0;
	
	//TODO: Correct 4D perspective
	tmp += position*size;

  gl_Position = projection * tmp;
  fog_coord = gl_Position.z;

	v_texcoord = (position.xy + 1.0)*0.5;
  v_color = color;
}
