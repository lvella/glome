#include "supernova.hpp"

Supernova::Supernova():
	t(0.0f),
	mesh(Mesh::get_mesh(ICOSPHERE))
{
	const char *source[] = {"supernova.vert", "supernova.frag", "fog.frag", NULL};
	shader.setup_shader(source);
	slerp_arc = shader.getUniform("slerp_arc");

	// Better place to start...
	_t = Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 1,
			0, 0, 1, 0
	);

	update();
}

Supernova::~Supernova()
{
	Mesh::release_mesh(mesh);
}

void Supernova::update()
{
	// Based on http://math.stackexchange.com/a/99171/7721
	// Expanding rate; 0 is collapsed at origin, M_PI is
	// collapsed at opposite pole.
	t += 0.0001;

	slerp[0] = sinf(t);
	slerp[1] = cosf(t);
}

void Supernova::draw(Camera &c)
{
	c.pushShader(&shader);
	c.pushMultMat(_t);

	slerp_arc.set(slerp);
	mesh->draw(c);

	c.popMat();
	c.popShader();
}
