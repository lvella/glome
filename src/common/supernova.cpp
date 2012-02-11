#include "supernova.hpp"

Supernova::Supernova():
	t(0.0f),
	mesh(Mesh::get_mesh(ICOSPHERE)),
	map_mesh(Mesh::get_mesh(UVSPHERE))
{
	const char *source[] = {"supernova.vert", "supernova.frag", "fog.frag", NULL};
	shader.setup_shader(source);
	slerp_arc = shader.getUniform("slerp_arc");
	center = shader.getUniform("center");

	const char *map_src[] = {"map_supernova.vert", "map_stuff.vert", "minimap.frag", "no_texture.frag", NULL};
	map_shader.setup_shader(map_src);
	map_slerp_arc = map_shader.getUniform("slerp_arc");

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
	Mesh::release_mesh(map_mesh);
	Mesh::release_mesh(mesh);
}

void Supernova::update()
{
	// Based on http://math.stackexchange.com/a/99171/7721
	// Expanding rate; 0 is collapsed at origin, M_PI is
	// collapsed at opposite pole.
	t += 0.0005;

	slerp[0] = sinf(t);
	slerp[1] = cosf(t);
}

void Supernova::draw(Camera &c)
{
	c.pushShader(&shader);
	c.pushMultMat(_t);

	Vector4 vcenter = (c.transformation() * Vector4::ORIGIN).ortho_proj();

	slerp_arc.set(slerp);
	center.set(vcenter);
	mesh->draw(c);

	c.popMat();
	c.popShader();
}

void Supernova::minimap_draw(Camera &c)
{
	c.pushShader(&map_shader);
	c.pushMultMat(_t);

	map_slerp_arc.set(slerp);
	glVertexAttrib3f(map_shader.colorAttr(), 1.0f, 1.0f, 1.0f);
	map_mesh->draw(c);

	c.popMat();
	c.popShader();
}
