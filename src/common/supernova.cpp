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
	const float ANGLE = M_PI / 20.0f;
	slerp_s = Vector4::CANONICAL * (sinf((1 - t) * ANGLE) / sinf(ANGLE));
	slerp_m = sinf(t * ANGLE) / sinf(ANGLE);
	t += 0.01;
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
