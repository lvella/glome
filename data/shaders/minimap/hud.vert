attribute vec4 position;
attribute vec4 texcoord;
attribute vec4 color;

varying vec2 v_texcoord;
varying vec4 v_color;

// Simplest no projection shader.
// Just for HUD.
void main()
{
	gl_Position = position;
	v_texcoord = (position.xy + 1.0) * 0.5;
	v_color = color;
}
