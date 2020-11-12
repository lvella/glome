#include "projectile.hpp"

#include <ctime>
#include <algorithm>
#include <memory>
#include <vector>

#include "drawable.hpp"
#include "shader.hpp"
#include "math.hpp"
#include "textures.hpp"
#include "initialization.hpp"
#include "updatable.hpp"

using namespace std;

namespace
{

GLuint tex_projectile;

GLuint vbo;
GLuint texture;
GLint uniform_has_tex;
CamShader program_bullet;

class ProjectileSpecs: public DrawSpecs<ProjectileSpecs>
{
public:
	template<class Token> ProjectileSpecs(const Token& t):
		DrawSpecs(t)
	{
		{
			const float data[] = {
				1.0f, 0.78f, 0.59f,
				1.0f, 1.0f,

				0.59f, 1.0f, 0.59f,
				1.0f, -1.0f,

				0.51f, 0.39f, 0.98f,
				-1.0f, -1.0f,

				1.0f, 0.59f, 0.59f,
				-1.0f, 1.0f
			};

			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(data), data,
				GL_STATIC_DRAW);
		}

		create_spherical_texture(64, texture);

		program_bullet.setup_shader({
			"world/projectile.vert",
			"world/modelview.vert",
			"common/quaternion.vert",
			"world/world.frag",
			"common/luminance_alpha_texture.frag",
			"world/world_fog.frag",
			"world/fog.frag"
		});

		uniform_has_tex = glGetUniformLocation(
			program_bullet.program(), "has_tex");
	}

	void setup_draw_state(Camera& c) override
	{
		c.setShader(&program_bullet);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glUniform1i(uniform_has_tex, 1);
		glBindTexture(GL_TEXTURE_2D, texture);

		glEnableVertexAttribArray(Shader::ATTR_COLOR);

		glVertexAttribPointer(Shader::ATTR_POSITION, 2, GL_FLOAT,
				      GL_FALSE, 5 * sizeof(float),
				      (float *)0 + 3);
		glVertexAttribPointer(Shader::ATTR_COLOR, 3, GL_FLOAT,
				      GL_FALSE, 5 * sizeof(float), (float *)0);
	}

	void shutdown_draw_state(Camera&) override
	{
		glDisableVertexAttribArray(Shader::ATTR_COLOR);
	}
};

}

Projectile::Projectile(const std::shared_ptr<Scorer>& s,
	const QRot& from, float speed):
    Object(from), VolSphere(0.004),
    speed(speed),
    ttl(0.0), max_ttl((2 * math::pi - 0.05) / speed),
    max_ttl_2(max_ttl / 2), alpha(255u),
    scorer(s)
{
}

void Projectile::draw(Camera & c)
{
	c.setQRot(get_t());
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Projectile::collided_with(const Collidable& other, float)
{
	die();
}

bool Projectile::update(float dt, UpdatableAdder&)
{
	ttl += dt;
	alpha = ttl < (max_ttl_2) ?
		255u : 255u - uint8_t((ttl - max_ttl_2) * 200.0 / max_ttl_2);

	mul_t(zw_qrot(-speed * dt));

	return !is_dead();
}

bool Projectile::is_transparent() const
{
	return true;
}

DrawSpecsBase& Projectile::get_draw_specs() const
{
	return MandatorySingleton::get_instance<ProjectileSpecs>();
}
