#include "gl.hpp"
#include "world.hpp"

void
World::initialize()
{
	const char *sources[] = { "world.vert", "world.frag", "no_texture.frag", NULL };

	shader.setup_shader(sources);

	shader_uniform_projection = glGetUniformLocation(shader.program(), "projection");
	shader_uniform_camera = glGetUniformLocation(shader.program(), "camera");
}

