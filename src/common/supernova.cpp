#include <iostream>

#include "supernova.hpp"

Supernova::Supernova():
	t(0.0f),
	mesh(Mesh::get_mesh(ICOSPHERE))
{
	const char *source[] = {"supernova.vert", "world.frag", "no_texture.frag", NULL};
	shader.setup_shader(source);
	slerp_sum = shader.getUniform("slerp_sum");
	slerp_mult = shader.getUniform("slerp_mult");

	update();
	std::cout << _t;
}

Supernova::~Supernova()
{
	Mesh::release_mesh(mesh);
}

void Supernova::update()
{
	// Based on http://math.stackexchange.com/a/99171/7721
	slerp_s = Vector4::CANONICAL * cosf(t);
	slerp_m = sinf(t);
	t += 0.0001; // Expanding rate; 0 is collapsed at origin, M_PI is collapsed at opposite pole.
}

void Supernova::draw(Camera &c)
{
	c.pushMult(_t);
	c.setShader(&shader);
	slerp_sum.set(slerp_s);
	slerp_mult.set(slerp_m);
	mesh->draw(c);
	c.pop();
}
