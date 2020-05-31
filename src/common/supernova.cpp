#include "math.hpp"
#include "random.hpp"

#include "textures.hpp"
#include "supernova.hpp"

Supernova::Supernova():
	VolSphere(0.0f),
	mesh(Mesh::get_mesh(Mesh::Type::ICOSPHERE)),
	map_mesh(Mesh::get_mesh(Mesh::Type::UVSPHERE))
{
	transparent = true;

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
	set_t(zw_qrot(-math::pi_2) * qrotation(Random::arc(), Random::direction()));
}

Supernova::~Supernova()
{
	Mesh::release_mesh(map_mesh);
	Mesh::release_mesh(mesh);
}

bool Supernova::update(float dt, UpdatableAdder&)
{
	// Based on http://math.stackexchange.com/a/99171/7721
	// Expanding rate; 0 is collapsed at origin, M_PI is
	// collapsed at opposite pole.
	float radius = get_radius();
	radius += dt * 0.03;
	set_radius(radius);

	transparent = radius < math::pi_2;

	slerp[0] = std::sin(radius);
	slerp[1] = std::cos(radius);

	// Spin
	mul_t(xy_qrot(slerp[1] * dt * 0.4));

	return true;
}

void Supernova::draw(Camera &c)
{
	c.pushShader(&shader);

	c.pushMultQRot(get_t());

	{
		// Calculate the center of the projected sphere, to use in the yellow gloom effect.
		// TODO: For a better effect, increase the LOD if the player gets close enough.
		Vector4 pos = c.transformation().position();
		const float radius = get_radius();
		float center_angle = acosf(pos.w);
		float p1d = sinf(center_angle + radius) / (1.0f - cosf(center_angle + radius));
		float p2d = sinf(center_angle - radius) / (1.0f - cosf(center_angle - radius));
		float dist = (p1d + p2d) / 2.0f;
		float r = (p2d - p1d) / 2.0f;

		Vector3 vcenter = pos.stereo_proj();
		float len = vcenter.length();
		vcenter = vcenter * (dist / len);

		// TODO: For better performance, use a different shader when r < 0.
		center.set(Vector4(vcenter, powf(1.4f, -r) * 0.3f));
	}

	slerp_arc.set(slerp);

	glBindTexture(GL_TEXTURE_2D, bg_noise);

	mesh->draw(c);

	c.popMat();
	c.popShader();
}

void Supernova::minimap_draw(Camera &c)
{
	c.pushShader(&map_shader);
	c.pushMultQRot(get_t());

	map_slerp_arc.set(slerp);
	glVertexAttrib3f(map_shader.colorAttr(), 1.0f, 1.0f, 1.0f);
	map_mesh->draw(c);

	c.popMat();
	c.popShader();
}

void Supernova::collided_with(const Collidable& other, float)
{
}
