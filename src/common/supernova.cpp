#include "math.hpp"
#include "random.hpp"

#include "textures.hpp"
#include "supernova.hpp"

Supernova::Supernova():
	size(0.0f),
	mesh(Mesh::get_mesh(Mesh::ICOSPHERE)),
	map_mesh(Mesh::get_mesh(Mesh::UVSPHERE))
{
	// TODO: initialize this stuff only once
	bg_noise = create_noise_texture(800, 600, 1.0f / 50.0f, Vector2(Random::arc(), Random::arc()) * 20.0f);

	const char *source[] = {"supernova.vert", "supernova.frag", "fog.frag", "noise3D.frag", nullptr};
	shader.setup_shader(source);
	slerp_arc = shader.getUniform("slerp_arc");
	center = shader.getUniform("center");
	shader.enable();
	shader.getUniform("texbase").set(0);

	const char *map_src[] = {"map_supernova.vert", "map_stuff.vert", "minimap.frag", "no_texture.frag", nullptr};
	map_shader.setup_shader(map_src);
	map_slerp_arc = map_shader.getUniform("slerp_arc");

	// Better place to start...
	_t = Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 1,
			0, 0, 1, 0
	);

	Vector4 rot_dir = Random::direction();
	_t = _t * rotation(Random::arc(), rot_dir.x, rot_dir.y, rot_dir.z);

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
	size += 0.0005;

	slerp[0] = sinf(size);
	slerp[1] = cosf(size);

	// Spin
	_t = _t * xy_matrix(slerp[1] * 0.002);
}

void Supernova::draw(Camera &c)
{
	c.pushShader(&shader);
	c.pushMultMat(_t);

	{
		// Calculate the center of the projected sphere, to use in the yellow gloom effect.
		// TODO: For a better effect, increase the LOD if the player gets close enough.
		float center_angle = acosf(-c.transformation()[3][3]);
		float p1d = sinf(center_angle + size) / (1.0f - cosf(center_angle + size));
		float p2d = sinf(center_angle - size) / (1.0f - cosf(center_angle - size));
		float dist = (p1d + p2d) / 2.0f;
		float r = (p2d - p1d) / 2.0f;

		Vector4 vcenter = c.transformation().position().stereo_proj();
		float len = sqrtf(vcenter.x*vcenter.x + vcenter.y*vcenter.y + vcenter.z*vcenter.z);
		vcenter *= dist / len;

		// TODO: For better performance, use a different shader when r < 0.
		vcenter.w = powf(1.4f, -r) * 0.3f;
		center.set(vcenter);
	}

	slerp_arc.set(slerp);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bg_noise);

	mesh->draw(c);

	glDisable(GL_TEXTURE_2D);

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
