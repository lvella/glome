#version 150 core

in vec4 position;
in vec4 texcoord;
in vec4 color;

out vec2 v_texcoord;
out vec4 v_color;

// Simplest no projection shader.
// Just for HUD.
void main()
{
	gl_Position = position;
	v_texcoord = (position.xy + 1.0) * 0.5;
	v_color = color;
}
