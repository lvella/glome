#include "projectile.hpp"

#include <ctime>
#include <algorithm>
#include <memory>
#include <vector>

#include "shader.hpp"
#include "math.hpp"
#include "textures.hpp"
#include "initialization.hpp"

using namespace std;

static GLuint tex_projectile;

static GLuint vbo;
static GLuint texture;
static GLint uniform_has_tex;
static CamShader program_bullet;

static GLuint minimap_vbo;
static std::vector < Vector4 > minimap_buf;

Projectile::SList Projectile::shots;

static RegisterInitialization ini{[]
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

		GLuint vbos[2];

		glGenBuffers(2, vbos);
		vbo = vbos[0];
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data,
			     GL_STATIC_DRAW);

		minimap_vbo = vbos[1];
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
}};

void Projectile::shot(const std::shared_ptr<Scorer>& s, const QRot& from, float speed)
{
	// TODO: find a non-hackish way to use emplace_front...
	shots.push_front(Projectile{s, from, speed});
}

void Projectile::update_all(float dt)
{
	shots.remove_if([dt](Projectile& p) {
		if(p.is_dead()) {
			return true;
		}
		p.update(dt);
		return false;
	});

	// Updates the buffer that will be drawn in minimap
	if (shots.size() > 0) {
		bool resized = false;

		// If not big enough, increase the buffer size
		if (minimap_buf.size() < shots.size()) {
			minimap_buf.resize(shots.size());
			resized = true;
		}

		{
			size_t i = 0;
			for (auto& e: shots) {
				minimap_buf[i++] = e.position();
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, minimap_vbo);
		if (resized) {
			glBufferData(GL_ARRAY_BUFFER,
				     minimap_buf.size() * sizeof(Vector4),
				     &minimap_buf[0], GL_STREAM_DRAW);
		} else {
			glBufferSubData(GL_ARRAY_BUFFER, 0,
					shots.size() * sizeof(Vector4),
					&minimap_buf[0]);
		}
	}
}

std::vector<Collidable *>
Projectile::get_collision_volumes()
{
	std::vector<Collidable *> ret(shots.size());
	std::transform(shots.begin(), shots.end(), ret.begin(),
		[](Projectile& p) {
			return &p;
		}
	);

	return ret;
}

std::vector <Projectile *>
Projectile::cull_sort_from_camera(const Camera & cam)
{
	struct Sortable {
		Projectile *proj;
		float dist;

		Sortable(Projectile * p, float d):proj(p), dist(d) {
		} bool operator<(const Sortable & other) const {
			return dist > other.dist;
	}};

	std::vector < Sortable > to_sort;
	to_sort.reserve(shots.size());

	for(auto & shot: shots) {
		Vector4 pos = cam.getBaseTransformation() * shot.position();
		if (pos[2] <= 0) {
			to_sort.emplace_back(&shot, pos.squared_length());
		}
	}
	std::sort(to_sort.begin(), to_sort.end());

	std::vector < Projectile * >ret;
	ret.reserve(to_sort.size());
	for (auto & e:to_sort) {
		ret.push_back(e.proj);
	}

	return ret;
}

void Projectile::draw_many(const std::vector < Projectile * >&shots, Camera & c)
{
	if (shots.size() != 0) {
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

		for(auto i: shots)
			i->draw(c);

		glDisableVertexAttribArray(Shader::ATTR_COLOR);
	}
}

void Projectile::draw_in_minimap()
{
	if (shots.size() > 0) {
		assert(minimap_buf.size() >= shots.size());
		glBindBuffer(GL_ARRAY_BUFFER, minimap_vbo);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
		glDrawArrays(GL_POINTS, 0, shots.size());
	}

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

void Projectile::update(float dt)
{
	ttl += dt;
	alpha = ttl < (max_ttl_2) ?
		255u : 255u - uint8_t((ttl - max_ttl_2) * 200.0 / max_ttl_2);

	mul_t(zw_qrot(-speed * dt));
}
