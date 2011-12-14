#pragma once

#include "shader.hpp"
#include "matrix4.hpp"
#include "controller.hpp"
#include "render.hpp"
#include "ship.hpp"

/** Every game mode should derive this class. */
class world
{
public:
	static void initialize();
	static void load_attr_format();

protected:
	controller* _ctrl;
	render* _render;
	std::vector<ship*> players;

	static Shader shader;
	static GLint shader_uniform_camera;
	static GLint shader_uniform_projection;
};

