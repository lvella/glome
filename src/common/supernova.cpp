#include "supernova.hpp"

Supernova::Supernova():
	t(0.0f),
	mesh(Mesh::get_mesh(ICOSPHERE))
{
	const char *source[] = {"supernova.vert", "world.frag", "no_texture.frag", NULL};
	shader.setup_shader(source);
	//slerp_sum = shader.get_uniform("slerp_sum");
	//slerp_mult = shader.get_uniform("slerp_mult");
}

Supernova::~Supernova()
{
	Mesh::release_mesh(mesh);
}

void Supernova::update()
{
	// Slerp parameters, explained in: http://math.stackexchange.com/a/99171/7721
	const Vector4 P(0.05f, 0.0f, 0.0f, -sqrtf(1.0f - 0.05*0.05));

	t += 0.1;
}

void Supernova::draw(Camera &c)
{

}
