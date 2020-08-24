#include "math.hpp"
#include "random.hpp"

#include "textures.hpp"
#include "supernova.hpp"
#include <memory>

namespace {

std::shared_ptr<Mesh> mesh;
CamShader shader;
Uniform slerp_arc;
Uniform center;

class SupernovaSpecs: public DrawSpecs<SupernovaSpecs>
{
public:
	template<class Token> SupernovaSpecs(const Token& t):
		DrawSpecs(t)
	{
		bg_noise = create_noise_texture(800, 600, 1.0f / 50.0f, Vector2(Random::arc(), Random::arc()) * 20.0f);

		mesh = Mesh::get_mesh(Mesh::ICOSPHERE);

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
	}

	void setup_draw_state(Camera& c) override
	{
		c.setShader(&shader);
		glBindTexture(GL_TEXTURE_2D, bg_noise);
	}

	void shutdown_draw_state(Camera&) override
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

private:
	GLuint bg_noise;
};

}

Supernova::Supernova():
	VolSphere(0.0f),
	map_mesh(Mesh::get_mesh(Mesh::UVSPHERE))
{
	// TODO: initialize this stuff only once

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

bool Supernova::update(float dt, UpdatableAdder&)
{
	// Based on http://math.stackexchange.com/a/99171/7721
	// Expanding rate; 0 is collapsed at origin, M_PI is
	// collapsed at opposite pole.
	float radius = get_radius();
	radius += dt * 0.03;
	set_radius(radius);

	slerp[0] = std::sin(radius);
	slerp[1] = std::cos(radius);

	// Spin
	mul_t(xy_qrot(slerp[1] * dt * 0.4));

	return true;
}

void Supernova::draw(Camera &c)
{
	{
		const QRot trans = c.setQRot(get_t());

		// Calculate the center of the projected sphere, to use in the yellow gloom effect.
		// TODO: For a better effect, increase the LOD if the player gets close enough.
		const Vector4 pos = trans.position();
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

	mesh->draw(c);
}

DrawSpecsBase& Supernova::get_draw_specs() const
{
	return DrawSpecsBase::get_instance<SupernovaSpecs>();
}

void Supernova::minimap_draw(Camera &c)
{
	const SpaceShader* prev = c.getShader();

	c.setShader(&map_shader);
	c.setQRot(get_t());

	map_slerp_arc.set(slerp);
	glVertexAttrib3f(Shader::ATTR_COLOR, 1.0f, 1.0f, 1.0f);
	map_mesh->draw(c);

	c.setShader(prev);
}

void Supernova::collided_with(const Collidable& other, float)
{
}

bool Supernova::is_transparent() const
{
	return get_radius() < math::pi_2;
}
