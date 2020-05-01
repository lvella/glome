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

	shader.setup_shader({
		"world/supernova.vert",
		"world/modelview.vert",
		"common/quaternion.vert",
		"world/supernova.frag",
		"world/world_fog.frag",
		"world/fog.frag",
		"world/noise3D.frag"
	});
	slerp_arc = shader.getUniform("slerp_arc");
	center = shader.getUniform("center");
	shader.enable();
	shader.getUniform("texbase").set(0);

	map_shader.setup_shader({
		"minimap/map_supernova.vert",
		"minimap/map_stuff.vert",
		"common/quaternion.vert",
		"minimap/minimap.frag",
		"common/no_texture.frag"
	});
	map_slerp_arc = map_shader.getUniform("slerp_arc");

	// Better place to start...
	Vector4 rot_dir = Random::direction();
	_t = Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1,
		0, 0, -1, 0
	) * rotation(Random::arc(), rot_dir.x, rot_dir.y, rot_dir.z);

	update(0);
}

Supernova::~Supernova()
{
	Mesh::release_mesh(map_mesh);
	Mesh::release_mesh(mesh);
}

void Supernova::update(float dt)
{
	// Based on http://math.stackexchange.com/a/99171/7721
	// Expanding rate; 0 is collapsed at origin, M_PI is
	// collapsed at opposite pole.
	size += dt * 0.03;

	slerp[0] = sinf(size);
	slerp[1] = cosf(size);

	// Spin
	_t = _t * xy_matrix(slerp[1] * dt * 0.4);
}

void Supernova::draw(Camera &c)
{
	c.pushShader(&shader);

	c.pushMultQRot(_t);

	{
		// Calculate the center of the projected sphere, to use in the yellow gloom effect.
		// TODO: For a better effect, increase the LOD if the player gets close enough.
		Vector4 pos = c.transformation().position();
		float center_angle = acosf(pos.w);
		float p1d = sinf(center_angle + size) / (1.0f - cosf(center_angle + size));
		float p2d = sinf(center_angle - size) / (1.0f - cosf(center_angle - size));
		float dist = (p1d + p2d) / 2.0f;
		float r = (p2d - p1d) / 2.0f;

		Vector3 vcenter = pos.stereo_proj();
		float len = vcenter.length();
		vcenter = vcenter * (dist / len);

		// TODO: For better performance, use a different shader when r < 0.
		center.set(Vector4(vcenter, powf(1.4f, -r) * 0.3f));
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
	c.pushMultQRot(_t);

	map_slerp_arc.set(slerp);
	glVertexAttrib3f(map_shader.colorAttr(), 1.0f, 1.0f, 1.0f);
	map_mesh->draw(c);

	c.popMat();
	c.popShader();
}
