#include <ctime>
#include <cmath>
#include <algorithm>
#include <vector>
#include <deque>

#include "shader.hpp"
#include "math.hpp"
#include "textures.hpp"

#include "projectile.hpp"

using namespace std;
typedef std::deque<Projectile> SList;
static SList shots;

static GLuint tex_projectile;

static GLuint vbo;
static GLuint texture;
static GLint uniform_has_tex;
static GLint uniform_camera;
static GLint uniform_projection;
static CamShader program_bullet;

static GLuint minimap_vbo;
static std::vector<Vector4> minimap_buf;

void Projectile::initialize()
{
	const char *source[] = {"projectile.vert", "world.frag", "texture.frag", "fog.frag", NULL};

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

		minimap_vbo = vbos[1];
	}

	create_spherical_texture(64, texture);

	program_bullet.setup_shader(source);
	uniform_has_tex = glGetUniformLocation(program_bullet.program(), "has_tex");
	uniform_camera = glGetUniformLocation(program_bullet.program(), "camera");
	uniform_projection = glGetUniformLocation(program_bullet.program(), "projection");
}

void Projectile::shot(ShipController *s, const Matrix4& from, float speed)
{
  // TODO: find a non-hackish way to use emplace_back...
  shots.push_front(Projectile(s, from, speed));
}

void Projectile::update_all()
{
    {
        auto i = shots.begin();
	    for(; i != shots.end(); ++i)
	    {
		    if(i->dead())
                // Assumes all projectiles beyond this
                // point are dead, i.e. they all have the
                // same initial TTL.
                break;
            i->update();
	    }

	    shots.erase(i, shots.end());
    }

	// Updates the buffer that will be drawn in minimap
	if(shots.size() > 0) {
		bool resized = false;

		// If not big enough, increase the buffer size
		if(minimap_buf.size() < shots.size()) {
			minimap_buf.resize(shots.size());
			resized = true;
		}

		for(size_t i = 0; i < shots.size(); ++i)
		{
			minimap_buf[i] = shots[i].transformation().position();
		}

		glBindBuffer(GL_ARRAY_BUFFER, minimap_vbo);
		if(resized) {
			glBufferData(GL_ARRAY_BUFFER, minimap_buf.size()*sizeof(Vector4), &minimap_buf[0], GL_STREAM_DRAW);
        }
		else {
			glBufferSubData(GL_ARRAY_BUFFER, 0, shots.size()*sizeof(Vector4), &minimap_buf[0]);
        }
    }
}

std::vector<Projectile*> Projectile::cull_sort_from_camera(const Camera& cam)
{
    struct Sortable {
        Projectile* proj;
        float dist;

        Sortable(Projectile* p, float d):
            proj(p), dist(d)
        {}

        bool operator<(const Sortable& other) const {
            return dist > other.dist;
        }
    };

    std::vector<Sortable> to_sort;
    to_sort.reserve(shots.size());

    for(auto &shot: shots) {
        Vector4 pos = cam.transformation() * shot._t.position();
        if(pos[2] <= 0) {
            to_sort.emplace_back(&shot, pos.squared_length());
        }
    }
    std::sort(to_sort.begin(), to_sort.end());

    std::vector<Projectile*> ret;
    ret.reserve(to_sort.size());
    for(auto &e: to_sort) {
        ret.push_back(e.proj);
    }

    return ret;
}

void Projectile::draw_many(const std::vector<Projectile*>& shots, Camera& c)
{
	if(shots.size() != 0) {
		c.pushShader(&program_bullet);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glUniform1i(uniform_has_tex, 1);
		glBindTexture(GL_TEXTURE_2D, texture);

		glEnableVertexAttribArray(program_bullet.colorAttr());

		glVertexAttribPointer(program_bullet.posAttr(), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0 + 3);
		glVertexAttribPointer(program_bullet.colorAttr(), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)0);

		for(auto i: shots)
			i->draw(c);

		glDisableVertexAttribArray(program_bullet.colorAttr());
		c.popShader();
	}
}

void Projectile::draw_in_minimap()
{
    if(shots.size() > 0) {
        assert(minimap_buf.size() >= shots.size());
	    glBindBuffer(GL_ARRAY_BUFFER, minimap_vbo);
	    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	    glDrawArrays(GL_POINTS, 0, shots.size());
    }
}

bool Projectile::collide(const Vector4& position, float radius)
{
  radius *= radius;

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i)
    if((position - i->transformation().position()).squared_length() < radius) {
      i->die();
      return true;
    }

  return false;
}

/*bool Projectile::collide(ShipController *s)
{
  const float r = 0.01f * 0.01f;
  Vector4 p = s->transformation().position();

  for(SList::iterator i = shots.begin(); i != shots.end(); ++i) {
    if(s != i->owner && (p - i->transformation().position()).squared_length() < r) {
      i->die();
      return true;
    }
  }

  return false;
}*/

Projectile::Projectile(ShipController *s, const Matrix4& from, float speed):
  Object(from),
  ds(zw_matrix(-speed)),
  owner(s),
  ttl(0),
  max_ttl((2 * M_PI - 0.05) / speed),
  max_ttl_2(max_ttl / 2),
  alpha(255u)
{
}

void Projectile::draw(Camera& c)
{
	c.pushMultMat(_t);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	c.popMat();
}

void Projectile::update()
{
  ++ttl;
  alpha = ttl < (max_ttl_2) ? 255u : 255u - (ttl - max_ttl_2) * 200 / max_ttl_2;

  _t = _t * ds;
}
